#include "pch.h"

#include "Util.h"

#include "Logger.h"

std::string GetExeFilename() {
    char       modulePath[MAX_PATH];
    const auto moduleHandle = GetModuleHandle(nullptr);
    GetModuleFileName(moduleHandle, modulePath, MAX_PATH);
    const auto moduleName = std::string(modulePath);
    return moduleName.substr(moduleName.find_last_of("/\\") + 1);
}

void PrintNBytes(const BYTE* address, const int length) {
    out << NOW << '\t' << "Bytes: ";
    for (auto i = 0; i < length; i++) {
        if (i > 0) out << ",";
        out << std::uppercase << std::hex << static_cast<const UINT32>(*(address + i));
    }
    out << std::endl;
}