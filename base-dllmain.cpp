#include "pch.h"

#include "Logger.h"

#include "base-dllmain.h"

std::thread newThread;

bool loaded = false;

BOOL APIENTRY BaseDllMain(HMODULE hModule, const DWORD ulReasonForCall, LPVOID lpReserved, IProxy& proxy, int sleepTime) {
    switch (ulReasonForCall) {
        case DLL_PROCESS_ATTACH: // Makes auto format behave.
            if (loaded) break;
            LOG("Attaching proxy.");
            proxy.Attach();
            newThread = std::thread([sleepTime] {
                if (sleepTime > 0) {
                    Sleep(sleepTime);
                }
                DoInjection();
            });
            loaded = true;

            break;
        case DLL_PROCESS_DETACH: // Makes auto format behave.
            if (!loaded) break;
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