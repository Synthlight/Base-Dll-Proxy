#include "pch.h"

#include "ScanMemory.h"

#include "Common.h"
#include "Logger.h"

const std::string wildcard = "??";

template <typename T>
const BYTE* DoSearch(const BYTE* begin, const BYTE* end, const std::vector<T>& toFind);

template <>
const BYTE* DoSearch<BYTE>(const BYTE* begin, const BYTE* end, const std::vector<BYTE>& toFind) {
    return std::search(begin, end, toFind.begin(), toFind.end());
}

template <>
const BYTE* DoSearch<std::string>(const BYTE* begin, const BYTE* end, const std::vector<std::string>& toFind) {
    return std::search(begin, end, toFind.begin(), toFind.end(),
                       [](const BYTE fromMemory, const std::string& fromToFind) {
                           if (fromToFind == wildcard) return true;
                           const auto value = static_cast<BYTE>(std::stoi(fromToFind, nullptr, 16));
                           return value == fromMemory;
                       });
}

template <typename T>
std::vector<const BYTE*> ScanMemoryInternal(const BYTE* startAddress, const std::size_t scanLength, const std::vector<T>& toFind, const bool backwards = false, const bool shortCircuit = false, LogBuffer* logBuffer = nullptr) {
    const auto               toFindAddress = reinterpret_cast<const INT_PTR>(toFind.data());
    std::vector<const BYTE*> addressesFound;

    // all readable pages: adjust this as required
    constexpr DWORD pageMask = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;

    MEMORY_BASIC_INFORMATION mbi{};

    auto       address     = startAddress;
    const auto addressHigh = address + scanLength;

    while (address >= static_cast<BYTE*>(nullptr) && address < addressHigh && VirtualQuery(address, std::addressof(mbi), sizeof(mbi))) {
        // committed memory, readable, won't raise exception guard page
        // if((mbi.State == MEM_COMMIT) && (mbi.Protect | pageMask) && !(mbi.Protect & PAGE_GUARD))
        if (mbi.State == MEM_COMMIT && mbi.Protect & pageMask && !(mbi.Protect & PAGE_GUARD)) {
            auto begin = static_cast<const BYTE*>(mbi.BaseAddress);
            auto end   = begin + mbi.RegionSize;

            // Fixes alignment.
            // If the start address doesn't align to a page, it may theoretically find a match before the start. This fixes that.
            if (!backwards && startAddress > begin) {
                const auto diff = startAddress - begin;
                end -= diff;
                begin = startAddress;
            } else if (backwards && begin > startAddress) {
                const auto diff = begin - startAddress;
                end -= diff;
                begin = startAddress;
            }

            auto found = DoSearch(begin, end, toFind);

            while (found != end) {
                const auto foundPtr = reinterpret_cast<const INT_PTR>(found);
                if (toFindAddress == foundPtr) {
                    LOG_BUFFER("Skipping match that is the address of our search array: " << std::uppercase << std::hex << foundPtr);
                } else {
                    addressesFound.push_back(found);
                    if (shortCircuit) return addressesFound;
                }
                found = DoSearch(found + 1, end, toFind);
            }
        }

        backwards ? address -= mbi.RegionSize : address += mbi.RegionSize;
        mbi = {};
    }

    return addressesFound;
}

template <typename T>
std::vector<const BYTE*> ScanMemoryT(const std::string& moduleName, const std::vector<T>& bytesToFind, const bool fullScan, const bool shortCircuit, const BYTE* startAddress, LogBuffer* logBuffer) {
    if (fullScan) {
        LOG_BUFFER("Doing a full scan from: 0->" << std::uppercase << std::hex << LARGEST_ADDRESS);
        return ScanMemoryInternal(0, LARGEST_ADDRESS, bytesToFind, false, shortCircuit, logBuffer);
    } else {
        auto base = GetModuleHandle(moduleName.c_str());
        if (base == nullptr) {
            LOG_BUFFER("Error getting the module handle.");
            return {};
        }

        MODULEINFO moduleInfo{};
        GetModuleInformation(GetCurrentProcess(), base, std::addressof(moduleInfo), sizeof(moduleInfo));

        if (startAddress != nullptr) {
            const auto moduleAddr  = reinterpret_cast<const UINT64>(base);
            const auto startOffset = reinterpret_cast<const UINT64>(startAddress) - moduleAddr;
            LOG_BUFFER("Scanning from: +" << std::uppercase << std::hex << startOffset << " -> +" << std::uppercase << std::hex << startOffset + moduleInfo.SizeOfImage);
            return ScanMemoryInternal(const_cast<BYTE*>(startAddress), moduleInfo.SizeOfImage - startOffset, bytesToFind, false, shortCircuit, logBuffer);
        } else {
            return ScanMemoryInternal(reinterpret_cast<BYTE*>(base), moduleInfo.SizeOfImage, bytesToFind, false, shortCircuit, logBuffer);
        }
    }
}

std::vector<const BYTE*> ScanMemory(const std::string& moduleName, const std::vector<BYTE>& bytesToFind, const bool fullScan, const bool shortCircuit, const BYTE* startAddress, LogBuffer* logBuffer) {
    return ScanMemoryT(moduleName, bytesToFind, fullScan, shortCircuit, startAddress, logBuffer);
}

std::vector<const BYTE*> ScanMemory(const std::string& moduleName, const std::vector<BYTE>&& bytesToFind, const bool fullScan, const bool shortCircuit, const BYTE* startAddress, LogBuffer* logBuffer) {
    return ScanMemoryT(moduleName, bytesToFind, fullScan, shortCircuit, startAddress, logBuffer);
}

std::vector<const BYTE*> ScanMemory(const std::string& moduleName, const std::vector<std::string>& bytesToFind, const bool fullScan, const bool shortCircuit, const BYTE* startAddress, LogBuffer* logBuffer) {
    return ScanMemoryT(moduleName, bytesToFind, fullScan, shortCircuit, startAddress, logBuffer);
}

std::vector<const BYTE*> ScanMemory(const std::string& moduleName, const std::vector<std::string>&& bytesToFind, const bool fullScan, const bool shortCircuit, const BYTE* startAddress, LogBuffer* logBuffer) {
    return ScanMemoryT(moduleName, bytesToFind, fullScan, shortCircuit, startAddress, logBuffer);
}

std::vector<std::string> StringToVector(const std::string& bytesToFind) {
    std::vector<std::string> byteStringArray;
    std::istringstream       stringStream;
    std::string              s;

    if (bytesToFind.find('*') != std::string::npos) {
        auto str = bytesToFind;
        std::replace(str.begin(), str.end(), '*', '?');
        stringStream = std::istringstream(str);
    } else {
        stringStream = std::istringstream(bytesToFind);
    }

    while (std::getline(stringStream, s, ' ')) {
        byteStringArray.push_back(s);
    }

    return byteStringArray;
}

std::vector<const BYTE*> ScanMemory(const std::string& moduleName, const std::string& bytesToFind, const bool fullScan, const bool shortCircuit, const BYTE* startAddress, LogBuffer* logBuffer) {
    return ScanMemoryT(moduleName, StringToVector(bytesToFind), fullScan, shortCircuit, startAddress, logBuffer);
}

std::vector<const BYTE*> ScanMemory(const std::string& moduleName, const std::string&& bytesToFind, const bool fullScan, const bool shortCircuit, const BYTE* startAddress, LogBuffer* logBuffer) {
    return ScanMemoryT(moduleName, StringToVector(bytesToFind), fullScan, shortCircuit, startAddress, logBuffer);
}

void DoWithProtect(BYTE* address, const SIZE_T size, const std::function<void()>& memActions, LogBuffer* logBuffer) {
    DWORD oldProtect = 0;
    if (VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        memActions();
        VirtualProtect(address, size, oldProtect, &oldProtect);
    } else {
        LOG_BUFFER("VirtualProtect failed.");
    }
}

BOOL VirtualProtect(_In_ const PVOID address, const _In_ SIZE_T size, _In_ const ULONG newProtection) {
    DWORD dummy;
    return VirtualProtect(address, size, newProtection, &dummy);
}