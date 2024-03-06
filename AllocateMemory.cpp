#include "pch.h"

#include "Logger.h"
#include "ScanMemory.h"
#include "Util.h"

#include "AllocateMemory.h"

bool AllocateMemory::AllocateGlobalAddresses(const std::string& moduleName, const UINT64 moduleAddress, const UINT64 allocatedNewMemSize) {
    allocatedSize = allocatedNewMemSize;

    // First allocate a block of mem we can write to for injection.
    freeSpaceStartAddress = moduleAddress - allocatedNewMemSize; // An estimate. `VirtualAlloc` will return a pointer to where it actually starts.
    LOG("Free space begin estimate: (" << moduleName << " + " << freeSpaceStartAddress << ")");

    allocatedNewMemAddress = VirtualAlloc(reinterpret_cast<void*>(freeSpaceStartAddress), allocatedNewMemSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); // NOLINT(performance-no-int-to-ptr)
    if (allocatedNewMemAddress == nullptr) {
        LOG("Error allocating `allocatedNewMemAddress`: \"" << GetLastErrorAsString() << "\", aborting.");
        return false;
    }
    LOG("allocatedNewMemAddress: " << std::uppercase << std::hex << reinterpret_cast<const UINT64>(allocatedNewMemAddress));
    freeSpaceStartAddress = reinterpret_cast<UINT64>(allocatedNewMemAddress);

    // Make it writable by all. Without this the game crashes because it can't `mov` into the space we allocate.
    VirtualProtect(allocatedNewMemAddress, allocatedNewMemSize, PAGE_EXECUTE_READWRITE);

    return true;
}

void* AllocateMemory::ReserveSpaceInAllocatedNewMem(const UINT64 size) {
    const auto reservationAddress = reinterpret_cast<void*>(freeSpaceStartAddress); // NOLINT(performance-no-int-to-ptr)
    freeSpaceStartAddress += size;
    return reservationAddress;
}

AllocateMemory::~AllocateMemory() {
    if (allocatedNewMemAddress != nullptr) {
        VirtualFree(allocatedNewMemAddress, allocatedSize, MEM_COMMIT | MEM_RESERVE);
    }
}