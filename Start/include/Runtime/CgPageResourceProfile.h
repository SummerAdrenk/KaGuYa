#pragma once
#include <Runtime/ModeSelectButtonNative.h>
#include <array>
#include <span>

namespace KaGuYa::Start::Runtime
{
struct CgPageBackground final
{
    const char* normal;
    const char* unavailable;
    // -1不查询保存的角色显示状态；只有Mamatomo前四分类有原始查询。
    int characterDisplayIndex;
};
struct CgPageResourceProfile final
{
    std::span<const CgPageBackground> categories;
    std::array<const char*,3> pageMarks;
    std::array<const char*,4> stamps; // 点击、焦点、未解锁、计数斜线。
    std::array<const char*,10> digits, percentDigits;
    std::array<const char*,3> generatedNames;
    std::array<CgUiButtonRecordNative28h,2> pageButtons;
    std::array<POINT,9> positions, slashes;
    std::array<std::array<int,2>,9> oneDigitX, counterY;
    std::array<std::array<int,4>,9> twoDigitX;
    std::array<std::array<int,6>,9> threeDigitX;
    std::array<POINT,3> percentPositions;
    std::array<int,108> neighbors;
    POINT imageInset, buttonOffset;
    // LoveHolic原479B90直接跳公共键处理，不处理18/19翻页键；鼠标分页不变。
    bool pageKeysEnabled;
};
const CgPageResourceProfile& CurrentCgPageResourceProfile();
}
