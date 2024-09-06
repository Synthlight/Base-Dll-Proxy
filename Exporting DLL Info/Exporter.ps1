<#
.DESCRIPTION
    A script which wraps DumpBin.exe to fun the exported functions in a DLL and create a list which can be pasted into a DLL.
.PARAMETER DllLocation
	Specify application name of the DLL we are proxying.
.EXAMPLE
	exporter.ps1 -DllName "C:\Windows\System32\TextShaping.dll"
#>

param(
    [Parameter(Mandatory=$true)]
    [string]$DllLocation
)

if(-not $DllLocation.EndsWith(".dll"))
{
    $DllLocation = $DllLocation + ".dll"
    Write-Host "$DllLocation"
}

$DumpPath = "C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.41.34120\bin\Hostx64\x64\dumpbin.exe"

#Strip filepath and extension for later use
$DllName = (Get-Item "$DllLocation").BaseName
$DllName = $DllName+"_original"
$DefPath = "$DllName.def"
$PragmaPath = "$DllName"+"_pragma.txt"

#creating the def file to write to later
if (Test-Path $DefPath) {
    Remove-Item $DefPath
}

if (Test-Path $PragmaPath) {
    Remove-Item $PragmaPath
}

New-Item -ItemType File -Name $DefPath
New-Item -ItemType File -Name $PragmaPath
Add-Content -Path "$DllName.def" -Value "LIBRARY $DllName"
Add-Content -Path "$DllName.def" -Value "EXPORTS"


# Run DumpBin.exe with the exports switch and capture the output, print to console.
& $DumpPath -exports $DllLocation |
    Select-String -Pattern '^\s+\d+\s+\w+\s+[\dA-F]+\s+(.+)$' |
    ForEach-Object {
        $_.Matches[0].Groups[0].Value.Trim()
    } |
    ForEach-Object {
        $columns = $_ -split ' +'
        $ordinal = $columns[0]
        $functionName = $columns[-1]
        Add-Content -Path $PragmaPath -Value "#pragma comment(linker, `"/export:$functionName=$DllName.$functionName,@$ordinal`")"
        Add-Content -Path $DefPath -Value "   $functionName=$DllName.$functionName @$ordinal"
    }