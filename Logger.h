#pragma once

extern const std::string outputFile;
extern std::ofstream out;

extern std::string GetCurrentDateTime(std::string s);

#define NOW GetCurrentDateTime("now")
#define PRE_LOG out << NOW << '\t'
#define POST_LOG std::endl
#define Log(str) PRE_LOG << str << POST_LOG