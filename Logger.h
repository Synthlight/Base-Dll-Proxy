#pragma once

// Examples.
// Call something like `SetupLog(GetLogPathAsCurrentDllDotLog());` from DllMain.

extern std::mutex    loggerLock;
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

#define LOG_PRE loggerLock.lock(); ClearLogFlags(); out << NOW << '\t'
#define LOG_POST std::endl; loggerLock.unlock()
#define LOG(str) LOG_PRE << str << LOG_POST

class LogBuffer { // NOLINT(cppcoreguidelines-special-member-functions)
public:
    std::ofstream* tempOut = nullptr;

    LogBuffer() = default;
    virtual void ClearLogFlags() {}
    virtual void CloseAndCopyToStream(std::ofstream& externalOut) {}
    virtual void Cleanup() {}
    virtual void DeleteTempFile() {}
    virtual      ~LogBuffer() = default;
};

class LogBufferImpl final : public LogBuffer { // NOLINT(cppcoreguidelines-special-member-functions)
    bool          setup;
    bool          fileRemoved = false;
    std::string   tempFileName;
    std::ostream* tempOutClearFormatHolder;

public:
    LogBufferImpl();
    void ClearLogFlags() override;
    void CloseAndCopyToStream(std::ofstream& externalOut) override;
    void Cleanup() override;
    void DeleteTempFile() override;
    ~LogBufferImpl() override;
};

class LogBufferWrapper final : public LogBuffer {
public:
    explicit LogBufferWrapper(std::ofstream& externalOut);
    void     ClearLogFlags() override {}
    void     CloseAndCopyToStream(std::ofstream& externalOut) override {}
    void     Cleanup() override {}
    void     DeleteTempFile() override {}
};

#define LOG_BUFFER_PRE logBuffer->ClearLogFlags(); *logBuffer->tempOut << NOW << '\t'
#define LOG_BUFFER_POST std::endl;
#define LOG_BUFFER(str) if (logBuffer == nullptr) { LOG(str); } else { LOG_BUFFER_PRE << str << LOG_BUFFER_POST; } struct dummy // Useless statement at the end exists only so we can ';' after the macro.