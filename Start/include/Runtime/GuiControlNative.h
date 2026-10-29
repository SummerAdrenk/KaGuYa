#pragma once

#include <Runtime/GuiControlResourceNative.h>
#include <Runtime/GuiClickedEventNative.h>

namespace KaGuYa::Start::Runtime
{
class IGraphicsSurfaceCompositionApi;
class IGraphicsSurfacePixelApi;
struct GuiControlNativeBody50h;
struct GuiCheckBoxNativeBody60h;

// 5BDE40/5BDDE0共有23槽。按值Ref是callee拥有参数，不是C++引用。
struct GuiControlNativeVtable23 final
{
    GuiControlNativeBody50h* (__thiscall* deletingDestructorAt00)(GuiControlNativeBody50h*, std::uint32_t);
    std::int32_t (__thiscall* queryAt04)(GuiControlNativeBody50h*);
    void (__thiscall* resetAt08)(GuiControlNativeBody50h*);
    void (__thiscall* drawAt0C)(GuiControlNativeBody50h*, const RECT*);
    void (__thiscall* compositeAt10)(GuiControlNativeBody50h*, YtlReferenceSlotRuntime, const RECT*);
    std::int32_t (__thiscall* keyboardAt14)(GuiControlNativeBody50h*);
    std::int32_t (__thiscall* mouseAt18)(GuiControlNativeBody50h*);
    std::int32_t (__thiscall* enabledAt1C)(GuiControlNativeBody50h*);
    void (__thiscall* focusAt20)(GuiControlNativeBody50h*);
    void (__thiscall* blurAt24)(GuiControlNativeBody50h*);
    std::int32_t (__thiscall* hoverAt28)(GuiControlNativeBody50h*);
    void (__thiscall* enterAt2C)(GuiControlNativeBody50h*);
    void (__thiscall* leaveAt30)(GuiControlNativeBody50h*);
    void (__thiscall* nullAt34)(GuiControlNativeBody50h*);
    std::int32_t (__thiscall* resourceStateAt38)(GuiControlNativeBody50h*);
    RECT* (__thiscall* rectAt3C)(GuiControlNativeBody50h*, RECT*);
    std::int32_t (__thiscall* hitAt40)(GuiControlNativeBody50h*, std::int32_t, std::int32_t);
    void (__thiscall* writeStateAt44)(GuiControlNativeBody50h*, std::int32_t);
    std::int32_t (__thiscall* readStateAt48)(GuiControlNativeBody50h*);
    void (__thiscall* writeEnabledAt4C)(GuiControlNativeBody50h*, std::int32_t);
    std::int32_t (__thiscall* readEnabledAt50)(GuiControlNativeBody50h*);
    void (__thiscall* nullAt54)(GuiControlNativeBody50h*, std::int32_t);
    std::int32_t (__thiscall* matchesAt58)(GuiControlNativeBody50h*, std::int32_t, std::int32_t);
};
struct GuiCheckBoxNativeVtable30 final
{
    GuiControlNativeVtable23 common;
    YtlReferenceSlotRuntime* (__thiscall* resourceAt5C)(GuiControlNativeBody50h*, YtlReferenceSlotRuntime*, std::int32_t);
    void (__thiscall* setHitSlotAt60)(GuiControlNativeBody50h*, std::int32_t);
    std::int32_t (__thiscall* hasHitSlotAt64)(GuiControlNativeBody50h*);
    std::int32_t (__thiscall* hitSlotAt68)(GuiControlNativeBody50h*);
    std::int32_t (__thiscall* pressedAt6C)(GuiControlNativeBody50h*);
    void (__thiscall* setPressedAt70)(GuiControlNativeBody50h*, std::int32_t);
    POINT* (__thiscall* positionAt74)(GuiControlNativeBody50h*, POINT*, std::int32_t, std::int32_t);
};
// 5BDF0C：Button只有29槽；CheckBox的+74不能出现在该表中。
struct GuiButtonNativeVtable29 final
{
    GuiControlNativeVtable23 common;
    YtlReferenceSlotRuntime* (__thiscall* resourceAt5C)(GuiControlNativeBody50h*, YtlReferenceSlotRuntime*, std::int32_t);
    void (__thiscall* setHitSlotAt60)(GuiControlNativeBody50h*, std::int32_t);
    std::int32_t (__thiscall* hasHitSlotAt64)(GuiControlNativeBody50h*);
    std::int32_t (__thiscall* hitSlotAt68)(GuiControlNativeBody50h*);
    std::int32_t (__thiscall* pressedAt6C)(GuiControlNativeBody50h*);
    void (__thiscall* setPressedAt70)(GuiControlNativeBody50h*, std::int32_t);
};
struct GuiControlNativeBody50h final
{
    const GuiControlNativeVtable23* vptrAt00;
    std::int32_t eventAt04;
    std::int32_t typeAt08;
    std::int32_t childIdAt0C;
    std::uint32_t unknownAt10;
    std::int32_t stateAt14;
    std::int32_t enabledAt18;
    std::int32_t hoverAt1C;
    std::int32_t focusAt20;
    std::uint32_t unknownAt24;
    GuiControlResourceReferences resourcesAt28;
    std::int32_t xAt34;
    std::int32_t yAt38;
    YtlReferenceSlotRuntime eventAt3C;
    std::int32_t eventEnabledAt48;
    void* parentAt4C;
};
struct GuiCheckBoxNativeBody60h final
{
    GuiControlNativeBody50h common;
    std::int32_t pressedAt50;
    std::int32_t hitSlotAt54;
    std::int32_t keyboardLatchAt58;
    std::int32_t checkedAt5C;
};
struct GuiButtonNativeBody5Ch final
{
    GuiControlNativeBody50h common;
    std::int32_t pressedAt50;
    std::int32_t hitSlotAt54;
    std::int32_t keyboardLatchAt58;
};
static_assert(sizeof(GuiButtonNativeBody5Ch) == 0x5C);
static_assert(offsetof(GuiButtonNativeBody5Ch, keyboardLatchAt58) == 0x58);
static_assert(sizeof(GuiButtonNativeVtable29) == 0x74);
static_assert(offsetof(GuiButtonNativeVtable29, setPressedAt70) == 0x70);
static_assert(sizeof(GuiControlNativeBody50h) == 0x50);
static_assert(alignof(GuiControlNativeBody50h) == 4);
static_assert(offsetof(GuiControlNativeBody50h, resourcesAt28) == 0x28);
static_assert(offsetof(GuiControlNativeBody50h, xAt34) == 0x34);
static_assert(offsetof(GuiControlNativeBody50h, eventAt3C) == 0x3C);
static_assert(offsetof(GuiControlNativeBody50h, parentAt4C) == 0x4C);
static_assert(sizeof(GuiCheckBoxNativeBody60h) == 0x60);
static_assert(offsetof(GuiCheckBoxNativeBody60h, checkedAt5C) == 0x5C);
static_assert(sizeof(GuiControlNativeVtable23) == 0x5C);
static_assert(offsetof(GuiControlNativeVtable23, compositeAt10) == 0x10);
static_assert(offsetof(GuiControlNativeVtable23, matchesAt58) == 0x58);
static_assert(sizeof(GuiCheckBoxNativeVtable30) == 0x78);
static_assert(offsetof(GuiCheckBoxNativeVtable30, resourceAt5C) == 0x5C);
static_assert(offsetof(GuiCheckBoxNativeVtable30, positionAt74) == 0x74);
static_assert(std::is_trivially_destructible_v<GuiControlNativeBody50h>);
static_assert(std::is_trivially_destructible_v<GuiCheckBoxNativeBody60h>);

const GuiControlNativeVtable23& GuiControlBaseVtable() noexcept;
const GuiControlNativeVtable23& GuiStaticVtable() noexcept;
const GuiCheckBoxNativeVtable30& GuiCheckBoxVtable() noexcept;
const GuiButtonNativeVtable29& GuiButtonVtable() noexcept;
GuiButtonNativeBody5Ch* ConstructGuiButtonAt4DA700(GuiButtonNativeBody5Ch* body,
    void* parent, std::int32_t event, YtlReferenceSlotRuntime ownedGroup);
YtlReferenceSlotRuntime* ConstructToolButtonReferenceAt4E2C61(YtlReferenceSlotRuntime& output,
    GuiButtonNativeBody5Ch* body);
YtlReferenceSlotRuntime* ConstructGuiButtonReferenceAt504298(YtlReferenceSlotRuntime& output,
    GuiButtonNativeBody5Ch* body);
GuiControlNativeBody50h* ConstructGuiStaticAt4F4050(GuiControlNativeBody50h* body,
    void* parent, std::int32_t event, YtlReferenceSlotRuntime ownedGroup);
GuiCheckBoxNativeBody60h* ConstructGuiCheckBoxAt4E0520(GuiCheckBoxNativeBody60h* body,
    void* parent, std::int32_t event, YtlReferenceSlotRuntime ownedNormal, YtlReferenceSlotRuntime ownedChecked);
YtlReferenceSlotRuntime* ConstructGuiStaticReferenceAt4D7940(YtlReferenceSlotRuntime& output,
    GuiControlNativeBody50h* body, std::uint8_t owning);
YtlReferenceSlotRuntime* ConstructPopupCheckBoxReferenceAt4EA73C(YtlReferenceSlotRuntime& output,
    GuiCheckBoxNativeBody60h* body);
YtlReferenceSlotRuntime* ConstructGuiCheckBoxReferenceAt4D7710(YtlReferenceSlotRuntime& output,
    GuiCheckBoxNativeBody60h* body, std::uint8_t owning);
void ReleaseGuiControlReferenceAt4F6460(YtlReferenceSlotRuntime& reference) noexcept;
GuiControlNativeBody50h* ResolveGuiControl(const YtlReferenceSlotRuntime& reference) noexcept;
GuiControlNativeBody50h& RequireGuiControl(const YtlReferenceSlotRuntime& reference);
// ownedEvent对应var_B0临时Ref；lateSource对应外层var_50，旧释放回调后只重读其base。
void BindPopupGuiControlEventAt4EA856(GuiControlNativeBody50h& body,
    YtlReferenceSlotRuntime ownedEvent, const YtlReferenceSlotRuntime& lateSource);

// 4182F0只在图形叶调用点取；服务不进入50h/60h原生正文。
class IGuiControlGraphicsSource
{
public:
    virtual ~IGuiControlGraphicsSource() = default;
    virtual IGraphicsSurfaceCompositionApi& CompositionAt4182F0() = 0;
    virtual IGraphicsSurfacePixelApi& PixelsAt4182F0() = 0;
};
class GuiControlGraphicsScope final
{
public:
    explicit GuiControlGraphicsScope(IGuiControlGraphicsSource& source) noexcept;
    explicit GuiControlGraphicsScope(IGuiControlGraphicsSource* source) noexcept;
    ~GuiControlGraphicsScope() noexcept;
    GuiControlGraphicsScope(const GuiControlGraphicsScope&) = delete;
    GuiControlGraphicsScope& operator=(const GuiControlGraphicsScope&) = delete;
private:
    IGuiControlGraphicsSource* previous_;
};
IGuiControlGraphicsSource& RequireGuiControlGraphicsSource();

// 分文件实现供真实表引用，不通过现代Control接口重排虚槽。
namespace GuiControlNativeEntries
{
// 原51E650/5214B0等入口服务两种正文；访问已确认的int字段，不把5Ch强转成60h对象。
template<std::size_t Offset> std::int32_t& ButtonField(GuiControlNativeBody50h& body) noexcept
{
    static_assert(Offset == 0x50 || Offset == 0x54 || Offset == 0x58);
    return *reinterpret_cast<std::int32_t*>(reinterpret_cast<std::byte*>(&body) + Offset);
}
void __fastcall DrawStatic(GuiControlNativeBody50h*, void*, const RECT*);
void __fastcall DrawCheckBox(GuiControlNativeBody50h*, void*, const RECT*);
void __fastcall CompositeStatic(GuiControlNativeBody50h*, void*, YtlReferenceSlotRuntime, const RECT*);
void __fastcall CompositeCheckBox(GuiControlNativeBody50h*, void*, YtlReferenceSlotRuntime, const RECT*);
RECT* __fastcall Rect(GuiControlNativeBody50h*, void*, RECT*);
POINT* __fastcall Position(GuiControlNativeBody50h*, void*, POINT*, std::int32_t, std::int32_t);
YtlReferenceSlotRuntime* __fastcall Resource(GuiControlNativeBody50h*, void*, YtlReferenceSlotRuntime*, std::int32_t);
YtlReferenceSlotRuntime* __fastcall ButtonResource(GuiControlNativeBody50h*, void*, YtlReferenceSlotRuntime*, std::int32_t);
void __fastcall SetHitSlot(GuiControlNativeBody50h*, void*, std::int32_t);
std::int32_t __fastcall HitStatic(GuiControlNativeBody50h*, void*, std::int32_t, std::int32_t);
std::int32_t __fastcall HitCheckBox(GuiControlNativeBody50h*, void*, std::int32_t, std::int32_t);
std::int32_t __fastcall Keyboard(GuiControlNativeBody50h*, void*);
std::int32_t __fastcall Mouse(GuiControlNativeBody50h*, void*);
}
}
