#pragma once

#include "Common.h"

/**
* You must call `AllocateGlobalAddresses` before you use it:

    if (!AllocateGlobalAddresses(moduleName, moduleAddress)) {
        MessageBoxW(nullptr, L"Patching failed.", L"Patching Failed", MB_ICONERROR | MB_OK);
        return;
    }
 */
class AllocateMemory final { // NOLINT(cppcoreguidelines-special-member-functions)
    std::mutex lock;

public:
    void*    allocatedNewMemAddress = nullptr;
    PTR_SIZE allocatedSize          = 0;
    PTR_SIZE freeSpaceStartAddress  = 0; // Will be added to as we use the allocated space, but will technically be allocated 'not free' mem.

    AllocateMemory() = default;

    /**
     * Allocates space near the target address. Must be called before `ReserveSpaceInAllocatedNewMem`!
     * NEVER CALL MORE THAN ONCE!
     */
    bool AllocateGlobalAddresses(const std::string& moduleName, PTR_SIZE moduleAddress, PTR_SIZE allocatedNewMemSize = 1024);

    /**
     * 'Gives' chunks of the allocated space to the requester. Essentially just gives the pointer needed and shifts the unused ptr address to
     */
    void* ReserveSpaceInAllocatedNewMem(PTR_SIZE size);

    ~AllocateMemory();
};