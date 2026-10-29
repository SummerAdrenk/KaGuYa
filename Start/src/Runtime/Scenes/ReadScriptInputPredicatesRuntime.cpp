#include <Runtime/ReadScriptInputPredicatesRuntime.h>

#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/ScrVmExecutableProfile.h>
#include <Runtime/ScrVmValueTable.h>
#include <Runtime/UnSaveDataRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
ReadScriptInputPredicatesRuntime::ReadScriptInputPredicatesRuntime(const ClickInfoArchive& click,
    const UnSaveDataRuntime& unsave, const ScrVmValueTable& values,
    const ScrVmExecutableProfile& profile, Platform::MouseView mouse,
    Platform::KeyboardView keyboard) noexcept
    : click_(click), unsave_(unsave), values_(values), profile_(profile), mouse_(mouse), input_(mouse, keyboard) {}

bool ReadScriptInputPredicatesRuntime::MouseSelectorAt4B0630(std::uint32_t selector) const noexcept
{
    // 原参数是完整 DWORD 枚举，不是虚拟键号，也不检查边沿/倒计时。
    switch (selector)
    {
    case 1: return mouse_.DownLatched(1) != 0;
    case 2: return mouse_.DownLatched(0) != 0;
    case 3: return mouse_.DownLatched(2) != 0;
    default: return false;
    }
}

bool ReadScriptInputPredicatesRuntime::ConfiguredMouseAtClickOffset(std::uint32_t offset) const
{
    // 4B2954 等先读 Environment slot 5 的字段，再把值传4B0630。
    const std::array<std::uint8_t, 8>* bytes = nullptr;
    switch (offset)
    {
    case 0x04: bytes = &click_.bytesAt04; break;
    case 0x0C: bytes = &click_.bytesAt0C; break;
    case 0x14: bytes = &click_.bytesAt14; break;
    case 0x1C: bytes = &click_.bytesAt1C; break;
    case 0x24: bytes = &click_.bytesAt24; break;
    case 0x2C: bytes = &click_.bytesAt2C; break;
    case 0x34: bytes = &click_.bytesAt34; break;
    default: throw std::invalid_argument("CClickInfo mouse selector field is not an audited group offset");
    }
    const auto& b = *bytes;
    return MouseSelectorAt4B0630(std::uint32_t{b[0]} | (std::uint32_t{b[1]} << 8) |
        (std::uint32_t{b[2]} << 16) | (std::uint32_t{b[3]} << 24));
}

bool ReadScriptInputPredicatesRuntime::HeldSkipKey() const
{
    return input_.ReadHeldAtVslot00(6);
}

std::int32_t ReadScriptInputPredicatesRuntime::SkipAt4B0760(bool includeHeldKey) const
{
    if ((unsave_.DwordAt0C() & 0xFFu) == 0) return 0;
    if (includeHeldKey && HeldSkipKey()) return 2;
    if ((unsave_.FlagsAt14() & 3u) == 3u) return 2;
    return (unsave_.FlagsAt14() & 1u) != 0 ? 1 : 0;
}

std::int32_t ReadScriptInputPredicatesRuntime::AutoAt4B0470(bool includeHeldKey) const
{
    if ((unsave_.FlagsAt18() & 1u) == 0) return 0;
    if ((unsave_.FlagsAt18() & 2u) != 0) return 2;
    if (includeHeldKey && HeldSkipKey()) return 0;
    return (unsave_.FlagsAt14() & 3u) != 3u ? 1 : 0;
}

bool ReadScriptInputPredicatesRuntime::ReplayValue() const
{
    return values_.ReadSigned(profile_.ResolveNamedValue("replay")) != 0;
}

bool ReadScriptInputPredicatesRuntime::SaveAt47F830() const
{
    // Keyboard vector 从+14起，每项12字节；+EC/+F8 对应 action18/19。
    return input_.ReadPressedAtVslot04(18) && unsave_.BytesAt08()[0] != 0;
}

bool ReadScriptInputPredicatesRuntime::LoadAt47F650() const
{
    return input_.ReadPressedAtVslot04(19) && unsave_.BytesAt08()[1] != 0;
}

bool ReadScriptInputPredicatesRuntime::QuickSaveAt47F780() const
{
    return input_.ReadPressedAtVslot04(20) && unsave_.BytesAt08()[0] != 0 && !ReplayValue();
}

bool ReadScriptInputPredicatesRuntime::QuickLoadAt47F6D0() const
{
    return input_.ReadPressedAtVslot04(21) && unsave_.BytesAt08()[1] != 0 && !ReplayValue();
}

bool ReadScriptInputPredicatesRuntime::LikabilityAt47F530(std::uint8_t checkFlag) const
{
    if (!input_.ReadPressedAtVslot04(22) || unsave_.ModeAt04() == 2 || ReplayValue()) return false;
    // 原5AE270 CP932常量；名称表使用样本profile，不将这个值缓存成独立bool。
    constexpr char name[] = "\x8D\x44\x8A\xB4\x93\x78\x81\x40\x95\x5C\x8E\xA6\x83\x74\x83\x89\x83\x4F";
    if (values_.ReadSigned(profile_.ResolveNamedValue(name)) == 0) return false;
    return checkFlag == 0 || (unsave_.FlagsAt20() & 1u) != 0;
}
}
