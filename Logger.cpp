#include "pch.h"

#include "Logger.h"

#include "Util.h"

bool          logSetup = false;
std::mutex    loggerLock;
std::ofstream out;
std::ostream  outClearFormatHolder(nullptr);

std::string GetCurrentDateTime(const std::string& s) {
    const time_t now = time(nullptr);
    tm           time;
    char         buf[80] = {};

    localtime_s(&time, &now); // NOLINT(cert-err33-c)

    if (s == "now")
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &time); // NOLINT(cert-err33-c)
    else if (s == "date")
        strftime(buf, sizeof(buf), "%Y-%m-%d", &time); // NOLINT(cert-err33-c)

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

void SetupLog(const std::string& path) {
    if (logSetup) return;

    out = std::ofstream(path.c_str(), std::ios_base::out | std::ios_base::trunc);
    outClearFormatHolder.copyfmt(out);
    LOG("Initializing.");
    logSetup = true;
}

void ClearLogFlags() {
    out.copyfmt(outClearFormatHolder);
}

LogBufferImpl::LogBufferImpl() {
    char path[MAX_PATH];
    tmpnam_s(path, MAX_PATH); // NOLINT(cert-err33-c)
    tempFileName = std::string(path);
    tempOut      = new std::ofstream(tempFileName.c_str(), std::ios_base::out | std::ios_base::trunc);

    tempOutClearFormatHolder = new std::ostream(nullptr);
    tempOutClearFormatHolder->copyfmt(*tempOut);

    setup = true;
}

void LogBufferImpl::ClearLogFlags() {
    tempOut->copyfmt(*tempOutClearFormatHolder);
}

void LogBufferImpl::CloseAndCopyToStream(std::ofstream& externalOut) {
    if (setup) Cleanup();

    std::ifstream input;
    input.open(tempFileName, std::ios::in);
    externalOut << input.rdbuf();
    input.close();
}

void LogBufferImpl::Cleanup() {
    if (!setup) return;

    tempOut->close();
    delete tempOut;
    delete tempOutClearFormatHolder;
    setup = false;
}

void LogBufferImpl::DeleteTempFile() {
    if (setup) Cleanup();

    if (DoesFileExist(tempFileName)) {
        std::remove(tempFileName.c_str()); // NOLINT(cert-err33-c)
        fileRemoved = true;
    }
}

LogBufferImpl::~LogBufferImpl() {
    LogBufferImpl::Cleanup();
    LogBufferImpl::DeleteTempFile();
}

LogBufferWrapper::LogBufferWrapper(std::ofstream& externalOut) {
    tempOut = &externalOut;
}