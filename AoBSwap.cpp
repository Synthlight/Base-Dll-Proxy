#include "pch.h"

#include "ScanMemory.h"

#include "AoBSwap.h"

const std::vector<BYTE> ONE_BYTE_NOP   = StringToByteVector("90");
const std::vector<BYTE> TWO_BYTE_NOP   = StringToByteVector("66 90");
const std::vector<BYTE> THREE_BYTE_NOP = StringToByteVector("0F 1F 00");
const std::vector<BYTE> FOUR_BYTE_NOP  = StringToByteVector("0F 1F 40 00");
const std::vector<BYTE> FIVE_BYTE_NOP  = StringToByteVector("0F 1F 44 00 00");
const std::vector<BYTE> SIX_BYTE_NOP   = StringToByteVector("66 0F 1F 44 00 00");
const std::vector<BYTE> SEVEN_BYTE_NOP = StringToByteVector("0F 1F 80 00 00 00 00");
const std::vector<BYTE> EIGHT_BYTE_NOP = StringToByteVector("0F 1F 84 00 00 00 00 00");
const std::vector<BYTE> NINE_BYTE_NOP  = StringToByteVector("66 0F 1F 84 00 00 00 00 00");

AoBSwap::AoBSwap(const std::string& targetModule, const std::vector<BYTE>& bytesToFind, const std::vector<BYTE>& bytesToReplace) {
    this->targetModule_   = targetModule;
    this->bytesToFind_    = bytesToFind;
    this->bytesToReplace_ = bytesToReplace;
}

std::vector<const BYTE*> AoBSwap::ScanAndPatch() const {
    auto addressesFound = ScanMemory(targetModule_, bytesToFind_);

    for (auto address : addressesFound) {
        auto size = bytesToReplace_.size();

        DoWithProtect(const_cast<BYTE*>(address), size, [address, size, this]() {
            memcpy(const_cast<BYTE*>(address), bytesToReplace_.data(), size);
        });
    }

    return addressesFound;
}

std::vector<BYTE> IntToByteArray(const UINT64 value) {
    auto buffer = std::vector<BYTE>(sizeof(UINT64));
    for (int i = 0; i < sizeof(UINT64); i++) {
        buffer[i] = value >> 8 * i & 0xFF;
    }
    return buffer;
}

std::vector<BYTE> IntToByteArray(const UINT32 value) {
    auto buffer = std::vector<BYTE>(sizeof(UINT32));
    for (int i = 0; i < sizeof(UINT32); i++) {
        buffer[i] = value >> 8 * i & 0xFF;
    }
    return buffer;
}

std::vector<BYTE> CreateCallBytesToAddress(const BYTE* targetAddress, const BYTE* fromAddress) {
    // First check if we're close enough to jump via an 8 byte offset.
    const auto offset = static_cast<int>(targetAddress - (fromAddress + 5)); // +5 for the call op.
    if (std::abs(offset) < INT32_MAX) {
        // Small enough to do a relative jump.
        auto callBytes = std::vector<BYTE>{0xE8};
        for (auto byte : IntToByteArray(static_cast<UINT32>(offset))) {
            callBytes.push_back(byte);
        }
        return callBytes;
    }

    // Create `call` bytes. e.g. FF15 02000000 EB08 30A08D2100000000 - call 218DA030
    auto replacementBytes = std::vector<BYTE>{0xFF, 0x15, 0x02, 0x00, 0x00, 0x00, 0xEB, 0x08}; // x64 long call, add target address to the end (8 bytes).
    for (auto byte : IntToByteArray(reinterpret_cast<UINT64>(targetAddress))) {
        replacementBytes.push_back(byte);
    }
    return replacementBytes;
}

std::vector<std::string> SplitStringBySpace(const std::string& input) {
    auto                     inStream = std::istringstream(input);
    std::vector<std::string> outStrings;
    std::string              s;
    while (getline(inStream, s, ' ')) {
        outStrings.push_back(s);
    }
    return outStrings;
}

std::vector<BYTE> StringToByteVector(const std::string& input) {
    const auto        stringBytes = SplitStringBySpace(input);
    std::vector<BYTE> bytes;
    bytes.reserve(stringBytes.size());
    for (const auto& s : stringBytes) {
        bytes.push_back(static_cast<BYTE>(std::stoul(s, nullptr, 16)));
    }
    return bytes;
}