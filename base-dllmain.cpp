#include "pch.h"

#include "Logger.h"
#include "proxy.h"

#include "base-dllmain.h"

std::thread newThread;

BOOL APIENTRY BaseDllMain(HMODULE hModule, const DWORD ulReasonForCall, LPVOID lpReserved) {
    switch (ulReasonForCall) {
    case DLL_PROCESS_ATTACH:
        //case DLL_THREAD_ATTACH:
        //DisableThreadLibraryCalls(hModule);
        Attach();

        newThread = std::thread([] {
            Sleep(5000);
            DoInjection();
        });

        break;
    //case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH: Detach();
        newThread.join();
        out.close();
        break;
    default: ;
    }
    return TRUE;
}