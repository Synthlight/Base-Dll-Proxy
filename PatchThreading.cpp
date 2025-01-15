#include "pch.h"

#include "PatchThreading.h"

void DoBlocking(const std::string& moduleName, const PTR_SIZE moduleAddress, const PatchFunction function, bool* result, std::mutex* resultLock, AllocateMemory* allocator) {
    LogBufferImpl logBuffer;
    const auto    newResult = function(moduleName, moduleAddress, allocator, &logBuffer);
    resultLock->lock();
    logBuffer.CloseAndCopyToStream(out);
    *result = *result && newResult;
    resultLock->unlock();
}

bool DoPatchFunctionsBlocking(const std::string& moduleName, const PTR_SIZE moduleAddress, AllocateMemory* allocator, const std::vector<PatchFunction>& injectorFunctions) {
    bool       result = true;
    std::mutex resultLock;
    for (const auto& func : injectorFunctions) {
        DoBlocking(moduleName, moduleAddress, func, &result, &resultLock, allocator);
    }
    return result;
}

std::thread DoInThread(const std::string& moduleName, const PTR_SIZE moduleAddress, const PatchFunction function, bool* result, std::mutex* resultLock, AllocateMemory* allocator) {
    return std::thread([moduleName, moduleAddress, function, result, resultLock, allocator] {
        DoBlocking(moduleName, moduleAddress, function, result, resultLock, allocator);
    });
}

bool DoPatchFunctionsAsync(const std::string& moduleName, const PTR_SIZE moduleAddress, AllocateMemory* allocator, const std::vector<PatchFunction>& injectorFunctions) {
    bool                     result = true;
    std::mutex               resultLock;
    std::vector<std::thread> threads;
    threads.reserve(injectorFunctions.size());
    for (const auto& func : injectorFunctions) {
        threads.push_back(DoInThread(moduleName, moduleAddress, func, &result, &resultLock, allocator));
    }
    for (auto& thread : threads) {
        thread.join();
    }
    return result;
}