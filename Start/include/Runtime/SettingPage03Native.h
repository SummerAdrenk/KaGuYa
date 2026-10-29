#pragma once
#include <Runtime/GuiDialogIntoNative.h>
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiSliderNative.h>
#include <Runtime/GuiClickedEventNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <Runtime/SoundManagerNativePlayback.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 4F2910: 84/88构造不写，50F630正常尾才写-1；禁止值初始化整个正文。
struct SettingPage03NativeBody8Ch final
{
    union { GuiDialogNativeBody5Ch common; };
    GuiDialogChildrenStorage dialogsAt5C;
    RECT boundsAt68;
    union { YtlReferenceSlotRuntime footerAt78; };
    std::int32_t typePreviewAt84;
    std::int32_t characterPreviewAt88;
    SettingPage03NativeBody8Ch() noexcept {}
};
struct SettingPage03BlockNativeBody80h final
{
    union { GuiDialogNativeBody5Ch common; };
    std::int32_t storedAt5C;
    std::uint32_t unwrittenAt60;
    RECT boundsAt64;
    union { YtlReferenceSlotRuntime managerAt74; };
    SettingPage03BlockNativeBody80h() noexcept {}
};
struct SettingPage03Block04NativeBody84h final
{
    union { SettingPage03BlockNativeBody80h block; };
    std::int32_t pageAt80;
    SettingPage03Block04NativeBody84h() noexcept {}
};
struct SettingPage03Block03NativeBody7Ch final
{
    union { GuiDialogNativeBody5Ch common; };
    GuiDialogChildrenStorage dialogsAt5C;
    std::int32_t storedAt68;
    RECT boundsAt6C;
    SettingPage03Block03NativeBody7Ch() noexcept {}
};
struct SettingPage03NativeVtable39 final
{
    GuiDialogIntoNativeVtable37 common;
    void (__thiscall* reloadAt94)(GuiDialogNativeBody5Ch*);
    RECT* (__thiscall* boundsAt98)(GuiDialogNativeBody5Ch*, RECT*);
};
struct SettingPage03BlockNativeVtable42 final
{
    GuiDialogNativeVtable35 common;
    RECT* (__thiscall* boundsAt8C)(GuiDialogNativeBody5Ch*, RECT*);
    void (__thiscall* loadAt90)(GuiDialogNativeBody5Ch*);
    void (__thiscall* storeAt94)(GuiDialogNativeBody5Ch*);
    BOOL (__thiscall* checkedAt98)(GuiDialogNativeBody5Ch*, std::int32_t);
    std::int32_t (__thiscall* valueAt9C)(GuiDialogNativeBody5Ch*, std::int32_t, std::uint8_t);
    void (__thiscall* setCheckedAtA0)(GuiDialogNativeBody5Ch*, std::int32_t, std::int32_t, std::int32_t);
    std::int32_t (__thiscall* previewForSliderAtA4)(GuiDialogNativeBody5Ch*, std::int32_t);
};
struct SettingPage03Block03NativeVtable41 final
{
    GuiDialogIntoNativeVtable37 common;
    RECT* (__thiscall* boundsAt94)(GuiDialogNativeBody5Ch*, RECT*);
    void (__thiscall* loadAt98)(GuiDialogNativeBody5Ch*);
    void (__thiscall* storeAt9C)(GuiDialogNativeBody5Ch*);
    void (__thiscall* setGateAtA0)(GuiDialogNativeBody5Ch*, std::int32_t, std::int32_t);
};
static_assert(sizeof(void*) == 4, "SettingPage03 requires x86");
static_assert(sizeof(SettingPage03NativeBody8Ch) == 0x8C);
static_assert(offsetof(SettingPage03NativeBody8Ch, typePreviewAt84) == 0x84);
static_assert(offsetof(SettingPage03NativeBody8Ch, footerAt78) == 0x78);
static_assert(sizeof(SettingPage03BlockNativeBody80h) == 0x80);
static_assert(offsetof(SettingPage03BlockNativeBody80h, managerAt74) == 0x74);
static_assert(sizeof(SettingPage03Block04NativeBody84h) == 0x84);
static_assert(offsetof(SettingPage03Block04NativeBody84h, pageAt80) == 0x80);
static_assert(sizeof(SettingPage03Block03NativeBody7Ch) == 0x7C);
static_assert(offsetof(SettingPage03Block03NativeBody7Ch, boundsAt6C) == 0x6C);
static_assert(sizeof(SettingPage03NativeVtable39) == 0x9C);
static_assert(sizeof(SettingPage03BlockNativeVtable42) == 0xA8);
static_assert(offsetof(SettingPage03BlockNativeVtable42, previewForSliderAtA4) == 0xA4);
static_assert(sizeof(SettingPage03Block03NativeVtable41) == 0xA4);
static_assert(std::is_trivially_destructible_v<SettingPage03NativeBody8Ch>);
static_assert(std::is_standard_layout_v<SettingPage03Block04NativeBody84h>);

class ISettingPage03NativeSource
{
public:
    virtual ~ISettingPage03NativeSource() = default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0() = 0;
    // 外部singleton真实inline获取：601160锁、6198C8惰性60h构造、解锁。
    // 不得用配置快照、恒定空根或忽略锁/EH的成功stub替代。
    virtual EnvironmentRootStorage* CaptureEnvironmentInlineAt6198C8() = 0;
    virtual void EnsureRootAt413BE0() = 0;
    virtual void* RootAt6198C4() = 0;
    virtual YtlReferenceSlotRuntime& RootSoundSlotAt24(void* fixedRoot) = 0;
};
struct SettingPage03NativeServices final
{
    ISettingPage03NativeSource& source;
    IGuiControlResourceLoadServices& loader;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    SoundModuleNativeApi& sound;
    ISoundManagerNativeOpen& open;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory();
};
class SettingPage03NativeScope final
{
public:
    explicit SettingPage03NativeScope(SettingPage03NativeServices&) noexcept;
    ~SettingPage03NativeScope() noexcept;
    SettingPage03NativeScope(const SettingPage03NativeScope&) = delete;
    SettingPage03NativeScope& operator=(const SettingPage03NativeScope&) = delete;
private:
    SettingPage03NativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    GuiControlGraphicsScope graphics_;
    GuiInputNativeScope input_;
    GuiDialogNativeScope screen_;
};
SettingPage03NativeServices& RequireSettingPage03NativeServices();
const SettingPage03NativeVtable39& SettingPage03NativeVtable() noexcept;
const SettingPage03BlockNativeVtable42& SettingPage03BlockNativeVtable(std::uint32_t block);
const SettingPage03Block03NativeVtable41& SettingPage03Block03NativeVtable() noexcept;
SettingPage03NativeBody8Ch* ConstructSettingPage03NativeAt4F2910(void*, GuiDialogNativeBody5Ch*, std::int32_t id);
SettingPage03BlockNativeBody80h* ConstructSettingPage03Block00NativeAt4E6B60(void*, GuiDialogNativeBody5Ch*, std::int32_t);
SettingPage03BlockNativeBody80h* ConstructSettingPage03Block01NativeAt4E7050(void*, GuiDialogNativeBody5Ch*, std::int32_t);
SettingPage03BlockNativeBody80h* ConstructSettingPage03Block02NativeAt4E7540(void*, GuiDialogNativeBody5Ch*, std::int32_t);
SettingPage03Block03NativeBody7Ch* ConstructSettingPage03Block03NativeAt4E7F30(void*, GuiDialogNativeBody5Ch*, std::int32_t);
SettingPage03Block04NativeBody84h* ConstructSettingPage03Block04NativeAt4E8130(void*, GuiDialogNativeBody5Ch*, std::int32_t);
void DestroySettingPage03NativeAt4F9840(SettingPage03NativeBody8Ch*) noexcept;
SettingPage03NativeBody8Ch* DeleteSettingPage03NativeAt4FE380(SettingPage03NativeBody8Ch*, std::uint32_t flags) noexcept;

enum class SettingPage03ControlKind : std::uint32_t { Static, Button, CheckBox, Slider };
struct SettingPage03ControlGroup final
{
    SettingPage03ControlKind kind;
    std::uint32_t count;
    const void* descriptors;
};
struct SettingPage03NamedRow final { const char* name; std::int32_t slider, checkbox; };
struct SettingPage03PreviewRow final { std::int32_t button, checkbox, slider; };
struct SettingPage03MuteRow final { std::int32_t checkbox; std::span<const std::int32_t> dependents; };
struct SettingPage03LabelPair final { const GuiStaticDescriptorNative24h* shown; const GuiStaticDescriptorNative24h* hidden; };
std::span<const SettingPage03ControlGroup> SettingPage03GroupsSample18(std::uint32_t block);
std::span<const SettingPage03LabelPair, 9> SettingPage03LabelsSample18() noexcept;
std::span<const SettingPage03NamedRow, 6> SettingPage03TypesSample18() noexcept;
std::span<const SettingPage03NamedRow, 9> SettingPage03CharactersSample18() noexcept;
std::span<const SettingPage03PreviewRow> SettingPage03PreviewRowsSample18(bool characters) noexcept;
std::span<const SettingPage03MuteRow, 18> SettingPage03MuteRowsSample18() noexcept;
std::span<const GuiButtonDescriptorNative2Ch, 2> SettingPage03RadioButtonsSample18() noexcept;
const GuiStaticDescriptorNative24h& SettingPage03LoopLabelSample18() noexcept;
const GuiStaticDescriptorNative24h& SettingPage03FooterSample18() noexcept;
const char* SettingPage03RegistrationNameSample18() noexcept;
std::span<const std::uint32_t> SettingPage03OriginalTableSample18(std::uint32_t index);
std::span<const std::uint32_t> SettingPage03OriginalUnwindSample18(std::uint32_t index);
}
