#include "pch.h"

#include "Util.h"

std::string GetExeFilename() {
    char modulePath[MAX_PATH];
    auto moduleHandle = GetModuleHandle(NULL);
    GetModuleFileName(moduleHandle, modulePath, MAX_PATH);
    auto moduleName = std::string(modulePath);
    return moduleName.substr(moduleName.find_last_of("/\\") + 1);
}