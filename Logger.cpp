#include "pch.h"

#include "Logger.h"

std::string GetCurrentDateTime(std::string s) {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80] = {};

    localtime_s(&tstruct, &now);

    if (s == "now")
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    else if (s == "date")
        strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);

    return std::string(buf);
};

std::string GetLogPathAsCurrentDllDotLog() {
    char path[MAX_PATH];
    HMODULE hm = NULL;

    if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          (LPCSTR) &SetupLog, &hm) == 0) {
        int ret = GetLastError();
        fprintf(stderr, "GetModuleHandle failed, error = %d\n", ret);
        // Return or however you want to handle an error.
    }
    if (GetModuleFileName(hm, path, sizeof(path)) == 0) {
        int ret = GetLastError();
        fprintf(stderr, "GetModuleFileName failed, error = %d\n", ret);
        // Return or however you want to handle an error.
    }

    auto pathStr = std::string(path);
    //pathStr = replace(pathStr, "Reactor-Count-Mod.dll", "Log.log");
    pathStr = pathStr.replace(pathStr.find(".dll"), sizeof(".dll") - 1, ".log");
    //pathStr = pathStr + std::string(".log");

    //Log("Got the new log path: " << pathStr);

    return pathStr;
}

std::ofstream SetupLog(std::string path) {
    return std::ofstream(path.c_str(), std::ios_base::out | std::ios_base::trunc);
}