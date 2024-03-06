#pragma once

#include "Proxy.h"

void DoInjection();

BOOL APIENTRY BaseDllMain(HMODULE hModule, DWORD ulReasonForCall, LPVOID lpReserved, IProxy& proxy, int sleepTime = 5000);