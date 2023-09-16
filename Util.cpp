#include "pch.h"

#include "Util.h"

#include "Logger.h"

std::string GetExePath() {
    char       modulePath[MAX_PATH];
    const auto moduleHandle = GetModuleHandle(nullptr);
    GetModuleFileName(moduleHandle, modulePath, MAX_PATH);
    return modulePath;
}

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

bool EndsWith(std::string const& value, std::string const& ending) {
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

#define MAKE_EXE_VERSION_EX(major, minor, build, sub)	((((major) & 0xFF) << 24) | (((minor) & 0xFF) << 16) | (((build) & 0xFFF) << 4) | ((sub) & 0xF))

UINT32 GetGameVersion() {
    const auto  path      = GetExePath();
    DWORD       verHandle = NULL;
    UINT        size      = 0;
    LPBYTE      lpBuffer  = nullptr;
    const DWORD verSize   = GetFileVersionInfoSize(path.c_str(), &verHandle);

    if (verSize != NULL) {
        const auto verData = new char[verSize];

        if (GetFileVersionInfo(path.c_str(), verHandle, verSize, verData)) {
            if (VerQueryValue(verData, "\\", reinterpret_cast<void**>(&lpBuffer), &size)) {
                if (size) {
                    const auto verInfo = reinterpret_cast<VS_FIXEDFILEINFO*>(lpBuffer);
                    if (verInfo->dwSignature == 0xfeef04bd) {
                        // Doesn't matter if you are on 32 bit or 64 bit,
                        // DWORD is always 32 bits, so first two revision numbers
                        // come from dwFileVersionMS, last two come from dwFileVersionLS
                        //LOG("File Version: " <<
                        //    ((verInfo->dwFileVersionMS >> 16) & 0xffff) <<
                        //    ((verInfo->dwFileVersionMS >> 0) & 0xffff) <<
                        //    ((verInfo->dwFileVersionLS >> 16) & 0xffff) <<
                        //    ((verInfo->dwFileVersionLS >> 0) & 0xffff)
                        //);

                        const auto version = MAKE_EXE_VERSION_EX((verInfo->dwFileVersionMS >> 16) & 0xffff,
                                                                 (verInfo->dwFileVersionMS >> 0) & 0xffff,
                                                                 (verInfo->dwFileVersionLS >> 16) & 0xffff,
                                                                 (verInfo->dwFileVersionLS >> 0) & 0xffff);
                        delete[] verData;
                        return version;
                    }
                }
            }
        }
        delete[] verData;
    }

    return MAKE_EXE_VERSION_EX(1, 1, 1, 1);
}