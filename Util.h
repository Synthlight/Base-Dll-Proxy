#pragma once

std::string GetExePath();
std::string GetExeFilename();

template <class Container>
bool Contains(const Container& container, const typename Container::value_type& element) {
    return std::find(container.begin(), container.end(), element) != container.end();
}

void        PrintNBytes(const BYTE* address, int length);
std::string BytesToString(const std::vector<BYTE>& bytes);

bool   EndsWith(std::string const& value, std::string const& ending);
UINT32 GetGameVersion();

std::string GetLastErrorAsString();

MODULEINFO  GetModuleInfo(_In_ HANDLE hProcess, _In_ HMODULE hModule);
SYSTEM_INFO GetSysInfo();

inline bool DoesFileExist(const std::string& name) {
    struct stat buffer;
    return stat(name.c_str(), &buffer) == 0;
}

#define PRINT_RELATIVE_ADDRESS(moduleName, moduleAddress, address) std::uppercase << std::hex << reinterpret_cast<const PTR_SIZE>(address) << " (" << (moduleName) << " + " << reinterpret_cast<const PTR_SIZE>(address) - (moduleAddress) << ")"
#define PRINT_RELATIVE_ADDRESS_NEG(moduleName, moduleAddress, address) std::uppercase << std::hex << reinterpret_cast<const PTR_SIZE>(address) << " (" << (moduleName) << " - " << (moduleAddress) - reinterpret_cast<const PTR_SIZE>(address) << ")"