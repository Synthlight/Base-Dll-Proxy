#include "pch.h"

#include "Logger.h"

#include "ScanMemory.h"

std::string wildcard = "??";

template<typename T>
const BYTE* DoSearch(const BYTE* begin, const BYTE* end, const std::vector<T>& toFind);

template<>
const BYTE* DoSearch<BYTE>(const BYTE* begin, const BYTE* end, const std::vector<BYTE>& toFind) {
    return std::search(begin, end, toFind.begin(), toFind.end());
}

template<>
const BYTE* DoSearch<std::string>(const BYTE* begin, const BYTE* end, const std::vector<std::string>& toFind) {
    return std::search(begin, end, toFind.begin(), toFind.end(), [](BYTE fromMemory, std::string fromToFind) {
        if (fromToFind == wildcard) return true;
        auto value = (BYTE) std::stoi(fromToFind, nullptr, 16);
        return value == fromMemory;
    });
}

template<typename T>
std::vector<const BYTE*> ScanMemory(void* addressLow, std::size_t nbytes, const std::vector<T>& toFind) {
    std::vector<const BYTE*> addressesFound;

    // all readable pages: adjust this as required
    const DWORD pmask = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;

    MEMORY_BASIC_INFORMATION mbi{};

    auto address = static_cast<BYTE*>(addressLow);
    auto addressHigh = address + nbytes;

    while (address < addressHigh && VirtualQuery(address, std::addressof(mbi), sizeof(mbi))) {
        // committed memory, readable, wont raise exception guard page
        // if((mbi.State == MEM_COMMIT) && (mbi.Protect | pmask) && !(mbi.Protect & PAGE_GUARD))
        if ((mbi.State == MEM_COMMIT) && (mbi.Protect & pmask) && !(mbi.Protect & PAGE_GUARD)) {
            auto begin = static_cast<const BYTE*>(mbi.BaseAddress);
            auto end = begin + mbi.RegionSize;
            auto found = DoSearch(begin, end, toFind);

            while (found != end) {
                addressesFound.push_back(found);
                found = DoSearch(found + 1, end, toFind);
            }
        }

        address += mbi.RegionSize;
        mbi = {};
    }

    return addressesFound;
}

template<typename T>
std::vector<const BYTE*> ScanMemoryT(std::string moduleName, const std::vector<T>& bytesToFind) {
    auto base = GetModuleHandle(moduleName.c_str());
    if (base == nullptr) return {};

    MODULEINFO minfo {};
    GetModuleInformation(GetCurrentProcess(), base, std::addressof(minfo), sizeof(minfo));

    return ScanMemory(base, minfo.SizeOfImage, bytesToFind);
}

std::vector<const BYTE*> ScanMemory(std::string moduleName, const std::vector<BYTE>& bytesToFind) {
    return ScanMemoryT(moduleName, bytesToFind);
}

std::vector<const BYTE*> ScanMemory(std::string moduleName, const std::vector<BYTE>&& bytesToFind) {
    return ScanMemoryT(moduleName, bytesToFind);
}

std::vector<const BYTE*> ScanMemory(std::string moduleName, const std::vector<std::string>& bytesToFind) {
    return ScanMemoryT(moduleName, bytesToFind);
}

std::vector<const BYTE*> ScanMemory(std::string moduleName, const std::vector<std::string>&& bytesToFind) {
    return ScanMemoryT(moduleName, bytesToFind);
}

const std::vector<std::string> stringToVector(const std::string& bytesToFind) {
    std::vector<std::string> byteStringArray;
    auto stringStream = std::istringstream(bytesToFind);
    std::string s;

    while (std::getline(stringStream, s, ' ')) {
        byteStringArray.push_back(s);
    }

    return byteStringArray;
}

std::vector<const BYTE*> ScanMemory(std::string moduleName, const std::string& bytesToFind) {
    return ScanMemoryT(moduleName, stringToVector(bytesToFind));
}

std::vector<const BYTE*> ScanMemory(std::string moduleName, const std::string&& bytesToFind) {
    return ScanMemoryT(moduleName, stringToVector(bytesToFind));
}

void DoWithProtect(BYTE* address, SIZE_T size, std::function<void()> memActions) {
    DWORD oldProtect = 0;
    if (VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        memActions();
        VirtualProtect(address, size, oldProtect, &oldProtect);
    }
    else {
        Log("VirtualProtect failed.");
    }
}