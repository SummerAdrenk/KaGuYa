#pragma once
#include <Runtime/SaveLoadPageSecondNative.h>
#include <Runtime/GuiSliderNative.h>
#include <Runtime/InputRootNative.h>
#include <array>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 4EFFA5：+00固定事件号，52C11A只改+04页码；三份Surface各自拥有Ref。
struct SaveLoadPopupNumberNativeBody34h final
{
    std::int32_t eventAt00;
    std::int32_t pageAt04; // 显示下标；-1表示屏外行。物理页号只参与数字合成。
    POINT originAt08;
    union { YtlReferenceSlotRuntime surfacesAt10[3]; };
    SaveLoadPopupNumberNativeBody34h() noexcept {}
};
// 4EF660：+5C/+84/+AC..BC未写，不能用值初始化抹去调用者字节。
struct SaveLoadPopupNativeBodyC0h final
{
    union { GuiDialogNativeBody5Ch common; };
    std::int32_t resultAt5C;
    RECT boundsAt60;
    union { YtlReferenceSlotRuntime managerAt70; };
    std::int32_t selectedPageAt7C; // 父页面的显示下标，空目录为-1。
    std::int32_t firstPageAt80; // 旧版Begin保留父tab窗口起点，再按实际目录夹紧。
    std::int32_t pendingPageAt84; // 接受54/取消53均保留显示下标，绝非物理页号。
    union { GuiControlSurfaceReferences digitsAt88; };
    union { GuiControlSurfaceReferences markersAt94; };
    union { EnvironmentVectorStorage<YtlReferenceSlotRuntime> numbersAtA0; };
    std::int32_t mouseRepeatCountAtAC;
    std::int32_t mouseRepeatEventAtB0;
    std::int32_t keyboardLatchAtB4;
    std::int32_t navigationActionAtB8;
    std::int32_t navigationRepeatAtBC;
    SaveLoadPopupNativeBodyC0h() noexcept {}
};
static_assert(sizeof(SaveLoadPopupNumberNativeBody34h) == 0x34);
static_assert(offsetof(SaveLoadPopupNumberNativeBody34h, pageAt04) == 4);
static_assert(offsetof(SaveLoadPopupNumberNativeBody34h, surfacesAt10) == 0x10);
static_assert(sizeof(SaveLoadPopupNativeBodyC0h) == 0xC0);
static_assert(alignof(SaveLoadPopupNativeBodyC0h) == 4);
static_assert(std::is_standard_layout_v<SaveLoadPopupNativeBodyC0h>);
static_assert(std::is_trivially_destructible_v<SaveLoadPopupNativeBodyC0h>);
#define SAVELOAD_POPUP_OFFSET(field, offset) static_assert(offsetof(SaveLoadPopupNativeBodyC0h, field) == offset)
SAVELOAD_POPUP_OFFSET(common, 0);
SAVELOAD_POPUP_OFFSET(resultAt5C, 0x5C);
SAVELOAD_POPUP_OFFSET(boundsAt60, 0x60);
SAVELOAD_POPUP_OFFSET(managerAt70, 0x70);
SAVELOAD_POPUP_OFFSET(selectedPageAt7C, 0x7C);
SAVELOAD_POPUP_OFFSET(firstPageAt80, 0x80);
SAVELOAD_POPUP_OFFSET(pendingPageAt84, 0x84);
SAVELOAD_POPUP_OFFSET(digitsAt88, 0x88);
SAVELOAD_POPUP_OFFSET(markersAt94, 0x94);
SAVELOAD_POPUP_OFFSET(numbersAtA0, 0xA0);
SAVELOAD_POPUP_OFFSET(mouseRepeatCountAtAC, 0xAC);
SAVELOAD_POPUP_OFFSET(mouseRepeatEventAtB0, 0xB0);
SAVELOAD_POPUP_OFFSET(keyboardLatchAtB4, 0xB4);
SAVELOAD_POPUP_OFFSET(navigationActionAtB8, 0xB8);
SAVELOAD_POPUP_OFFSET(navigationRepeatAtBC, 0xBC);
#undef SAVELOAD_POPUP_OFFSET

class ISaveLoadPopupNativeSource
{
public:
    virtual ~ISaveLoadPopupNativeSource() = default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0() = 0;
    virtual InputRootNativeBody24h* InputAt418210() = 0;
};
// 只借同根Environment/Input owner；构造不Capture，不委托整页业务。
class RootSaveLoadPopupNativeSource final : public ISaveLoadPopupNativeSource
{
public:
    RootSaveLoadPopupNativeSource(EnvironmentRootOwner& environment, InputRootOwner& input) noexcept
        : environment_(environment), input_(input) {}
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override;
    InputRootNativeBody24h* InputAt418210() override;
private:
    EnvironmentRootOwner& environment_;
    InputRootOwner& input_;
};
struct SaveLoadPopupNativeServices final
{
    ISaveLoadPopupNativeSource& source;
    IGuiControlResourceLoadServices& loader;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
};
// main的Task/Scene owner须维持此Scope到逃逸child/number/Surface的最终Ref；Ctor局部Scope不替代它。
class SaveLoadPopupNativeScope final
{
public:
    explicit SaveLoadPopupNativeScope(SaveLoadPopupNativeServices&) noexcept;
    explicit SaveLoadPopupNativeScope(SaveLoadPopupNativeServices*) noexcept;
    ~SaveLoadPopupNativeScope() noexcept;
    SaveLoadPopupNativeScope(const SaveLoadPopupNativeScope&) = delete;
    SaveLoadPopupNativeScope& operator=(const SaveLoadPopupNativeScope&) = delete;
private:
    SaveLoadPopupNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    GuiControlGraphicsScope graphics_;
    GuiInputNativeScope input_;
    GuiDialogNativeScope screen_;
};
SaveLoadPopupNativeServices& RequireSaveLoadPopupNativeServices();
const GuiDialogNativeVtable35& SaveLoadPopupNativeVtable() noexcept;
GuiDialogNativeBody5Ch* ConstructSaveLoadPopupNativeAt4EF660(void* rawC0,
    GuiDialogNativeBody5Ch* parent, std::int32_t id, SaveLoadPopupNativeServices&);
void DestroySaveLoadPopupNative(SaveLoadPopupNativeBodyC0h*) noexcept;
SaveLoadPopupNativeBodyC0h* DeleteSaveLoadPopupNativeAt4FDFE0(SaveLoadPopupNativeBodyC0h*, std::uint32_t) noexcept;
void UpdateSaveLoadPopupNativeAt52B390(GuiDialogNativeBody5Ch*, std::int32_t firstPage, std::int32_t dirty);
void SetSaveLoadPopupSliderNativeAt537C80(GuiDialogNativeBody5Ch*, std::int32_t value);

// Runtime常量已编译；下列原VA仅为独立测试定位证据，生产路径绝不读取PE。
struct SaveLoadPopupNativeWords final { std::uint32_t va; std::span<const std::uint32_t> words; };
struct SaveLoadPopupNativeString final { std::uint32_t va; const char* bytes; };
std::span<const SaveLoadPopupNativeWords> SaveLoadPopupNativeOriginalData() noexcept;
std::span<const SaveLoadPopupNativeString> SaveLoadPopupNativeOriginalStrings() noexcept;
std::span<const std::uint32_t, 35> SaveLoadPopupNativeOriginalSlots() noexcept;
}
