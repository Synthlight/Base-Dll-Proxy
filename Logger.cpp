#include "pch.h"

#include "Logger.h"

std::ofstream out = std::ofstream(outputFile.c_str(), std::ios_base::out | std::ios_base::trunc);

std::string GetCurrentDateTime(std::string s) {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];

    localtime_s(&tstruct, &now);

    if (s == "now")
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    else if (s == "date")
        strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);

    return std::string(buf);
};