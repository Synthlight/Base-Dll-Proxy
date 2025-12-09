#include "pch.h"

#include "AllocateMemory.h"

#include "Logger.h"
#include "Util.h"

bool AllocateMemory::AllocateGlobalAddresses(const std::string& moduleName, const PTR_SIZE moduleAddress, const PTR_SIZE allocatedNewMemSize) {
    lock.lock();

    allocatedSize = allocatedNewMemSize;

    const auto sysInfo = GetSysInfo();
    LOG("sysInfo.dwPageSize: " << std::uppercase << std::hex << sysInfo.dwPageSize);
    LOG("sysInfo.dwAllocationGranularity: " << std::uppercase << std::hex << sysInfo.dwAllocationGranularity);

    // First allocate a block of mem we can write to for injection.
    freeSpaceStartAddress = moduleAddress - sysInfo.dwAllocationGranularity; // An estimate. `VirtualAlloc` will return a pointer to where it actually starts.
    LOG("Free space begin estimate: " << std::uppercase << std::hex << freeSpaceStartAddress << " (" << moduleName << " - " << sysInfo.dwAllocationGranularity << ")");

    int32_t tryCount = 0;

    while (true) {
        allocatedNewMemAddress = VirtualAlloc(reinterpret_cast<void*>(freeSpaceStartAddress), allocatedNewMemSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); // NOLINT(performance-no-int-to-ptr)
        if (allocatedNewMemAddress != nullptr) break;

        LOG("Error allocating mem at " << std::uppercase << std::hex << freeSpaceStartAddress << ", trying again " << sysInfo.dwAllocationGranularity << " bytes earlier.");
        freeSpaceStartAddress -= sysInfo.dwAllocationGranularity;

        tryCount++;
        if (tryCount > 500) {
            LOG("Error allocating mem: \"" << GetLastErrorAsString() << "\", aborting.");
            lock.unlock();
            return false;
        }
    }

    LOG("allocatedNewMemAddress: " << PRINT_RELATIVE_ADDRESS_NEG(moduleName, moduleAddress, allocatedNewMemAddress));
    freeSpaceStartAddress = reinterpret_cast<PTR_SIZE>(allocatedNewMemAddress);

    lock.unlock();

    return true;
}

void* AllocateMemory::ReserveSpaceInAllocatedNewMem(const PTR_SIZE size) {
    lock.lock();

    const auto reservationAddress = reinterpret_cast<void*>(freeSpaceStartAddress); // NOLINT(performance-no-int-to-ptr)
    freeSpaceStartAddress         += size;

    lock.unlock();

    return reservationAddress;
}

AllocateMemory::~AllocateMemory() {
    lock.lock();

    if (allocatedNewMemAddress != nullptr) {
        VirtualFree(allocatedNewMemAddress, allocatedSize, MEM_COMMIT | MEM_RESERVE);
    }

    lock.unlock();
}