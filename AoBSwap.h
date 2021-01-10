#pragma once

class AoBSwap {
    std::vector<BYTE> bytesToFind;
    std::vector<BYTE> bytesToReplace;
    std::string targetModule;

public:
    AoBSwap(std::string targetModule, std::vector<BYTE>& bytesToFind, std::vector<BYTE>& bytesToReplace);
    AoBSwap(std::string targetModule, std::vector<BYTE>&& bytesToFind, std::vector<BYTE>&& bytesToReplace): AoBSwap(targetModule, bytesToFind, bytesToReplace) {}
    AoBSwap(std::string targetModule, std::vector<BYTE>& bytesToFind, std::vector<BYTE>&& bytesToReplace): AoBSwap(targetModule, bytesToFind, bytesToReplace) {}
    AoBSwap(std::string targetModule, std::vector<BYTE>&& bytesToFind, std::vector<BYTE>& bytesToReplace): AoBSwap(targetModule, bytesToFind, bytesToReplace) {}

    std::vector<const BYTE*> ScanAndPatch();
};

extern std::vector<BYTE> oneByteNop;
extern std::vector<BYTE> twoByteNop;
extern std::vector<BYTE> threeByteNop;
extern std::vector<BYTE> fourByteNop;
extern std::vector<BYTE> fiveByteNop;
extern std::vector<BYTE> sixByteNop;

std::vector<BYTE> IntToByteArray(long long value, int size);
std::vector<BYTE> CreateCallBytesToAddress(BYTE* targetAddress);