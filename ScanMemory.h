#pragma once

#include "Logger.h"

std::vector<const BYTE*> ScanMemory(const std::string& moduleName, const std::vector<BYTE>& bytesToFind, bool fullScan = false, bool shortCircuit = false, const BYTE* startAddress = nullptr, LogBuffer* logBuffer = nullptr);
std::vector<const BYTE*> ScanMemory(const std::string& moduleName, const std::vector<BYTE>&& bytesToFind, bool fullScan = false, bool shortCircuit = false, const BYTE* startAddress = nullptr, LogBuffer* logBuffer = nullptr);
std::vector<const BYTE*> ScanMemory(const std::string& moduleName, const std::vector<std::string>& bytesToFind, bool fullScan = false, bool shortCircuit = false, const BYTE* startAddress = nullptr, LogBuffer* logBuffer = nullptr);
std::vector<const BYTE*> ScanMemory(const std::string& moduleName, const std::vector<std::string>&& bytesToFind, bool fullScan = false, bool shortCircuit = false, const BYTE* startAddress = nullptr, LogBuffer* logBuffer = nullptr);
std::vector<const BYTE*> ScanMemory(const std::string& moduleName, const std::string& bytesToFind, bool fullScan = false, bool shortCircuit = false, const BYTE* startAddress = nullptr, LogBuffer* logBuffer = nullptr);
std::vector<const BYTE*> ScanMemory(const std::string& moduleName, const std::string&& bytesToFind, bool fullScan = false, bool shortCircuit = false, const BYTE* startAddress = nullptr, LogBuffer* logBuffer = nullptr);

void DoWithProtect(BYTE* address, SIZE_T size, const std::function<void()>& memActions, LogBuffer* logBuffer = nullptr);

BOOL VirtualProtect(_In_ PVOID address, _In_ SIZE_T size, _In_ ULONG newProtection);