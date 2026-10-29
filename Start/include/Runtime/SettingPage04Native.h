#pragma once
#include <Runtime/GuiDialogIntoNative.h>
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiSliderNative.h>
#include <Runtime/GraphicsTextNativeApi.h>
#include <Runtime/SaveLoadMessageBoxNativeChildFactoriesRuntime.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/GameResourceNamePrompt.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 父84构造不写；50F880在全部子Begin成功之后才置-1。
struct SettingPage04NativeBody88h final
{
    union { GuiDialogNativeBody5Ch common; };
    GuiDialogChildrenStorage dialogsAt5C;
    RECT boundsAt68;
    union { YtlReferenceSlotRuntime footerAt78; };
    std::int32_t resultAt84;
    SettingPage04NativeBody88h() noexcept {}
};
struct SettingPage04ContainerNativeBody7Ch final
{
    union { GuiDialogNativeBody5Ch common; };
    GuiDialogChildrenStorage dialogsAt5C;
    std::int32_t storedAt68;
    RECT boundsAt6C;
    SettingPage04ContainerNativeBody7Ch() noexcept {}
};
struct SettingPage04Block01NativeBody94h final
{
    union { SettingPage04ContainerNativeBody7Ch container; };
    union { YtlReferenceSlotRuntime textAt7C; };
    union { YtlReferenceSlotRuntime managerAt88; };
    SettingPage04Block01NativeBody94h() noexcept {}
};
struct SettingPage04ManagedNativeBody88h final
{
    union { SettingPage04ContainerNativeBody7Ch container; };
    union { YtlReferenceSlotRuntime managerAt7C; };
    SettingPage04ManagedNativeBody88h() noexcept {}
};
struct SettingPage04RadioNativeBody80h final
{
    union { GuiDialogNativeBody5Ch common; };
    std::int32_t fieldAt5C;
    std::uint32_t unwrittenAt60;
    RECT boundsAt64;
    union { YtlReferenceSlotRuntime managerAt74; };
    SettingPage04RadioNativeBody80h() noexcept {}
};
struct SettingPage04Block03NativeBody8Ch final
{
    union { SettingPage04RadioNativeBody80h block; };
    GuiControlSurfaceReferences surfacesAt80;
    SettingPage04Block03NativeBody8Ch() noexcept {}
};
struct SettingPage04NativeVtable39 final
{
    GuiDialogIntoNativeVtable37 common;
    void (__thiscall* reloadAt94)(GuiDialogNativeBody5Ch*);
    RECT* (__thiscall* boundsAt98)(GuiDialogNativeBody5Ch*, RECT*);
};
struct SettingPage04ContainerNativeVtable40 final
{
    GuiDialogIntoNativeVtable37 common;
    RECT* (__thiscall* boundsAt94)(GuiDialogNativeBody5Ch*, RECT*);
    void (__thiscall* loadAt98)(GuiDialogNativeBody5Ch*);
    void (__thiscall* storeAt9C)(GuiDialogNativeBody5Ch*);
};
struct SettingPage04ManagedNativeVtable41 final
{
    SettingPage04ContainerNativeVtable40 common;
    void (__thiscall* gateAtA0)(GuiDialogNativeBody5Ch*, std::int32_t, std::int32_t);
};
struct SettingPage04RadioNativeVtable38 final
{
    GuiDialogNativeVtable35 common;
    RECT* (__thiscall* boundsAt8C)(GuiDialogNativeBody5Ch*, RECT*);
    void (__thiscall* loadAt90)(GuiDialogNativeBody5Ch*);
    void (__thiscall* storeAt94)(GuiDialogNativeBody5Ch*);
};
static_assert(sizeof(void*) == 4, "SettingPage04 requires x86");
static_assert(sizeof(SettingPage04NativeBody88h) == 0x88);
static_assert(offsetof(SettingPage04NativeBody88h,resultAt84) == 0x84);
static_assert(sizeof(SettingPage04ContainerNativeBody7Ch) == 0x7C);
static_assert(offsetof(SettingPage04ContainerNativeBody7Ch,storedAt68) == 0x68);
static_assert(offsetof(SettingPage04ContainerNativeBody7Ch,boundsAt6C) == 0x6C);
static_assert(sizeof(SettingPage04Block01NativeBody94h) == 0x94);
static_assert(offsetof(SettingPage04Block01NativeBody94h,textAt7C) == 0x7C);
static_assert(offsetof(SettingPage04Block01NativeBody94h,managerAt88) == 0x88);
static_assert(sizeof(SettingPage04ManagedNativeBody88h) == 0x88);
static_assert(offsetof(SettingPage04ManagedNativeBody88h,managerAt7C) == 0x7C);
static_assert(sizeof(SettingPage04RadioNativeBody80h) == 0x80);
static_assert(sizeof(SettingPage04Block03NativeBody8Ch) == 0x8C);
static_assert(offsetof(SettingPage04Block03NativeBody8Ch,surfacesAt80) == 0x80);
static_assert(sizeof(SettingPage04NativeVtable39) == 0x9C);
static_assert(sizeof(SettingPage04ContainerNativeVtable40) == 0xA0);
static_assert(sizeof(SettingPage04ManagedNativeVtable41) == 0xA4);
static_assert(sizeof(SettingPage04RadioNativeVtable38) == 0x98);
static_assert(std::is_standard_layout_v<SettingPage04Block01NativeBody94h>);
static_assert(std::is_trivially_destructible_v<SettingPage04NativeBody88h>);

class ISettingPage04NativeSource
{
public:
    virtual ~ISettingPage04NativeSource() = default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0() = 0;
    // 必须借各自真实全局CString：406BC0/407430先GetNil再赋13-byte初值。
    // 下方sample18初值只供宿主还原初始化，不得作为getter缺项时的fallback。
    virtual const char* FontFaceAt61A64C() = 0;
    virtual const char* FontFaceAt61A650() = 0;
    // 02-10页内调用RT_DIALOG112；其余版本不消费此叶。未绑定必须明确失败。
    virtual GameResourceNamePromptResult RunLocalNamePrompt();
};
// 仅三个真实Graphics导出叶，不能委托整个页/块/文字构造算法。
class ISettingPage04NativeTextApi
{
public:
    virtual ~ISettingPage04NativeTextApi() = default;
    virtual bool CreateFontAt4388B0(const GraphicsTextNativeFontParameters*) = 0;
    virtual bool InitializeAt4389D0(const char*,std::int32_t,std::int32_t) = 0;
    virtual bool BlitAllAt53A980(std::int32_t,RECT*) = 0;
};
class SettingPage04NativeTextApi final : public ISettingPage04NativeTextApi
{
public:
    explicit SettingPage04NativeTextApi(GraphicsTextNativeApi& api) noexcept : api_(api) {}
    bool CreateFontAt4388B0(const GraphicsTextNativeFontParameters*) override;
    bool InitializeAt4389D0(const char*,std::int32_t,std::int32_t) override;
    bool BlitAllAt53A980(std::int32_t,RECT*) override;
private:
    GraphicsTextNativeApi& api_;
};
struct SettingPage04NativeServices final
{
    ISettingPage04NativeSource& source;
    IGuiControlResourceLoadServices& loader;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    ISettingPage04NativeTextApi& text;
    IGraphicsWindowSurfaceApi& drawing;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory();
};
class SettingPage04NativeScope final
{
public:
    explicit SettingPage04NativeScope(SettingPage04NativeServices&) noexcept;
    ~SettingPage04NativeScope() noexcept;
    SettingPage04NativeScope(const SettingPage04NativeScope&) = delete;
    SettingPage04NativeScope& operator=(const SettingPage04NativeScope&) = delete;
private:
    SettingPage04NativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    GuiControlGraphicsScope graphics_;
    GuiInputNativeScope input_;
    GuiDialogNativeScope screen_;
};
SettingPage04NativeServices& RequireSettingPage04NativeServices();
const SettingPage04NativeVtable39& SettingPage04NativeVtable() noexcept;
const SettingPage04ContainerNativeVtable40& SettingPage04ContainerNativeVtable(std::uint32_t kind);
const SettingPage04ManagedNativeVtable41& SettingPage04ManagedNativeVtable(std::uint32_t kind);
const SettingPage04RadioNativeVtable38& SettingPage04Block03NativeVtable() noexcept;
SettingPage04NativeBody88h* ConstructSettingPage04NativeAt4F2FE0(void*,GuiDialogNativeBody5Ch*,std::int32_t);
SettingPage04ContainerNativeBody7Ch* ConstructSettingPage04Block00NativeAt4E8590(void*,GuiDialogNativeBody5Ch*,std::int32_t);
SettingPage04Block01NativeBody94h* ConstructSettingPage04Block01NativeAt4E8900(void*,GuiDialogNativeBody5Ch*,std::int32_t);
SettingPage04ManagedNativeBody88h* ConstructSettingPage04Block02NativeAt4E8EA0(void*,GuiDialogNativeBody5Ch*,std::int32_t);
SettingPage04Block03NativeBody8Ch* ConstructSettingPage04Block03NativeAt4E9450(void*,GuiDialogNativeBody5Ch*,std::int32_t);
SettingPage04ContainerNativeBody7Ch* ConstructSettingPage04Block04NativeAt4E9960(void*,GuiDialogNativeBody5Ch*,std::int32_t);
SettingPage04ManagedNativeBody88h* ConstructSettingPage04Block05NativeAt4E9B90(void*,GuiDialogNativeBody5Ch*,std::int32_t);
void DestroySettingPage04NativeAt4F9840(SettingPage04NativeBody88h*) noexcept;
SettingPage04NativeBody88h* DeleteSettingPage04NativeAt4FE3B0(SettingPage04NativeBody88h*,std::uint32_t) noexcept;
struct SettingPage04RadioGroup final { std::int32_t id;std::span<const GuiButtonDescriptorNative2Ch> buttons; };
std::span<const SettingPage04RadioGroup> SettingPage04RadioGroupsSample18(std::uint32_t kind);
std::span<const GuiStaticDescriptorNative24h> SettingPage04StaticsSample18(std::uint32_t kind);
const GuiButtonDescriptorNative2Ch& SettingPage04NameButtonSample18() noexcept;
const GuiSliderDescriptorNative6Ch& SettingPage04SpeedSliderSample18() noexcept;
const SaveLoadMessageBoxCheckBoxDescriptorNative48h& SettingPage04CountCheckSample18() noexcept;
const GuiStaticDescriptorNative24h& SettingPage04FooterSample18() noexcept;
const char* SettingPage04FontFaceInitialSample18() noexcept;
POINT SettingPage04TextAnchorSample18(std::uint32_t index);
std::span<const std::uint32_t> SettingPage04OriginalTableSample18(std::uint32_t index);
std::span<const std::uint32_t> SettingPage04OriginalUnwindSample18(std::uint32_t index);
}
