#include "pch.h"

#include "Logger.h"

#include "Util.h"

std::string GetCurrentDateTime(const std::string& s) {
    const time_t now = time(nullptr);
    tm           time;
    char         buf[80] = {};

    localtime_s(&time, &now);

    if (s == "now")
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &time);
    else if (s == "date")
        strftime(buf, sizeof(buf), "%Y-%m-%d", &time);

    return {buf};
};

std::string GetFullModulePath() {
    char    path[MAX_PATH];
    HMODULE hm = nullptr;

    if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          reinterpret_cast<LPCSTR>(&SetupLog), &hm) == 0) {
        const auto ret = GetLastError();
        LOG("GetModuleHandle failed, error = " << ret);
        return "";
    }
    if (GetModuleFileName(hm, path, sizeof(path)) == 0) {
        const auto ret = GetLastError();
        LOG("GetModuleFileName failed, error = " << ret);
        return "";
    }
    return path;
}

std::string GetLogPathAsCurrentDllDotLog() {
    auto        pathStr = GetFullModulePath();
    std::string ending;

    if (EndsWith(pathStr, ".dll")) {
        ending = ".dll";
    } else if (EndsWith(pathStr, ".asi")) {
        ending = ".asi";
    }

    pathStr = pathStr.replace(pathStr.find(ending), 4, ".log");

    //LOG("Got the new log path: " << pathStr);

    return pathStr;
}

std::ofstream SetupLog(const std::string& path) {
    return std::ofstream(path.c_str(), std::ios_base::out | std::ios_base::trunc);
}