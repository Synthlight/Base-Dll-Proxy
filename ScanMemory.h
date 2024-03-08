#pragma once

std::vector<const BYTE*> ScanMemory(const std::string& moduleName, const std::vector<BYTE>& bytesToFind, bool fullScan = false, bool shortCircuit = false);
std::vector<const BYTE*> ScanMemory(const std::string& moduleName, const std::vector<BYTE>&& bytesToFind, bool fullScan = false, bool shortCircuit = false);
std::vector<const BYTE*> ScanMemory(const std::string& moduleName, const std::vector<std::string>& bytesToFind, bool fullScan = false, bool shortCircuit = false);
std::vector<const BYTE*> ScanMemory(const std::string& moduleName, const std::vector<std::string>&& bytesToFind, bool fullScan = false, bool shortCircuit = false);
std::vector<const BYTE*> ScanMemory(const std::string& moduleName, const std::string& bytesToFind, bool fullScan = false, bool shortCircuit = false);
std::vector<const BYTE*> ScanMemory(const std::string& moduleName, const std::string&& bytesToFind, bool fullScan = false, bool shortCircuit = false);

void DoWithProtect(BYTE* address, SIZE_T size, const std::function<void()>& memActions);

BOOL VirtualProtect(_In_ PVOID address, _In_ SIZE_T size, _In_ ULONG newProtection);