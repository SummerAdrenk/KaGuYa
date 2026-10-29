#pragma once
#include <Runtime/SoundManagerNativeOpen.h>

namespace KaGuYa::Start::Runtime
{
// 业务服务借在正文外；所有槽、params与lower只读写实际50h对象。
class SoundManagerNativeEffects final
{
public:
    explicit SoundManagerNativeEffects(ISoundManagerNativeOpen& open) noexcept : open_(open) {}
    bool RestoreFirstAt42C1B0(SoundManagerNativeBody50h&, const char* name,
        std::uint32_t groupIndex, std::uint8_t restoreAlternate);
    bool RestoreSecondAt42C070(SoundManagerNativeBody50h&, const char* name,
        std::uint32_t groupIndex, std::int32_t fallback);
    // 4D1AA8..4D1B29：调用者已固定manager/名称/组索引/fallback的脚本分支。
    bool OpenRemainingAt4D1AA8(SoundManagerNativeBody50h&, const char* name,
        std::uint32_t groupIndex, std::int32_t fallback);
    bool PlayCueAt4BCD70(SoundManagerNativeBody50h&,const char* name,
        std::uint32_t groupIndex,std::int32_t fallback,std::uint8_t checkType);
private:
    ISoundManagerNativeOpen& open_;
};
}
