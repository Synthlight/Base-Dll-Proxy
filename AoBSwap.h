#pragma once

#include "AllocateMemory.h"
#include "Common.h"

class AoBSwap {
    std::vector<BYTE> bytesToFind_;
    std::vector<BYTE> bytesToReplace_;
    std::string       targetModule_;

public:
    AoBSwap(const std::string& targetModule, const std::vector<BYTE>& bytesToFind, const std::vector<BYTE>& bytesToReplace);
    AoBSwap(const std::string& targetModule, std::vector<BYTE>&& bytesToFind, std::vector<BYTE>&& bytesToReplace): AoBSwap(targetModule, bytesToFind, bytesToReplace) {}
    AoBSwap(const std::string& targetModule, const std::vector<BYTE>& bytesToFind, std::vector<BYTE>&& bytesToReplace): AoBSwap(targetModule, bytesToFind, bytesToReplace) {}
    AoBSwap(const std::string& targetModule, std::vector<BYTE>&& bytesToFind, const std::vector<BYTE>& bytesToReplace): AoBSwap(targetModule, bytesToFind, bytesToReplace) {}

    std::vector<const BYTE*> ScanAndPatch() const;
};

extern const std::vector<BYTE> RETURN_OP;
extern const std::vector<BYTE> ONE_BYTE_NOP;
extern const std::vector<BYTE> TWO_BYTE_NOP;
extern const std::vector<BYTE> THREE_BYTE_NOP;
extern const std::vector<BYTE> FOUR_BYTE_NOP;
extern const std::vector<BYTE> FIVE_BYTE_NOP;
extern const std::vector<BYTE> SIX_BYTE_NOP;
extern const std::vector<BYTE> SEVEN_BYTE_NOP;
extern const std::vector<BYTE> EIGHT_BYTE_NOP;
extern const std::vector<BYTE> NINE_BYTE_NOP;

std::vector<BYTE>        IntToByteArray(UINT64 value);
std::vector<BYTE>        IntToByteArray(UINT32 value);
std::vector<BYTE>        CreateCallBytesToAddress(const BYTE* targetAddress, const BYTE* fromAddress);
std::vector<std::string> SplitStringBySpace(const std::string& input);
std::vector<BYTE>        StringToByteVector(const std::string& input);

bool DoSimplePatch(const std::string& moduleName, PTR_SIZE moduleAddress, const std::string& scanName, const std::string& scanBytes, const std::vector<BYTE>& newMemBytes);
bool DoInjectPatch(const std::string& moduleName, PTR_SIZE moduleAddress, const std::string& scanName, const std::string& scanBytes, const PTR_SIZE originalOpSize, AllocateMemory* allocator, const std::vector<BYTE>& newMemBytes);