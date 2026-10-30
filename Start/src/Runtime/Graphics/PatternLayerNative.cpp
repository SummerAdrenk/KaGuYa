#include <Runtime/PatternLayerNative.h>
#include <Runtime/GuiDirtyRectNative.h>

#include <bit>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = PatternLayerNativeBody80h;
using Base = LayerItemNativeBody14h;
thread_local IGraphicsWindowSurfaceApi* activeDrawing = nullptr;

IGraphicsWindowSurfaceApi& Drawing()
{
    if (!activeDrawing) throw std::logic_error("Pattern native drawing requires an external Graphics scope");
    return *activeDrawing;
}
GraphicsSurfaceNativeBody& Surface(const YtlReferenceSlotRuntime& slot)
{
    auto* const body = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(slot);
    return *body;
}
Body& Derived(Base* body) noexcept { return *reinterpret_cast<Body*>(body); }
void __fastcall Draw(Base* body, void*, const RECT* clip)
    { DrawPatternLayerNativeAt482C00(Derived(body), 0, 0, clip, Drawing()); }
void __fastcall DrawOffset(Base* body, void*, std::int32_t x, std::int32_t y, const RECT* clip)
    { DrawPatternLayerNativeAt482C00(Derived(body), x, y, clip, Drawing()); }
void __fastcall Composite(Base* body, void*, GraphicsSurfaceNativeBody* destination, const RECT* clip)
    { CompositePatternLayerNativeAt481BD0(Derived(body), destination, clip, Drawing()); }
Base* __fastcall Delete(Base* body, void*, std::uint32_t flags)
    { DeletePatternLayerNativeAt468FC0(Derived(body), flags); return body; }
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const PatternLayerNativeVtable methods{
    {reinterpret_cast<decltype(LayerItemNativeVtable::drawAt00)>(&Draw),
     reinterpret_cast<decltype(LayerItemNativeVtable::compositeAt04)>(&Composite),
     reinterpret_cast<decltype(LayerItemNativeVtable::deletingDestructorAt08)>(&Delete)},
    reinterpret_cast<decltype(PatternLayerNativeVtable::drawOffsetAt0C)>(&DrawOffset)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

struct ConstructionUnwind final
{
    Body& body;
    unsigned stage = 0;
    bool armed = true;
    ~ConstructionUnwind() noexcept
    {
        if (!armed) return;
        if (stage >= 6) ReleaseGuiDirtyRectReferenceAt460500(body.dirtyAt70);
        if (stage >= 5) ReleaseYtlReference(body.processedAt64);
        if (stage >= 4) DestroyPatternFilterNativeAt462FC0(body.filterAt40);
        if (stage >= 3) ReleaseSpriteReferenceAt460260(body.spritesAt34);
        if (stage >= 2) ReleaseYtlReference(body.mainAt18);
        DestroyLayerItemNativeAt441310(body.layer);
    }
};
class PatternControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit PatternControl(Body* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 492CC0：当前正文+08虚槽，不固定为本类静态删除函数。
        auto* const body = static_cast<Body*>(payloadAt14);
        if (body) body->layer.vptrAt00->deletingDestructorAt08(&body->layer, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
        { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(PatternControl) == 0x18);
}

const PatternLayerNativeVtable& PatternLayerNativeMethods() noexcept { return methods; }
Body* ConstructPatternLayerNativeAt45B120(Body& body, LayerListNativeBody18h* parent,
    SIZE size, std::int32_t alpha, std::uint32_t spriteCount,
    IGraphicsSurfaceApi& surfaces, IRuntimeStringManagerSource& strings)
{
    GraphicsSurfaceNativeScope scope(surfaces);
    ConstructLayerItemNativeAt441270(body.layer, parent);
    ConstructionUnwind unwind{body};
    body.layer.vptrAt00 = &methods.base;
    body.itemIndexAt14 = -1;
    auto* const main = NewGraphicsSurfaceAt40C2E0(surfaces, strings);
    ConstructGraphicsSurfaceReferenceAt451500(body.mainAt18, main, 1);
    unwind.stage = 2;
    body.boundsAt24 = {};
    body.spritesAt34 = {};
    unwind.stage = 3;
    (void)ConstructPatternFilterNativeAt45B1CB(&body.filterAt40);
    unwind.stage = 4;
    body.processedAt64 = {};
    unwind.stage = 5;
    ConstructGuiDirtyRectReferenceAt45B227(body.dirtyAt70);
    body.resultAt7D = 0;
    unwind.stage = 6;
    (void)Surface(body.mainAt18).CreateAt415010(surfaces, size.cx, size.cy, alpha, 1);
    body.boundsAt24 = {0, 0, size.cx, size.cy};
    // 45B327的raw数组只保护元素构造，控制块失败时不能追加删除完成数组。
    auto* const sprites = NewSpriteArrayAt45B327(spriteCount, strings);
    InstallSpriteArrayReferenceAt45B361(body.spritesAt34, sprites, std::bit_cast<std::int32_t>(spriteCount));
    // +60/+7C和padding在45B120没有写入；不以“安全初始化”修改原字段。
    unwind.armed = false;
    return &body;
}
void DestroyPatternLayerNativeAt4636E0(Body& body) noexcept
{
    // 原析构入口不恢复Pattern表；成员异常终止，不继续释放后续成员。
    ReleaseGuiDirtyRectReferenceAt460500(body.dirtyAt70);
    ReleaseYtlReference(body.processedAt64);
    DestroyPatternFilterNativeAt462FC0(body.filterAt40);
    ReleaseSpriteReferenceAt460260(body.spritesAt34);
    ReleaseYtlReference(body.mainAt18);
    DestroyLayerItemNativeAt441310(body.layer);
}
Body* DeletePatternLayerNativeAt468FC0(Body& body, std::uint32_t flags) noexcept
{
    DestroyPatternLayerNativeAt4636E0(body);
    auto* const result = &body;
    if (flags & 1u) ::operator delete(result, std::size_t{0x80});
    return result;
}
void ConstructPatternLayerReferenceAt4599B5(YtlReferenceSlotRuntime& out, Body* body)
{
    out = {};
    if (!body) return;
    out.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* const raw = ::operator new(std::size_t{0x18});
    auto* const control = raw ? ::new(raw) PatternControl(body) : nullptr;
    out.controlAt08 = control;
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    out.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->strideAt0C = 0x80;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->countAt10 = 1;
}
void DrawPatternLayerNativeAt482C00(Body& body, std::int32_t x, std::int32_t y,
    const RECT* clip, IGraphicsWindowSurfaceApi& drawing)
{
    const auto& source = Surface(body.processedAt64.baseAt00 ? body.processedAt64 : body.mainAt18);
    const auto handle = source.handleAt04;
    if (handle) (void)drawing.SurfaceDrawRaw(handle, x, y, nullptr, nullptr, clip, 0);
}
void CompositePatternLayerNativeAt481BD0(Body& body, GraphicsSurfaceNativeBody* destination,
    const RECT* clip, IGraphicsWindowSurfaceApi& drawing)
{
    const auto mode = std::bit_cast<std::int32_t>(body.filterAt40.modeAt04);
    const auto* const source = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(
        mode > 0 && mode != 10 ? body.processedAt64 : body.mainAt18);
    const auto target = destination->handleAt04;
    if (!target) return; // 4425E6先读目标；目标零时不能触碰空source。
    const auto handle = source->handleAt04;
    if (handle) (void)drawing.SurfaceBlitRaw(target, handle, 0, 0, nullptr, nullptr, clip, 0);
}
PatternLayerNativeGraphicsScope::PatternLayerNativeGraphicsScope(IGraphicsWindowSurfaceApi& drawing) noexcept
    : previous_(activeDrawing) { activeDrawing = &drawing; }
PatternLayerNativeGraphicsScope::~PatternLayerNativeGraphicsScope() noexcept { activeDrawing = previous_; }
}
