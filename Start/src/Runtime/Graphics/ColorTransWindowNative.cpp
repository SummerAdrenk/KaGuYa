#include <Runtime/ColorTransWindowNative.h>
#include <Runtime/GraphicsSurfaceNativeApi.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/GuiControlResourceNative.h>

#include <bit>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = ColorTransWindowNativeBody38h;
using Surface = GraphicsSurfaceNativeBody;

Body* __fastcall DeleteBody(Body* body, void*, std::uint32_t flags)
{
    return DeleteColorTransWindowNativeAt469730(*body, flags);
}
const ColorTransWindowNativeVtable methods{
    reinterpret_cast<decltype(ColorTransWindowNativeVtable::deletingDestructorAt00)>(&DeleteBody)};

class Control final : public EnvironmentObjectReferenceControl
{
public:
    explicit Control(Body* payload) noexcept
        : EnvironmentObjectReferenceControl(payload, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 492DB0固定payload，仍经正文当前单槽表分派。
        auto* body = static_cast<Body*>(payloadAt14);
        if (body) body->vptrAt00->deletingDestructorAt00(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
        { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(Control) == 0x18);

std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}
std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b));
}

void ReleaseMembers(Body& body) noexcept
{
    ReleaseYtlReference(body.processedAt1C);
    ReleaseYtlReference(body.source2At10);
    ReleaseYtlReference(body.source1At04);
}
struct MemberUnwind final
{
    Body& body;
    bool active = true;
    ~MemberUnwind() noexcept { if (active) ReleaseMembers(body); }
};

bool Load(YtlReferenceSlotRuntime& slot, const char* name, ColorTransWindowNativeServices& services)
{
    // 45EA20/45EAEA的receiver先取；480000自身仅测base，不能提前拒绝解析出的null。
    void* const receiver = services.resources.CaptureLoadReceiverAt413BE0();
    if (!slot.baseAt00)
        AdoptGraphicsSurfaceAt407A80(slot, NewGraphicsSurfaceAt40C2E0(services.surfaces, services.strings));
    return services.resources.LoadIntoAt4800A0(receiver, *ResolveEnvironmentObjectReference<Surface>(slot), name);
}

void UnionSurface(RECT& bounds, const YtlReferenceSlotRuntime& slot)
{
    const auto* size = ResolveEnvironmentObjectReference<Surface>(slot);
    const auto width = size->widthAt08;
    const auto height = size->heightAt0C;
    const auto* origin = ResolveEnvironmentObjectReference<Surface>(slot);
    const RECT rect{origin->originXAt10, origin->originYAt14,
        Add(origin->originXAt10, width), Add(origin->originYAt14, height)};
    ::UnionRect(&bounds, &bounds, &rect);
}

void Blit(Surface* destination, Surface* source, std::int32_t x, std::int32_t y,
    IGraphicsWindowSurfaceApi& drawing)
{
    const auto target = destination->handleAt04;
    if (!target) return;
    const auto handle = source->handleAt04;
    if (handle) (void)drawing.SurfaceBlitRaw(target, handle, x, y, nullptr, nullptr, nullptr, 0);
}
}

const ColorTransWindowNativeVtable& ColorTransWindowNativeMethods() noexcept { return methods; }

ColorTransWindowNativeClear::ColorTransWindowNativeClear(GraphicsSurfaceNativeApi& graphics) noexcept
    : graphics_(graphics) {}

void ColorTransWindowNativeClear::ClearAfterCaptureAt46F745(Surface& fixedBody)
{
    graphics_.ClearSurfaceBodyAt46F745(fixedBody);
}

Surface* ResolveColorTransSource1NativeAt47ADC0(const Body& body) noexcept
    { return ResolveEnvironmentObjectReference<Surface>(body.source1At04); }
Surface* ResolveColorTransSource2NativeAt47C770(const Body& body) noexcept
    { return ResolveEnvironmentObjectReference<Surface>(body.source2At10); }
Surface* ResolveColorTransProcessedNative(const Body& body) noexcept
    { return ResolveEnvironmentObjectReference<Surface>(body.processedAt1C); }

Body* ConstructColorTransWindowNativeAt45E970(Body& body, const char* firstName,
    const char* secondName, std::int32_t mode, ColorTransWindowNativeServices& services)
{
    GraphicsSurfaceNativeScope scope(services.surfaces);
    body.vptrAt00 = &methods;
    body.source1At04 = {};
    body.source2At10 = {};
    body.processedAt1C = {};
    body.modeAt2C = mode;
    MemberUnwind unwind{body};
    try
    {
        RECT bounds{};
        if (firstName)
        {
            if (!Load(body.source1At04, firstName, services)) throw 1;
            UnionSurface(bounds, body.source1At04);
        }
        if (secondName)
        {
            if (!Load(body.source2At10, secondName, services)) throw 2;
            UnionSurface(bounds, body.source2At10);
        }
        if (::IsRectEmpty(&bounds)) throw 3;
        AdoptGraphicsSurfaceAt407A80(body.processedAt1C,
            NewGraphicsSurfaceAt40C2E0(services.surfaces, services.strings));
        if (!ResolveColorTransProcessedNative(body)->CreateAt415010(services.surfaces,
            Sub(bounds.right, bounds.left), Sub(bounds.bottom, bounds.top), 1, 1)) throw 3;
        auto* const processed = ResolveColorTransProcessedNative(body);
        processed->originXAt10 = bounds.left;
        processed->originYAt14 = bounds.top;
        if (body.source1At04.baseAt00)
        {
            const auto* old = ResolveColorTransSource1NativeAt47ADC0(body);
            const auto x = Sub(old->originXAt10, bounds.left);
            const auto y = Sub(old->originYAt14, bounds.top);
            auto* const current = ResolveColorTransSource1NativeAt47ADC0(body);
            current->originYAt14 = y;
            current->originXAt10 = x;
        }
        if (body.source2At10.baseAt00)
        {
            const auto* old = ResolveColorTransSource2NativeAt47C770(body);
            const auto x = Sub(old->originXAt10, bounds.left);
            const auto y = Sub(old->originYAt14, bounds.top);
            auto* const current = ResolveColorTransSource2NativeAt47C770(body);
            current->originXAt10 = x;
            current->originYAt14 = y;
        }
    }
    catch (...)
    {
        // 45ECA7自身的释放若抛，成员EH才逆序接手；不得提前撤掉unwind。
        ClearColorTransWindowNativeAt486A30(body);
    }
    unwind.active = false;
    return &body;
}

Body* NewColorTransWindowNativeAt45F384(const char* firstName, const char* secondName,
    std::int32_t mode, ColorTransWindowNativeServices& services)
{
    void* raw = ::operator new(0x38);
    try
    {
        return raw ? ConstructColorTransWindowNativeAt45E970(*::new(raw) Body,
            firstName, secondName, mode, services) : nullptr;
    }
    catch (...)
    {
        struct RawUnwind final
        {
            void* raw;
            ~RawUnwind() noexcept { ::operator delete(raw, std::size_t{0x38}); }
        } unwind{raw};
        throw;
    }
}

void AdoptColorTransWindowNativeAt45F3B7(YtlReferenceSlotRuntime& slot, Body* created)
{
    ReleaseYtlReference(slot);
    if (!created) return;
    if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(created);
    void* raw = ::operator new(0x18);
    auto* control = raw ? ::new(raw) Control(created) : nullptr;
    slot.controlAt08 = control;
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0x38;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}

void ClearColorTransWindowNativeAt486A30(Body& body)
{
    ReleaseYtlReference(body.source1At04);
    ReleaseYtlReference(body.source2At10);
    ReleaseYtlReference(body.processedAt1C);
}
void DestroyColorTransWindowNativeAt469730(Body& body) noexcept
{
    body.vptrAt00 = &methods;
    ClearColorTransWindowNativeAt486A30(body);
    ReleaseMembers(body);
}
Body* DeleteColorTransWindowNativeAt469730(Body& body, std::uint32_t flags) noexcept
{
    DestroyColorTransWindowNativeAt469730(body);
    auto* result = &body;
    if (flags & 1u) ::operator delete(result, std::size_t{0x38});
    return result;
}

std::uint8_t ApplyColorTransWindowNativeAt46F6D0(Body& body, std::uint32_t color,
    std::int32_t transparency, ColorTransWindowNativeServices& services)
{
    GraphicsSurfaceNativeScope scope(services.surfaces);
    try
    {
        if (!body.processedAt1C.baseAt00) throw 1;
        const auto clamped = transparency < 0 ? 0 : transparency > 255 ? 255 : transparency;
        auto* const fixed = ResolveColorTransProcessedNative(body);
        if (fixed->handleAt04) services.clear.ClearAfterCaptureAt46F745(*fixed);
        if (body.source2At10.baseAt00)
        {
            const auto* initial = ResolveColorTransSource2NativeAt47C770(body);
            const auto x = initial->originXAt10;
            const auto y = initial->originYAt14;
            const auto width = initial->widthAt08;
            const auto height = initial->heightAt0C;
            auto* source = services.drawing.GetSurfacePointerRaw(
                ResolveColorTransSource2NativeAt47C770(body)->handleAt04);
            const auto sourcePitch = services.drawing.GetSurfacePitchRaw(
                ResolveColorTransSource2NativeAt47C770(body)->handleAt04);
            auto* destination = services.drawing.GetSurfacePointerRaw(
                ResolveColorTransProcessedNative(body)->handleAt04);
            const auto destinationPitch = services.drawing.GetSurfacePitchRaw(
                ResolveColorTransProcessedNative(body)->handleAt04);
            const bool recolor = body.modeAt2C != 0;
            const auto rgb = ((color & 0xFFu) << 16u) | (color & 0xFF00u) | ((color >> 16u) & 0xFFu);
            if (height > 0)
            {
                auto sourceRow = reinterpret_cast<std::uintptr_t>(source);
                auto targetRow = reinterpret_cast<std::uintptr_t>(destination) +
                    static_cast<std::uint32_t>(y) * static_cast<std::uint32_t>(destinationPitch) +
                    static_cast<std::uint32_t>(x) * 4u;
                for (std::int32_t row = 0; row < height; ++row)
                {
                    for (std::int32_t column = 0; column < width; ++column)
                    {
                        const auto offset = static_cast<std::uint32_t>(column) * 4u;
                        const auto pixel = *reinterpret_cast<const std::uint32_t*>(sourceRow + offset);
                        const auto alpha = pixel >> 24u;
                        if (!alpha) continue;
                        const auto nextAlpha = alpha > static_cast<std::uint32_t>(clamped) ?
                            alpha - static_cast<std::uint32_t>(clamped) : 0u;
                        *reinterpret_cast<std::uint32_t*>(targetRow + offset) =
                            (nextAlpha << 24u) | (recolor ? rgb : pixel & 0xFFFFFFu);
                    }
                    sourceRow += static_cast<std::uint32_t>(sourcePitch);
                    targetRow += static_cast<std::uint32_t>(destinationPitch);
                }
            }
        }
        if (body.source1At04.baseAt00)
        {
            auto* source = ResolveColorTransSource1NativeAt47ADC0(body);
            const auto x = source->originXAt10;
            const auto y = source->originYAt14;
            source = ResolveColorTransSource1NativeAt47ADC0(body);
            auto* const target = ResolveColorTransProcessedNative(body);
            Blit(target, source, x, y, services.drawing);
        }
        body.lastColorAt30 = color;
        body.lastTransparencyAt34 = clamped;
        return 1;
    }
    catch (...) { return 0; }
}
}
