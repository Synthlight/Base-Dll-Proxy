#include "pch.h"

#include "Logger.h"

#include "Proxy.h"

HMODULE versionHMod;

void IProxy::Attach() {
    LOG("Attaching to process.");

    const auto name = GetTargetDllName();
    const auto path = GetTargetDllPath();

    LOG("Loading original dll: " << name << "\n    From: " << path);

    versionHMod = LoadLibrary(path.c_str());
    if (versionHMod == nullptr) {
        LOG("Unable to load base " << name << " dll.");
        return;
    }

    LOG("Orig " << name << " loaded.");
}

void IProxy::Detach() {
    const auto name = GetTargetDllName();
    LOG("Unloading orig " << name << ".");
    FreeLibrary(versionHMod);
}

std::string IProxy::GetDllPathFromSysPath(const std::string& dllName) {
    char sysPath[MAX_PATH];
    GetSystemDirectory(sysPath, MAX_PATH);
    strcat_s(sysPath, "\\");
    strcat_s(sysPath, dllName.c_str());

    return sysPath;
}