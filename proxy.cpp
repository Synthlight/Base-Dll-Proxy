#include "pch.h"

#include "Logger.h"

#include "proxy.h"

const char* targetDll = "version.dll";
HMODULE     versionHMod;

void Attach() {
    char sysPath[MAX_PATH];
    GetSystemDirectory(sysPath, MAX_PATH);
    strcat_s(sysPath, "\\");
    strcat_s(sysPath, targetDll);

    versionHMod = LoadLibrary(sysPath);
    if (versionHMod == nullptr) {
        LOG("Unable to load base " << targetDll << " dll.");
        return;
    }

    LOG("Orig " << targetDll << " loaded.");
}

void Detach() {
    LOG("Unloading orig " << targetDll << ".");
    FreeLibrary(versionHMod);
}