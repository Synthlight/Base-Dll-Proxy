#pragma once

class AoBSwap {
    std::vector<BYTE> bytesToFind;
    std::vector<BYTE> bytesToReplace;
    std::string targetModule;

public:
    AoBSwap(const std::string targetModule, std::vector<BYTE>& bytesToFind, std::vector<BYTE>& bytesToReplace);
    AoBSwap(const std::string targetModule, std::vector<BYTE>&& bytesToFind, std::vector<BYTE>&& bytesToReplace): AoBSwap(targetModule, bytesToFind, bytesToReplace) {}
    AoBSwap(const std::string targetModule, std::vector<BYTE>& bytesToFind, std::vector<BYTE>&& bytesToReplace): AoBSwap(targetModule, bytesToFind, bytesToReplace) {}
    AoBSwap(const std::string targetModule, std::vector<BYTE>&& bytesToFind, std::vector<BYTE>& bytesToReplace): AoBSwap(targetModule, bytesToFind, bytesToReplace) {}

    std::vector<const BYTE*> ScanAndPatch();
};

extern std::vector<BYTE> oneByteNop;
extern std::vector<BYTE> twoByteNop;
extern std::vector<BYTE> threeByteNop;
extern std::vector<BYTE> fourByteNop;
extern std::vector<BYTE> fiveByteNop;
extern std::vector<BYTE> sixByteNop;
extern std::vector<BYTE> sevenByteNop;
extern std::vector<BYTE> eightByteNop;
extern std::vector<BYTE> nineByteNop;

std::vector<BYTE> IntToByteArray(const long long value, const int size);
std::vector<BYTE> CreateCallBytesToAddress(const BYTE* targetAddress);
std::vector<std::string> SpltStringBySpace(const std::string input);
std::vector<BYTE> StringToByteVector(const std::string input);