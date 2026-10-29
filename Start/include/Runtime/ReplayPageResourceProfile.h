#pragma once
#include <Runtime/ModeSelectButtonNative.h>
#include <cstdint>
#include <span>

namespace KaGuYa::Start::Runtime
{
struct ReplayPageBackground final
{
    const char* normal;
    const char* unavailable;
    std::int32_t availabilityIndex; // -1表示不查询角色显示可用性。
};

struct ReplayPageResourceProfile final
{
    std::span<const ReplayPageBackground> backgrounds;
    std::span<const char* const, 3> pageMarks;
    std::span<const char* const, 3> stamps; // 依次为按下、焦点、未解锁印章。
    std::span<const char* const, 3> thumbnailKeys; // 合成surface缓存键，不是缩略图文件名。
    std::span<const char* const, 10> percentDigits;
    std::span<const POINT, 9> positions;
    std::span<const POINT, 3> percentPositions;
    POINT buttonOffset;
    POINT imageOffset;
    std::span<const std::int32_t, 108> neighbors; // 按[条目][上、下、左、右]排列。
    std::span<const CgUiButtonRecordNative28h, 2> pageButtons;
    std::int32_t itemsPerPage;
    std::int32_t maximumItems;
};

// provider及引用的数组、CP932字串均为静态存储，生命周期覆盖消费者。
// 编译游戏缺少绑定时明确抛出异常，不回退到18号数据。
const ReplayPageResourceProfile& CurrentReplayPageProfile();
}
