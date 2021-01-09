#include "pch.h"

#include "Logger.h"
#include "ScanMemory.h"

#include "AoBSwap.h"

std::vector<BYTE> oneByteNop   = std::vector<BYTE>{0x90};
std::vector<BYTE> twoByteNop   = std::vector<BYTE>{0x66, 0x90};
std::vector<BYTE> threeByteNop = std::vector<BYTE>{0x0F, 0x1F, 0x00};
std::vector<BYTE> fourByteNop  = std::vector<BYTE>{0x0F, 0x1F, 0x40, 0x00};
std::vector<BYTE> sixByteNop   = std::vector<BYTE>{0x66, 0x0F, 0x1F, 0x44, 0x00, 0x00};

AoBSwap::AoBSwap(std::string targetModule, std::vector<BYTE>& bytesToFind, std::vector<BYTE>& bytesToReplace) {
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

std::vector<BYTE> IntToByteArray(long long value, int size) {
    auto buffer = std::vector<BYTE>(8);
    for (int i = 0; i < size; i++) {
        buffer[i] = (value >> (8 * i)) & 0xFF;
    }
    return buffer;
}

std::vector<BYTE> CreateCallBytesToAddress(BYTE* targetAddress) {
    // Create `call` bytes. e.g. FF15 02000000 EB08 30A08D2100000000 - call 218DA030
    auto replacementBytes = std::vector<BYTE>{0xFF, 0x15, 0x02, 0x00, 0x00, 0x00, 0xEB, 0x08}; // x64 long call, add target address to the end (8 bytes).
    for (auto byte : IntToByteArray((long long) targetAddress, 8)) {
        replacementBytes.push_back(byte);
    }
    return replacementBytes;
}