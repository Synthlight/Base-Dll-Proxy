#include "pch.h"

#include "AoBSwap.h"

#include "Logger.h"
#include "ScanMemory.h"
#include "Util.h"

const std::vector<BYTE> RETURN_OP      = StringToByteVector("C3");
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
    for (UINT64 i = 0; i < sizeof(UINT64); i++) {
        buffer[i] = value >> 8 * i & 0xFF;
    }
    return buffer;
}

std::vector<BYTE> IntToByteArray(const UINT32 value) {
    auto buffer = std::vector<BYTE>(sizeof(UINT32));
    for (UINT32 i = 0; i < sizeof(UINT32); i++) {
        buffer[i] = value >> 8 * i & 0xFF;
    }
    return buffer;
}

std::vector<BYTE> CreateCallBytesToAddress(const BYTE* targetAddress, const BYTE* fromAddress) {
    // First check if we're close enough to jump via an 8 byte offset.
    const auto offset = static_cast<int>(targetAddress - (fromAddress + 5)); // +5 for the `call` op.
    if (std::abs(offset) < INT32_MAX) {
        // Small enough to do a relative call.
        auto callBytes = std::vector<BYTE>{0xE8}; // call
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

std::vector<BYTE> CreateJumpBytesToAddress(const BYTE* targetAddress, const BYTE* fromAddress, LogBuffer* logBuffer) {
    // First check if we're close enough to jump via an 8 byte offset.
    const auto offset = static_cast<int>(targetAddress - (fromAddress + 5)); // +5 for the `jmp` op.
    if (std::abs(offset) >= INT32_MAX) {
        LOG_BUFFER("Unable to create a `jmp` to an offset that exceeds INT32_MAX.");
        return {};
    }
    // Small enough to do a relative jump.
    auto callBytes = std::vector<BYTE>{0xE9}; // jmp
    for (auto byte : IntToByteArray(static_cast<UINT32>(offset))) {
        callBytes.push_back(byte);
    }
    return callBytes;
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

bool DoSimplePatch(const std::string& moduleName, const PTR_SIZE moduleAddress, const std::string& scanName, const std::string& scanBytes, const std::vector<BYTE>& newMemBytes, LogBuffer* logBuffer) {
    ScanOptions scanOptions;
    scanOptions.moduleName    = &moduleName;
    scanOptions.moduleAddress = moduleAddress;
    scanOptions.patchType     = PatchType::SIMPLE;
    scanOptions.scanBytes     = scanBytes;
    scanOptions.newMemBytes   = newMemBytes;
    return DoPatch(scanName, scanOptions, logBuffer);
}

bool DoInjectPatch(const std::string& moduleName, const PTR_SIZE moduleAddress, const std::string& scanName, const std::string& scanBytes, const PTR_SIZE originalOpSize, AllocateMemory* allocator, const std::vector<BYTE>& newMemBytes, LogBuffer* logBuffer) {
    ScanOptions scanOptions;
    scanOptions.moduleName     = &moduleName;
    scanOptions.moduleAddress  = moduleAddress;
    scanOptions.patchType      = PatchType::INJECT;
    scanOptions.scanBytes      = scanBytes;
    scanOptions.newMemBytes    = newMemBytes;
    scanOptions.originalOpSize = originalOpSize;
    scanOptions.allocator      = allocator;
    scanOptions.returnType     = ReturnType::RETURN;
    return DoPatch(scanName, scanOptions, logBuffer);
}

bool DoPatchInternal(const BYTE* const& injectAddress, ScanOptions scanOptions, LogBuffer* logBuffer) {
    LOG_BUFFER("Inject address: " << PRINT_RELATIVE_ADDRESS(*scanOptions.moduleName, scanOptions.moduleAddress, injectAddress));

    LOG_BUFFER("New mem bytes: " << BytesToString(scanOptions.newMemBytes));

    switch (scanOptions.patchType) {
        case PatchType::SIMPLE: //
            DoWithProtect(const_cast<BYTE*>(injectAddress), scanOptions.newMemBytes.size(), [injectAddress, scanOptions] {
                memcpy(const_cast<BYTE*>(injectAddress), scanOptions.newMemBytes.data(), scanOptions.newMemBytes.size()); // NOLINT(performance-no-int-to-ptr)
            }, logBuffer);
            return true;
        case PatchType::INJECT: //
            auto allocateSize = scanOptions.newMemBytes.size();
            switch (scanOptions.returnType) {
                case ReturnType::RETURN: allocateSize += 1;
                    break;
                case ReturnType::CALL:
                case ReturnType::JUMP: allocateSize += 5;
                    break;
            }

            const auto newMemStart = scanOptions.allocator->ReserveSpaceInAllocatedNewMem(allocateSize); // NOLINT(performance-no-int-to-ptr)
            LOG_BUFFER("New mem address: " << std::uppercase << std::hex << reinterpret_cast<const UINT64>(newMemStart));
            if (newMemStart == nullptr) {
                LOG_BUFFER("Error: New mem address is 0, aborting.");
                return false;
            }

            if (scanOptions.returnType == ReturnType::RETURN) {
                scanOptions.newMemBytes.push_back(0xC3); // ret
            } else if (scanOptions.returnType == ReturnType::JUMP || scanOptions.returnType == ReturnType::CALL) {
                const auto jumpOrCall = scanOptions.returnType == ReturnType::JUMP ? "jump" : "call";

                std::vector<BYTE> returnBytes;
                if (scanOptions.returnType == ReturnType::JUMP) {
                    returnBytes = CreateJumpBytesToAddress(scanOptions.returnAddress, static_cast<BYTE*>(newMemStart) + scanOptions.newMemBytes.size(), logBuffer);
                } else if (scanOptions.returnType == ReturnType::CALL) {
                    returnBytes = CreateCallBytesToAddress(scanOptions.returnAddress, static_cast<BYTE*>(newMemStart) + scanOptions.newMemBytes.size());
                }
                scanOptions.newMemBytes.insert(scanOptions.newMemBytes.end(), returnBytes.begin(), returnBytes.end());
                LOG_BUFFER("Wrote return " << jumpOrCall << " to: " << PRINT_RELATIVE_ADDRESS(*scanOptions.moduleName, scanOptions.moduleAddress, scanOptions.returnAddress));
                LOG_BUFFER("Return " << jumpOrCall << " bytes: " << BytesToString(returnBytes));
            }

            memcpy(newMemStart, scanOptions.newMemBytes.data(), scanOptions.newMemBytes.size());

            const auto jumpOrCall = scanOptions.jumpType == JumpType::JUMP ? "jump" : "call";

            std::vector<BYTE> callBytes; // Create a 'call' to inject to jump to our code.
            if (scanOptions.jumpType == JumpType::JUMP) {
                callBytes = CreateJumpBytesToAddress(static_cast<const BYTE*>(newMemStart), injectAddress, logBuffer);
            } else if (scanOptions.jumpType == JumpType::CALL) {
                callBytes = CreateCallBytesToAddress(static_cast<const BYTE*>(newMemStart), injectAddress);
            }
            if (callBytes.size() > scanOptions.originalOpSize) {
                LOG_BUFFER("Error: Generated " << jumpOrCall << " bytes are too long, aborting.");
                LOG_BUFFER("Generated " << jumpOrCall << " bytes: " << BytesToString(callBytes));
                return false;
            }
            while (callBytes.size() < scanOptions.originalOpSize) {
                callBytes.push_back(0x90); // nop
            }

            DoWithProtect(const_cast<BYTE*>(injectAddress), callBytes.size(), [injectAddress, callBytes] {
                memcpy(const_cast<BYTE*>(injectAddress), callBytes.data(), callBytes.size()); // NOLINT(performance-no-int-to-ptr)
            }, logBuffer);
            LOG_BUFFER("Wrote " << jumpOrCall << " to inject mem: " << BytesToString(callBytes));

            return true;
    }

    return false; // Not reachable.
}

bool DoPatch(const std::string& scanName, const ScanOptions& scanOptions, LogBuffer* logBuffer) {
    LOG_BUFFER("");
    LOG_BUFFER("Scanning for " << scanName << " bytes.");

    const auto shortCircuit = scanOptions.scanType == ScanType::SINGLE;
    const auto addresses    = ScanMemory(*scanOptions.moduleName, scanOptions.scanBytes, false, shortCircuit, nullptr, logBuffer);
    LOG_BUFFER("Found " << addresses.size() << " match(es).");

    if (addresses.empty()) {
        LOG_BUFFER("AoB scan returned no results, aborting.");
        return false;
    }

    switch (scanOptions.scanType) {
        case ScanType::SINGLE: //
            return DoPatchInternal(addresses[0] + scanOptions.injectOffset, scanOptions, logBuffer);
        case ScanType::MULTIPLE: //
            for (const auto& address : addresses) {
                if (!DoPatchInternal(address + scanOptions.injectOffset, scanOptions, logBuffer)) return false;
            }
            return true;
    }

    return false; // Not reachable.
}