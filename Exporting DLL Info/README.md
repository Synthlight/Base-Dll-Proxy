Run `Exporter.ps1 {path to DLL}`.

You may need to edit it and update the path to `DumpBin.exe`.

This will dump:
- `{DLL name}_original.def`
- `{DLL name}_original_pragma.txt`
In the working dir.<br>
`{DLL name}_original_pragma.txt` contains all the pragma definitions you should need.


Source: https://github.com/freefallerr/DllExporter