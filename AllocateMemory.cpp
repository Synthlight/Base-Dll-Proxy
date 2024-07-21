#include "pch.h"

#include "AllocateMemory.h"

#include "Logger.h"
#include "ScanMemory.h"
#include "Util.h"

bool AllocateMemory::AllocateGlobalAddresses(const std::string& moduleName, const PTR_SIZE moduleAddress, const PTR_SIZE allocatedNewMemSize) {
    allocatedSize = allocatedNewMemSize;

    // First allocate a block of mem we can write to for injection.
    freeSpaceStartAddress = moduleAddress - allocatedNewMemSize; // An estimate. `VirtualAlloc` will return a pointer to where it actually starts.
    LOG("Free space begin estimate: (" << moduleName << " + " << freeSpaceStartAddress << ")");

    allocatedNewMemAddress = VirtualAlloc(reinterpret_cast<void*>(freeSpaceStartAddress), allocatedNewMemSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); // NOLINT(performance-no-int-to-ptr)
    if (allocatedNewMemAddress == nullptr) {
        LOG("Error allocating `allocatedNewMemAddress`: \"" << GetLastErrorAsString() << "\", aborting.");
        return false;
    }
    LOG("allocatedNewMemAddress: " << std::uppercase << std::hex << reinterpret_cast<const PTR_SIZE>(allocatedNewMemAddress));
    freeSpaceStartAddress = reinterpret_cast<PTR_SIZE>(allocatedNewMemAddress);

    // Make it writable by all. Without this the game crashes because it can't `mov` into the space we allocate.
    VirtualProtect(allocatedNewMemAddress, allocatedNewMemSize, PAGE_EXECUTE_READWRITE);

    return true;
}

void* AllocateMemory::ReserveSpaceInAllocatedNewMem(const PTR_SIZE size) {
    const auto reservationAddress = reinterpret_cast<void*>(freeSpaceStartAddress); // NOLINT(performance-no-int-to-ptr)
    freeSpaceStartAddress += size;
    return reservationAddress;
}

AllocateMemory::~AllocateMemory() {
    if (allocatedNewMemAddress != nullptr) {
        VirtualFree(allocatedNewMemAddress, allocatedSize, MEM_COMMIT | MEM_RESERVE);
    }
}