Run `Exporter.ps1 {path to DLL}`.

You may need to edit it and update the path to `DumpBin.exe`.

This will dump:
- `{DLL name}_original.def`
- `{DLL name}_original_pragma.txt`
In the working dir.<br>
`{DLL name}_original_pragma.txt` contains all the pragma definitions you should need.


Pragma definitions will have stuff like:
`#pragma comment(linker, "/export:WinHttpReadData=winhttp_original.WinHttpReadData,@48")`
And you need to change:
`winhttp_original`
into:
`C:\\Windows\\System32\\winhttp`

Else the proxy won't know how to load the original and it'll just crash.

Source: https://github.com/freefallerr/DllExporter