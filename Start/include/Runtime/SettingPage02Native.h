#pragma once
#include <Runtime/SettingDialogNative.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
struct SettingPage02NativeBody84h final
{
    union { GuiDialogNativeBody5Ch common; };
    GuiDialogChildrenStorage dialogsAt5C;
    RECT boundsAt68;
    union { YtlReferenceSlotRuntime footerAt78; };
    SettingPage02NativeBody84h() noexcept {}
};
struct SettingPageRadioContainerNativeBody7Ch final
{
    union { GuiDialogNativeBody5Ch common; };
    GuiDialogChildrenStorage dialogsAt5C;
    std::int32_t storedAt68;
    RECT boundsAt6C;
    SettingPageRadioContainerNativeBody7Ch() noexcept {}
};
// FixRadioButtons与Page02Block01同80h形状，但虚表和load/store函数不同。
struct SettingPageChoiceNativeBody80h final
{
    union { GuiDialogNativeBody5Ch common; };
    std::int32_t fieldAt5C;
    std::uint32_t unwrittenAt60;
    RECT boundsAt64;
    union { YtlReferenceSlotRuntime managerAt74; };
    SettingPageChoiceNativeBody80h() noexcept {}
};
struct SettingPage02ModeNativeBody7Ch final
{
    union { GuiDialogNativeBody5Ch common; };
    RECT boundsAt5C;
    union { YtlReferenceSlotRuntime managerAt6C; };
    std::uint8_t modeAt78;
    std::uint8_t unwrittenAt79[3];
    SettingPage02ModeNativeBody7Ch() noexcept {}
};
static_assert(sizeof(SettingPage02NativeBody84h) == 0x84);
static_assert(sizeof(SettingPageRadioContainerNativeBody7Ch) == 0x7C);
static_assert(sizeof(SettingPageChoiceNativeBody80h) == 0x80);
static_assert(sizeof(SettingPage02ModeNativeBody7Ch) == 0x7C);
static_assert(offsetof(SettingPage02NativeBody84h, footerAt78) == 0x78);
static_assert(offsetof(SettingPage02NativeBody84h, boundsAt68) == 0x68);
static_assert(offsetof(SettingPageRadioContainerNativeBody7Ch, storedAt68) == 0x68);
static_assert(offsetof(SettingPageRadioContainerNativeBody7Ch, boundsAt6C) == 0x6C);
static_assert(offsetof(SettingPageChoiceNativeBody80h, managerAt74) == 0x74);
static_assert(offsetof(SettingPage02ModeNativeBody7Ch, managerAt6C) == 0x6C);
static_assert(offsetof(SettingPage02ModeNativeBody7Ch, modeAt78) == 0x78);
static_assert(std::is_trivially_destructible_v<SettingPage02NativeBody84h>);
static_assert(std::is_standard_layout_v<SettingPage02NativeBody84h>);
struct SettingPageNativeVtable39 final
{
    GuiDialogIntoNativeVtable37 common;
    void (__thiscall* reloadAt94)(GuiDialogNativeBody5Ch*);
    RECT* (__thiscall* boundsAt98)(GuiDialogNativeBody5Ch*, RECT*);
};
struct SettingPageRadioContainerNativeVtable40 final
{
    GuiDialogIntoNativeVtable37 common;
    RECT* (__thiscall* boundsAt94)(GuiDialogNativeBody5Ch*, RECT*);
    void (__thiscall* loadAt98)(GuiDialogNativeBody5Ch*);
    void (__thiscall* storeAt9C)(GuiDialogNativeBody5Ch*);
};
struct SettingPageChoiceNativeVtable38 final
{
    GuiDialogNativeVtable35 common;
    RECT* (__thiscall* boundsAt8C)(GuiDialogNativeBody5Ch*, RECT*);
    void (__thiscall* loadAt90)(GuiDialogNativeBody5Ch*);
    void (__thiscall* storeAt94)(GuiDialogNativeBody5Ch*);
};
static_assert(sizeof(SettingPageNativeVtable39) == 0x9C);
static_assert(sizeof(SettingPageRadioContainerNativeVtable40) == 0xA0);
static_assert(sizeof(SettingPageChoiceNativeVtable38) == 0x98);
static_assert(offsetof(SettingPageRadioContainerNativeVtable40, storeAt9C) == 0x9C);
enum class SettingPageRadioContainerKind : std::uint32_t
{ Page01Block00, Page01Block01, Page02Block00, Page02Block03, SceneChoices };
struct SettingPageRadioGroupNative final
{
    std::int32_t id;
    std::span<const GuiButtonDescriptorNative2Ch> buttons;
};
class ISettingPage02NativeSource
{
public:
    virtual ~ISettingPage02NativeSource() = default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0() = 0;
    virtual void EnsureRootAt413BE0() = 0;
    virtual void* RootAt6198C4() = 0;
    // 借固定Root唯一原Ref，不再次Ensure/选根/增持；目标是真实WindowMode2Ch。
    virtual YtlReferenceSlotRuntime& RootWindowModeSlotAt0C(void* fixedRoot) = 0;
    virtual BOOL PostMessageA(HWND, UINT, WPARAM, LPARAM) = 0;
};
struct SettingPage02NativeServices final
{
    ISettingPage02NativeSource& source;
    IGuiControlResourceLoadServices& loader;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory();
};
class SettingPage02NativeScope final
{
public:
    explicit SettingPage02NativeScope(SettingPage02NativeServices&) noexcept;
    explicit SettingPage02NativeScope(SettingPage02NativeServices*) noexcept;
    ~SettingPage02NativeScope() noexcept;
    SettingPage02NativeScope(const SettingPage02NativeScope&) = delete;
    SettingPage02NativeScope& operator=(const SettingPage02NativeScope&) = delete;
private:
    SettingPage02NativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    GuiControlGraphicsScope graphics_;
    GuiInputNativeScope input_;
    GuiDialogNativeScope screen_;
};
SettingPage02NativeServices& RequireSettingPage02NativeServices();
const SettingPageNativeVtable39& SettingPage02NativeVtable() noexcept;
const SettingPageNativeVtable39& SettingPageNativeBaseVtable() noexcept;
const SettingPageRadioContainerNativeVtable40& SettingPageRadioContainerNativeVtable(SettingPageRadioContainerKind) noexcept;
const SettingPageChoiceNativeVtable38& SettingFixRadioButtonsNativeVtable() noexcept;
const SettingPageChoiceNativeVtable38& SettingPage02Block01NativeVtable() noexcept;
const GuiDialogNativeVtable35& SettingPage02Block02NativeVtable() noexcept;
SettingPage02NativeBody84h* ConstructSettingPage02NativeAt4F23B0(void*, GuiDialogNativeBody5Ch* parent, std::int32_t id);
SettingPageRadioContainerNativeBody7Ch* ConstructSettingPage02Block00NativeAt4E5E40(void*, GuiDialogNativeBody5Ch*, std::int32_t);
SettingPageChoiceNativeBody80h* ConstructSettingPage02Block01NativeAt4E6070(void*, GuiDialogNativeBody5Ch*, std::int32_t);
SettingPage02ModeNativeBody7Ch* ConstructSettingPage02Block02NativeAt4E64D0(void*, GuiDialogNativeBody5Ch*, std::int32_t);
SettingPageRadioContainerNativeBody7Ch* ConstructSettingPage02Block03NativeAt4E6930(void*, GuiDialogNativeBody5Ch*, std::int32_t);
SettingPageChoiceNativeBody80h* ConstructSettingFixRadioButtonsNativeAt4E07B0(void*, GuiDialogNativeBody5Ch*,
    std::int32_t id, std::span<const GuiButtonDescriptorNative2Ch>);
void DestroySettingPageNativeAt4F9840(SettingPage02NativeBody84h*) noexcept;
SettingPage02NativeBody84h* DeleteSettingPage02NativeAt4FE350(SettingPage02NativeBody84h*, std::uint32_t) noexcept;
std::int32_t ReadSettingRadioChoiceNativeAt50ADE0(GuiDialogNativeBody5Ch&);
void WriteSettingRadioChoiceNativeAt537B10(GuiDialogNativeBody5Ch&, std::int32_t);
std::span<const SettingPageRadioGroupNative> SettingPageRadioGroupsSample18(SettingPageRadioContainerKind) noexcept;
std::span<const GuiButtonDescriptorNative2Ch, 5> SettingPage02Block01ButtonsSample18() noexcept;
std::span<const GuiButtonDescriptorNative2Ch, 2> SettingPage02Block02ButtonsSample18() noexcept;
const GuiStaticDescriptorNative24h& SettingPage02Block02StaticSample18() noexcept;
const GuiStaticDescriptorNative24h& SettingPage02FooterSample18() noexcept;
std::span<const std::uint32_t> SettingPage02OriginalTableSample18(std::uint32_t index) noexcept;
std::span<const std::uint32_t> SettingPage02OriginalUnwindSample18(std::uint32_t index) noexcept;
}
