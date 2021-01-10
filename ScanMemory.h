#pragma once

std::vector<const BYTE*> ScanMemory(std::string moduleName, const std::vector<BYTE>& bytesToFind);
std::vector<const BYTE*> ScanMemory(std::string moduleName, const std::vector<BYTE>&& bytesToFind);
std::vector<const BYTE*> ScanMemory(std::string moduleName, const std::vector<std::string>& bytesToFind);
std::vector<const BYTE*> ScanMemory(std::string moduleName, const std::vector<std::string>&& bytesToFind);
std::vector<const BYTE*> ScanMemory(std::string moduleName, const std::string& bytesToFind);
std::vector<const BYTE*> ScanMemory(std::string moduleName, const std::string&& bytesToFind);

void DoWithProtect(BYTE* address, SIZE_T size, std::function<void()> memActions);