#pragma once

#include "Common.h"

/**
* You must call `AllocateGlobalAddresses` before you use it:
    AllocateMemory allocator;
    if (!allocator.AllocateGlobalAddresses(moduleName, moduleAddress)) {
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
     * 'Gives' chunks of the allocated space to the requester. Essentially just returns a pointer to the allocated area and shifts the unused ptr address by the requested size.
     * It is assumed that the requester will NOT exceed the requested bounds/size. If you do, you will overwrite whatever requests space next. There's no protection against this.
     */
    void* ReserveSpaceInAllocatedNewMem(PTR_SIZE size);

    ~AllocateMemory();
};