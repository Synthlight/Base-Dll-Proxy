You must declare `std::ofstream out;` which is required for the logger. You'll get unresolved symbols if you don't have it.<br>
You also need to call `out = SetupLog(GetLogPathAsCurrentDllDotLog());` somewhere before using `LOG("")`.

If you want to proxy a DLL you need:
```c++
std::ofstream out; // Required for the logger. You'll get unresolved symbols if you don't have it.
bool          logSetup = false;

BOOL APIENTRY DllMain(const HMODULE hModule, const DWORD ulReasonForCall, const LPVOID lpReserved) {
    if (!logSetup) {
        LOG("Initializing.");
        out      = SetupLog(GetLogPathAsCurrentDllDotLog());
        logSetup = true;
    }

    VersionDllProxy proxy;
    return BaseDllMain(hModule, ulReasonForCall, lpReserved, proxy);
}

void DoInjection() { // Essentially the main entry point.
    // TODO: Stuff.
}
```

And import `proxy-router.h` in your `pch.h` along with including this `pch.h`.

If you want to override a different DLL use the extractor to dump the pragmas and use that instead of `proxy-router.h`.<br>
ALso replace `VersionDllProxy` with your own implementation that targets your new DLL.