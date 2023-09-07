#include "pch.h"

#include "Logger.h"
#include "ScanMemory.h"

#include "AoBSwap.h"

std::vector<BYTE> oneByteNop   = StringToByteVector("90");
std::vector<BYTE> twoByteNop   = StringToByteVector("66 90");
std::vector<BYTE> threeByteNop = StringToByteVector("0F 1F 00");
std::vector<BYTE> fourByteNop  = StringToByteVector("0F 1F 40 00");
std::vector<BYTE> fiveByteNop  = StringToByteVector("0F 1F 44 00 00");
std::vector<BYTE> sixByteNop   = StringToByteVector("66 0F 1F 44 00 00");
std::vector<BYTE> sevenByteNop = StringToByteVector("0F 1F 80 00 00 00 00");
std::vector<BYTE> eightByteNop = StringToByteVector("0F 1F 84 00 00 00 00 00");
std::vector<BYTE> nineByteNop  = StringToByteVector("66 0F 1F 84 00 00 00 00 00");

AoBSwap::AoBSwap(const std::string targetModule, std::vector<BYTE>& bytesToFind, std::vector<BYTE>& bytesToReplace) {
    this->targetModule = targetModule;
    this->bytesToFind = bytesToFind;
    this->bytesToReplace = bytesToReplace;
}

std::vector<const BYTE*> AoBSwap::ScanAndPatch() {
    auto addressesFound = ScanMemory(targetModule, bytesToFind);

    for (auto address : addressesFound) {
        auto size = bytesToReplace.size();

        DoWithProtect((BYTE*) address, size, [address, size, this]() {
            memcpy((BYTE*) address, bytesToReplace.data(), size);
        });
    }

    return addressesFound;
}

std::vector<BYTE> IntToByteArray(const long long value, const int size) {
    auto buffer = std::vector<BYTE>(8);
    for (int i = 0; i < size; i++) {
        buffer[i] = (value >> (8 * i)) & 0xFF;
    }
    return buffer;
}

std::vector<BYTE> CreateCallBytesToAddress(const BYTE* targetAddress) {
    // Create `call` bytes. e.g. FF15 02000000 EB08 30A08D2100000000 - call 218DA030
    auto replacementBytes = std::vector<BYTE>{0xFF, 0x15, 0x02, 0x00, 0x00, 0x00, 0xEB, 0x08}; // x64 long call, add target address to the end (8 bytes).
    for (auto byte : IntToByteArray((long long) targetAddress, 8)) {
        replacementBytes.push_back(byte);
    }
    return replacementBytes;
}

std::vector<std::string> SpltStringBySpace(const std::string input) {
    std::istringstream inStream = std::istringstream(input);
    std::vector<std::string> outStrings;
    std::string s;
    while (getline(inStream, s, ' ')) {
        outStrings.push_back(s);
    }
    return outStrings;
}

std::vector<BYTE> StringToByteVector(const std::string input) {
    auto stringBytes = SpltStringBySpace(input);
    std::vector<BYTE> bytes;
    for (auto s : stringBytes) {
        bytes.push_back((BYTE) std::stoul(s.c_str(), nullptr, 16));
    }
    return bytes;
}