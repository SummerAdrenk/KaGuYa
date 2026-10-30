#include <Runtime/SelectWindowNative.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/GraphicsSurfaceRuntime.h>

#include <bit>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = SelectWindowNativeBody6Ch;
using Base = LayerItemNativeBody14h;
using Slot = YtlReferenceSlotRuntime;
thread_local SelectWindowNativeServices* currentServices;
SelectWindowNativeServices& Services()
{
    if (!currentServices) throw std::logic_error("SelectWindowNativeScope is not installed");
    return *currentServices;
}
template<class T> T Read(const T& field) noexcept { return *static_cast<const volatile T*>(&field); }
GraphicsSurfaceNativeBody* Canvas(const Slot& slot) noexcept
{
    // 482CEF/481C82先解析源Ref，后续4425C0才检查目标handle；不让优化器跨门控后移。
    const auto base = Read(slot.baseAt00);
    if (!base) return nullptr;
    auto* control = Read(slot.controlAt08);
    const auto stride = control ? Read(static_cast<EnvironmentObjectReferenceControl*>(control)->strideAt0C) : 0u;
    return reinterpret_cast<GraphicsSurfaceNativeBody*>(base + Read(slot.biasAt04) * stride);
}
class DialogControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit DialogControl(SelectDialogNativeBody88h* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* body = static_cast<GuiDialogNativeBody5Ch*>(payloadAt14);
        if (body) body->vptrAt00->deletingDestructorAt04(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(DialogControl) == 0x18);
void InstallDialog(Slot& slot, SelectDialogNativeBody88h* body, IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(slot);
    if (!body) return;
    // 45CFEA把旧control接收寄存器清0，45CFF7不额外重读成员control。
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* raw = memory.AllocateBody(0x18);
    auto* control = raw ? ::new(raw) DialogControl(body) : nullptr;
    slot.controlAt08 = control;
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0x88;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}
struct RawLifetime final
{
    IEnvironmentRootMemory& memory;
    void* body;
    std::size_t size;
    bool active = true;
    ~RawLifetime() noexcept { if (active) memory.DeleteBody(body, size); }
};
void __fastcall Draw(Base* base, void*, const RECT* clip)
{
    auto& body = *reinterpret_cast<Body*>(base);
    auto* dialog = ResolveEnvironmentObjectReference<GuiDialogNativeBody5Ch>(body.dialogAt38);
    dialog->vptrAt00->drawAt60(dialog, clip);
    const auto handle = Read(Canvas(body.canvasAt2C)->handleAt04);
    if (handle) (void)Services().drawing.SurfaceDrawRaw(handle, 0, 0, nullptr, nullptr, clip, 0);
}
void __fastcall Composite(Base* base, void*, GraphicsSurfaceNativeBody* target, const RECT* clip)
{
    auto& body = *reinterpret_cast<Body*>(base);
    auto* dialog = ResolveEnvironmentObjectReference<GuiDialogNativeBody5Ch>(body.dialogAt38);
    Slot argument;
    ConstructGraphicsSurfaceReferenceAt451500(argument, target, 0);
    dialog->vptrAt00->compositeAt5C(dialog, argument, clip);
    auto* canvas = Canvas(body.canvasAt2C);
    const auto targetHandle = Read(target->handleAt04);
    if (!targetHandle) return;
    const auto sourceHandle = Read(canvas->handleAt04);
    if (sourceHandle) (void)Services().drawing.SurfaceBlitRaw(targetHandle, sourceHandle, 0, 0, nullptr, nullptr, clip, 0);
}
Base* __fastcall Delete(Base* base, void*, std::uint32_t flags) noexcept
{ return reinterpret_cast<Base*>(DeleteSelectWindowNativeAt4693F0(*reinterpret_cast<Body*>(base), flags)); }
RECT* __fastcall Bounds(Base* base, void*, RECT* output) noexcept
{ *output = reinterpret_cast<Body*>(base)->window.boundsAt14; return output; }
std::uint8_t __fastcall Request(Base* base, void*, std::uint8_t value, std::int32_t)
{
    if ((base->enabledAt0C != 0) == (value != 0)) return 0;
    auto* parent = base->parentAt10;
    base->enabledAt0C = value ? 1 : 0;
    parent->vptrAt00->addDirtyAt20(parent, &reinterpret_cast<Body*>(base)->window.boundsAt14);
    return 1;
}
void __fastcall Advance(Base*, void*) noexcept {} // 46B230，选择流程由4BBFD0另行驱动。
void __fastcall Reset(Base* base, void*) noexcept { base->enabledAt0C = 0; } // 原PE47EE70。
// 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
// 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
// noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const LayerItemNativeFrameVtable methods{
    {reinterpret_cast<decltype(LayerItemNativeVtable::drawAt00)>(&Draw),
     reinterpret_cast<decltype(LayerItemNativeVtable::compositeAt04)>(&Composite),
     reinterpret_cast<decltype(LayerItemNativeVtable::deletingDestructorAt08)>(&Delete)},
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::boundsAt0C)>(&Bounds),
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::requestAt10)>(&Request),
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::advanceAt14)>(&Advance),
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::resetAt18)>(&Reset)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
constexpr char normalName[] = "\x91\x49\x91\xF0\x8E\x88\x95\xB6\x8E\x9A\x90\x46";
constexpr char selectedName[] = "\x8A\xF9\x91\x49\x91\xF0\x95\xB6\x8E\x9A\x90\x46";
constexpr char shadowName[] = "\x91\x49\x91\xF0\x8E\x88\x8F\xFC\x82\xE8\x90\x46";
}
SelectWindowNativeScope::SelectWindowNativeScope(SelectWindowNativeServices& services) noexcept
    : previous_(currentServices), surfaces_(services.surfaces), layers_(services.memory), dialog_(services.profile, services.menu)
{ currentServices = &services; }
SelectWindowNativeScope::~SelectWindowNativeScope() noexcept { currentServices = previous_; }
const LayerItemNativeFrameVtable& SelectWindowNativeMethods() noexcept { return methods; }
Body* ConstructSelectWindowNativeAt45CC30(Body& body, LayerListNativeBody18h* parent, SelectWindowNativeServices& services)
{
    SelectWindowNativeScope scope(services);
    ConstructLayerItemNativeAt441270(body.window.layer, parent);
    body.window.boundsAt14 = {};
    body.window.layer.vptrAt00 = &methods.base;
    body.canvasAt2C = {};
    body.dialogAt38 = {};
    body.normalColorAt48 = {}; body.selectedColorAt54 = {}; body.shadowColorAt60 = {};
    try
    {
        TryInitializeWindowNativeColor(body.normalColorAt48, normalName, services.colors, services.memory);
        TryInitializeWindowNativeColor(body.selectedColorAt54, selectedName, services.colors, services.memory);
        TryInitializeWindowNativeColor(body.shadowColorAt60, shadowName, services.colors, services.memory);
        body.window.layer.enabledAt0C = 0;
        void* raw = services.memory.AllocateBody(0x88);
        RawLifetime dialogRaw{services.memory, raw, 0x88};
        auto* dialog = raw ? ConstructSelectDialogNativeAt4F17E0(*::new(raw) SelectDialogNativeBody88h,
            services.profile, services.resources, services.surfaces, services.strings) : nullptr;
        dialogRaw.active = false;
        InstallDialog(body.dialogAt38, dialog, services.memory);
        raw = services.memory.AllocateBody(0x24);
        RawLifetime surfaceRaw{services.memory, raw, 0x24};
        auto* surface = raw ? ::new(raw) GraphicsSurfaceNativeBody(services.surfaces, services.strings) : nullptr;
        surfaceRaw.active = false;
        AdoptGraphicsSurfaceAt407A80(body.canvasAt2C, surface);
        const auto height = std::bit_cast<std::int32_t>(Read(services.environment.CaptureAt4180C0()->heightAt1C));
        auto* environment = services.environment.CaptureAt4180C0();
        const auto base = Read(body.canvasAt2C.baseAt00);
        const auto width = std::bit_cast<std::int32_t>(Read(environment->widthAt18));
        GraphicsSurfaceNativeBody* canvas = nullptr;
        if (base)
        {
            auto* control = Read(body.canvasAt2C.controlAt08);
            const auto stride = control ? Read(static_cast<EnvironmentObjectReferenceControl*>(control)->strideAt0C) : 0u;
            canvas = reinterpret_cast<GraphicsSurfaceNativeBody*>(base + Read(body.canvasAt2C.biasAt04) * stride);
        }
        (void)canvas->CreateAt415010(services.surfaces, width, height, 1, 1);
        const auto boundsHeight = std::bit_cast<std::int32_t>(Read(services.environment.CaptureAt4180C0()->heightAt1C));
        const auto boundsWidth = std::bit_cast<std::int32_t>(Read(services.environment.CaptureAt4180C0()->widthAt18));
        body.window.boundsAt14 = {0, 0, boundsWidth, boundsHeight};
    }
    catch (...) { body.window.boundsAt14 = {}; }
    return &body;
}
void DestroySelectWindowNativeAt463CF0(Body& body) noexcept
{
    ReleaseYtlReference(body.shadowColorAt60);
    ReleaseYtlReference(body.selectedColorAt54);
    ReleaseYtlReference(body.normalColorAt48);
    ReleaseYtlReference(body.dialogAt38);
    ReleaseYtlReference(body.canvasAt2C);
    DestroyLayerWindowNativeAt464910(body.window);
}
Body* DeleteSelectWindowNativeAt4693F0(Body& body, std::uint32_t flags) noexcept
{
    DestroySelectWindowNativeAt463CF0(body);
    if (flags & 1U) ::operator delete(&body, std::size_t{0x6C});
    return &body;
}
}
