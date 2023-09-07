#pragma once

// Examples.
//std::ofstream out = std::ofstream("C:\\Temp\\Log.log", std::ios_base::out | std::ios_base::trunc);

extern std::ofstream out;

std::string GetCurrentDateTime(std::string s);
std::string GetLogPathAsCurrentDllDotLog();
std::ofstream SetupLog(std::string path);

#define NOW GetCurrentDateTime("now")
#define PRE_LOG out << NOW << '\t'
#define POST_LOG std::endl
#define Log(str) PRE_LOG << str << POST_LOG