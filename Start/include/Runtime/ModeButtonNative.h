#pragma once

#include <Runtime/CgUiButtonNative.h>
#include <Runtime/GuiDirtyRectNative.h>
#include <Runtime/InputNativeObjects.h>

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
struct ModeButtonNativeBody50h;
using ModeButtonReferenceVector = EnvironmentVectorStorage<YtlReferenceSlotRuntime>;

// 原PE 5B4960/5B4998：两张13槽表，只有删除槽和+10不同；+04不是purecall。
struct ModeButtonNativeVtable13 final
{
    ModeButtonNativeBody50h* (__thiscall* deleteAt00)(ModeButtonNativeBody50h*, std::uint32_t);
    std::int32_t (__thiscall* processAt04)(ModeButtonNativeBody50h*);
    std::int32_t (__thiscall* invalidateAt08)(ModeButtonNativeBody50h*, RECT);
    std::uint8_t (__thiscall* releaseAt0C)(ModeButtonNativeBody50h*);
    std::int32_t (__thiscall* inputAt10)(ModeButtonNativeBody50h*);
    void (__thiscall* resetAllAt14)(ModeButtonNativeBody50h*);
    std::int32_t (__thiscall* resetAt18)(ModeButtonNativeBody50h*, std::int32_t);
    std::int32_t (__thiscall* focusAt1C)(ModeButtonNativeBody50h*, std::int32_t);
    std::int32_t (__thiscall* pressAt20)(ModeButtonNativeBody50h*, std::int32_t);
    std::int32_t (__thiscall* commitAt24)(ModeButtonNativeBody50h*, std::int32_t);
    std::int32_t (__thiscall* selectAt28)(ModeButtonNativeBody50h*, std::int32_t);
    std::int32_t (__thiscall* currentAt2C)(ModeButtonNativeBody50h*);
    std::int32_t (__thiscall* navigateAt30)(ModeButtonNativeBody50h*);
};

// 4B4920读取的继承导航对象视图，不是GuiOperationKey/pair表；不在这里创造或填充它。
struct ModeButtonNavigationNativeBody34h final
{
    std::int32_t itemAt00;
    EnvironmentVectorStorage<std::int32_t> candidatesAt04[4];
};

// IBtnProc前缀3Ch + 两个由派生者赋值的DWORD + GUI三元引用0Ch。
// 4A7270实际sized-delete为3Ch，不根据最末字段猜基类分配尺寸。
struct ModeButtonNativeBody50h final
{
    const ModeButtonNativeVtable13* vptrAt00;
    std::int32_t selectedAt04;
    std::int32_t directionAt08;
    std::int32_t confirmLatchAt0C;
    std::int32_t releaseStateAt10;
    ModeButtonReferenceVector childrenAt14;
    const std::int32_t* neighborsAt20;
    union { YtlReferenceSlotRuntime dirtyAt24; };
    ModeButtonReferenceVector navigationAt30;
    std::int32_t itemCountAt3C;
    std::int32_t skippedGuiAt40;
    union { YtlReferenceSlotRuntime publicGuiAt44; };

    // 4A2E60不写+3C/+40；禁止C++默认成员初始化提前改写原始分配字节。
    ModeButtonNativeBody50h() noexcept {}
    ~ModeButtonNativeBody50h() noexcept {}
};
static_assert(sizeof(ModeButtonNativeBody50h) == 0x50);
static_assert(alignof(ModeButtonNativeBody50h) == 4);
static_assert(std::is_standard_layout_v<ModeButtonNativeBody50h>);
static_assert(sizeof(ModeButtonNativeVtable13) == 0x34);
static_assert(sizeof(ModeButtonNavigationNativeBody34h) == 0x34);
static_assert(offsetof(ModeButtonNavigationNativeBody34h, candidatesAt04) == 0x04);
static_assert(offsetof(ModeButtonNativeBody50h, selectedAt04) == 0x04);
static_assert(offsetof(ModeButtonNativeBody50h, directionAt08) == 0x08);
static_assert(offsetof(ModeButtonNativeBody50h, confirmLatchAt0C) == 0x0C);
static_assert(offsetof(ModeButtonNativeBody50h, releaseStateAt10) == 0x10);
static_assert(offsetof(ModeButtonNativeBody50h, childrenAt14) == 0x14);
static_assert(offsetof(ModeButtonNativeBody50h, neighborsAt20) == 0x20);
static_assert(offsetof(ModeButtonNativeBody50h, dirtyAt24) == 0x24);
static_assert(offsetof(ModeButtonNativeBody50h, navigationAt30) == 0x30);
static_assert(offsetof(ModeButtonNativeBody50h, itemCountAt3C) == 0x3C);
static_assert(offsetof(ModeButtonNativeBody50h, skippedGuiAt40) == 0x40);
static_assert(offsetof(ModeButtonNativeBody50h, publicGuiAt44) == 0x44);
static_assert(offsetof(ModeButtonNativeVtable13, inputAt10) == 0x10);
static_assert(offsetof(ModeButtonNativeVtable13, invalidateAt08) == 0x08);
static_assert(offsetof(ModeButtonNativeVtable13, resetAllAt14) == 0x14);
static_assert(offsetof(ModeButtonNativeVtable13, resetAt18) == 0x18);
static_assert(offsetof(ModeButtonNativeVtable13, focusAt1C) == 0x1C);
static_assert(offsetof(ModeButtonNativeVtable13, pressAt20) == 0x20);
static_assert(offsetof(ModeButtonNativeVtable13, commitAt24) == 0x24);
static_assert(offsetof(ModeButtonNativeVtable13, selectAt28) == 0x28);
static_assert(offsetof(ModeButtonNativeVtable13, currentAt2C) == 0x2C);
static_assert(offsetof(ModeButtonNativeVtable13, navigateAt30) == 0x30);

// 复用唯一CgUiButton输入/图形借用；基类算法留在本模块，不向source转发导航或选择。
struct ModeButtonNativeServices final
{
    CgUiButtonNativeServices& buttons;
    IEnvironmentRootMemory& memory;
};
class ModeButtonInputNativeSource final : public ICgUiButtonInputNativeSource
{
public:
    explicit ModeButtonInputNativeSource(InputRootOwner& owner) noexcept : owner_(owner) {}
    InputRootNativeBody24h* CaptureAt418210() override;
private:
    InputRootOwner& owner_;
};
// 无services形参的输入/虚槽入口由外层绑定scope；构造入口自行绑定，不把借用塞进50h。
class ModeButtonNativeScope final
{
public:
    explicit ModeButtonNativeScope(ModeButtonNativeServices&) noexcept;
    ~ModeButtonNativeScope() noexcept;
    ModeButtonNativeScope(const ModeButtonNativeScope&) = delete;
    ModeButtonNativeScope& operator=(const ModeButtonNativeScope&) = delete;
private:
    ModeButtonNativeServices* previous_;
};

const ModeButtonNativeVtable13& IBtnProcNativeVtable() noexcept;
const ModeButtonNativeVtable13& IModeBtnProcNativeVtable() noexcept;
ModeButtonNativeBody50h* ConstructModeButtonNativeAt4A2E60(
    ModeButtonNativeBody50h&, ModeButtonNativeServices&);
// 原正常析构及构造EH叶都为terminate边界，不以可抛的现代析构替换。
void DestroyButtonBaseNativeAt4A4BB0(ModeButtonNativeBody50h&) noexcept;
void DestroyModeButtonNativeAt4A4D90(ModeButtonNativeBody50h&) noexcept;
ModeButtonNativeBody50h* DeleteButtonBaseNativeAt4A7270(ModeButtonNativeBody50h*, std::uint32_t) noexcept;
ModeButtonNativeBody50h* DeleteModeButtonNativeAt4A7330(ModeButtonNativeBody50h*, std::uint32_t) noexcept;

// 498C10/498CA0允许正常调用的释放异常外传；4A3A00/4A3AB0析构则终止。
void ReleaseModeButtonReferenceRangeNativeAt498C10(YtlReferenceSlotRuntime*, YtlReferenceSlotRuntime*);
void ReleaseModeButtonNavigationRangeNativeAt498CA0(YtlReferenceSlotRuntime*, YtlReferenceSlotRuntime*);
void DestroyModeButtonChildrenNativeAt4A3A00(ModeButtonReferenceVector&) noexcept;
void DestroyModeButtonNavigationNativeAt4A3AB0(ModeButtonReferenceVector&) noexcept;
void ClearModeButtonChildrenNativeAt4A8490(ModeButtonNativeBody50h&);

CgUiButtonNativeBody6Ch* ResolveModeButtonChildNativeAt4ACF20(
    ModeButtonNativeBody50h&, std::int32_t) noexcept;
std::int32_t InvalidateModeButtonNativeAt4B4570(ModeButtonNativeBody50h&, RECT);
std::uint8_t ReadModeButtonReleaseNativeAt4A9C60(ModeButtonNativeBody50h&);
std::int32_t ReadButtonBaseInputNativeAt4A9D90(ModeButtonNativeBody50h&);
std::int32_t ReadModeButtonInputNativeAt4AA200(ModeButtonNativeBody50h&);
std::int32_t ProcessModeButtonNativeAt4BE930(ModeButtonNativeBody50h&);
void ResetModeButtonChildrenNativeAt4A73F0(ModeButtonNativeBody50h&);
std::int32_t ResetModeButtonChildNativeAt4AACD0(ModeButtonNativeBody50h&, std::int32_t);
std::int32_t FocusModeButtonChildNativeAt4ACD30(ModeButtonNativeBody50h&, std::int32_t);
std::int32_t PressModeButtonChildNativeAt4C1980(ModeButtonNativeBody50h&, std::int32_t);
std::int32_t CommitModeButtonChildNativeAt4B10F0(ModeButtonNativeBody50h&, std::int32_t);
std::int32_t SelectModeButtonNativeAt4C2200(ModeButtonNativeBody50h&, std::int32_t) noexcept;
std::int32_t CurrentModeButtonNativeAt4ACFA0(ModeButtonNativeBody50h&) noexcept;
std::int32_t NavigateModeButtonNativeAt4B4920(ModeButtonNativeBody50h&);
}
