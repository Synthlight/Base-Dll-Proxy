#include "pch.h"

#include "Logger.h"

#include "base-dllmain.h"

std::thread newThread;

bool loaded = false;

BOOL APIENTRY BaseDllMain(HMODULE hModule, const DWORD ulReasonForCall, LPVOID lpReserved, IProxy& proxy) {
    switch (ulReasonForCall) {
        case DLL_PROCESS_ATTACH: if (loaded) break;
            LOG("Attaching proxy.");
            proxy.Attach();
            newThread = std::thread([] {
                Sleep(5000);
                DoInjection();
            });
            loaded = true;

            break;
        case DLL_PROCESS_DETACH: if (!loaded) break;
            LOG("Detaching proxy.");
            proxy.Detach();
            newThread.join();
            out.close();
            loaded = false;
            break;
        default: break;
    }
    return TRUE;
}