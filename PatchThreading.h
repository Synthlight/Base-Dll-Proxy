#pragma once

#include "AllocateMemory.h"
#include "Logger.h"

typedef bool (*PatchFunction)(const std::string& moduleName, PTR_SIZE moduleAddress, AllocateMemory* allocator, LogBuffer* logBuffer);

bool DoPatchFunctionsBlocking(const std::string& moduleName, const PTR_SIZE moduleAddress, AllocateMemory* allocator, const std::vector<PatchFunction>& injectorFunctions);
bool DoPatchFunctionsAsync(const std::string& moduleName, const PTR_SIZE moduleAddress, AllocateMemory* allocator, const std::vector<PatchFunction>& injectorFunctions);