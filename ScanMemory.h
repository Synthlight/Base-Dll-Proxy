#pragma once

std::vector<const BYTE*> ScanMemory(void* addressLow, std::size_t nbytes, const std::vector<BYTE>& bytesToFind);
std::vector<const BYTE*> ScanMemory(std::string moduleName, const std::vector<BYTE>& bytesToFind);
std::vector<const BYTE*> ScanMemory(std::string moduleName, const std::vector<BYTE>&& bytesToFind);

void DoWithProtect(BYTE* address, SIZE_T size, std::function<void()> memActions);