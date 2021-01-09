#include "pch.h"

#include "Logger.h"

#include "version.h"

const char* targetDll = "version.dll";
HMODULE varsionHMod;

void Attach() {
    char syspath[MAX_PATH];
    GetSystemDirectory(syspath, MAX_PATH);
    strcat_s(syspath, "\\");
    strcat_s(syspath, targetDll);

    varsionHMod = LoadLibrary(syspath);
    if (varsionHMod == nullptr) {
        Log("Unable to load base " << targetDll << " dll.");
        return;
    }

    Log("Orig " << targetDll << " loaded.");
}

void Detach() {
    Log("Unloading orig " << targetDll << ".");
    FreeLibrary(varsionHMod);
}