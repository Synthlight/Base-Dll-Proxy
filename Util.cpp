#include "pch.h"

#include "Util.h"

#include "Common.h"
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
        out << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<const UINT32>(*(address + i));
    }
    out << '\n';
}

std::string BytesToString(const std::vector<BYTE>& bytes) {
    auto outStream = std::ostringstream();

    for (PTR_SIZE i = 0; i < bytes.size(); i++) {
        if (i > 0) outStream << ",";
        outStream << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<const UINT32>(bytes[i]);
    }

    return outStream.str();
}

bool EndsWith(std::string const& value, std::string const& ending) {
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

#define MAKE_EXE_VERSION_EX(major, minor, build, sub)	((((major) & 0xFF) << 24) | (((minor) & 0xFF) << 16) | (((build) & 0xFFF) << 4) | ((sub) & 0xF))

UINT32 GetGameVersion() {
    const auto  path      = GetExePath();
    DWORD       verHandle = 0;
    UINT        size      = 0;
    LPBYTE      lpBuffer  = nullptr;
    const DWORD verSize   = GetFileVersionInfoSize(path.c_str(), &verHandle);

    if (verSize != NULL) {
        const auto verData = new char[verSize];

        if (GetFileVersionInfo(path.c_str(), 0, verSize, verData)) {
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

// https://stackoverflow.com/a/17387176
std::string GetLastErrorAsString() {
    // Get the error message ID, if any.
    const DWORD errorMessageId = ::GetLastError();
    if (errorMessageId == 0) {
        return {}; // No error message has been recorded
    }

    LPSTR messageBuffer = nullptr;

    // Ask Win32 to give us the string version of that message ID.
    // The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    const size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                       nullptr, errorMessageId, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&messageBuffer), 0, nullptr);

    // Copy the error message into a std::string.
    std::string message(messageBuffer, size);

    // Free the Win32's string's buffer.
    LocalFree(messageBuffer);

    return message;
}

MODULEINFO GetModuleInfo(_In_ const HANDLE hProcess, _In_ const HMODULE hModule) {
    MODULEINFO moduleInfo;
    GetModuleInformation(hProcess, hModule, &moduleInfo, sizeof(MODULEINFO));
    return moduleInfo;
}

SYSTEM_INFO GetSysInfo() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo;
}