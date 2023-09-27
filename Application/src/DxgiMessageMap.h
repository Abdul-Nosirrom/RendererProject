#pragma once
#include <string>
#include <unordered_map>
#include "RomanceWin.h"

class DxgiMessageMap
{
public:
    DxgiMessageMap();
    std::string operator()(HRESULT hr, bool& bFoundCode) const;
private:
    std::unordered_map<DWORD, std::string> map;
};
