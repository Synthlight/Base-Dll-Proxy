#include "pch.h"

#include "Logger.h"

#include "ScanMemory.h"

std::vector<const BYTE*> ScanMemory(void* addressLow, std::size_t nbytes, const std::vector<BYTE>& bytesToFind) {
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

            auto found = std::search(begin, end, bytesToFind.begin(), bytesToFind.end());

            while (found != end) {
                addressesFound.push_back(found);
                found = std::search(found + 1, end, bytesToFind.begin(), bytesToFind.end());
            }
        }

        address += mbi.RegionSize;
        mbi = {};
    }

    return addressesFound;
}

std::vector<const BYTE*> ScanMemory(std::string moduleName, const std::vector<BYTE>& bytesToFind) {
    auto base = GetModuleHandle(moduleName.c_str());
    if (base == nullptr) return {};

    MODULEINFO minfo {};
    GetModuleInformation(GetCurrentProcess(), base, std::addressof(minfo), sizeof(minfo));

    return ScanMemory(base, minfo.SizeOfImage, bytesToFind);
}

std::vector<const BYTE*> ScanMemory(std::string moduleName, const std::vector<BYTE>&& bytesToFind) {
    return ScanMemory(moduleName, bytesToFind);
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