#pragma once

class IProxy { // NOLINT(cppcoreguidelines-special-member-functions)
public:
    virtual ~IProxy() = default;

    virtual void Attach();
    virtual void Detach();

    virtual std::string GetTargetDllName() =0;
    virtual std::string GetTargetDllPath() =0;

    static std::string GetDllPathFromSysPath(const std::string& dllName);
};

class DInput8DllProxy final : public IProxy {
protected:
    std::string GetTargetDllName() override { return "dinput8.dll"; }
    std::string GetTargetDllPath() override { return GetDllPathFromSysPath(GetTargetDllName()); }
};

class VersionDllProxy final : public IProxy {
protected:
    std::string GetTargetDllName() override { return "version.dll"; }
    std::string GetTargetDllPath() override { return GetDllPathFromSysPath(GetTargetDllName()); }
};

class WinHttpDllProxy final : public IProxy {
protected:
    std::string GetTargetDllName() override { return "winhttp.dll"; }
    std::string GetTargetDllPath() override { return GetDllPathFromSysPath(GetTargetDllName()); }
};