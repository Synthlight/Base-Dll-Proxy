#pragma once

// Examples.
// Call something like `SetupLog(GetLogPathAsCurrentDllDotLog());` from DllMain.

extern std::ofstream out;
extern std::ostream  outClearFormatHolder;

std::string GetCurrentDateTime(const std::string& s);
std::string GetFullModulePath();
std::string GetLogPathAsCurrentDllDotLog();
void        SetupLog(const std::string& path);
void        ClearLogFlags();

#define NOW GetCurrentDateTime("now")
//#define PRE_LOG NOW << '\t'
//#define POST_LOG '\n'
//#define LOG(str) ClearLogFlags(); out << PRE_LOG << str << POST_LOG; ClearLogFlags() // NOLINT(bugprone-macro-parentheses)

#define PRE_LOG ClearLogFlags(); out << NOW << '\t'
#define POST_LOG std::endl
#define LOG(str) PRE_LOG << str << POST_LOG