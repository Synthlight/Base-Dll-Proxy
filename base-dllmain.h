#pragma once

#include "Proxy.h"

extern bool loaded;

void DoInjection();

BOOL APIENTRY BaseDllMain(HMODULE hModule, DWORD ulReasonForCall, LPVOID lpReserved, IProxy& proxy, int sleepTime = 5000);