#include <Runtime/TextWindowLayerNative.h>
#include <Runtime/TextWindowNativeColors.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/LayerWindowNativeBase.h>
#include <Runtime/MessageWindowNativeConfig.h>
#include <Runtime/ParamsGameSystemRuntime.h>

#include <bit>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = TextWindowLayerNativeBodyE8h;
using Base = LayerItemNativeBody14h;
using Slot = YtlReferenceSlotRuntime;
using Table = TextWindowLayerNativeVtable;
thread_local TextWindowLayerNativeServices* currentServices = nullptr;

TextWindowLayerNativeServices& Services()
{
    if (!currentServices) throw std::logic_error("CTextWin native scope is absent");
    return *currentServices;
}
Body& Derived(Base* body) noexcept { return *reinterpret_cast<Body*>(body); }

template<class T>
T Read(const T& field) noexcept { return *static_cast<const volatile T*>(&field); }

template<bool LateBase = false>
GraphicsSurfaceNativeBody* ResolveCanvas(const Body& body) noexcept
{
    // 482D4B/46D1B1固定首次base；481CEB/46DC48先判base，读stride/bias后再加当前base。
    // 保留这些inline读取，避免优化把坏control的访问移到4425C0目标handle门控之后。
    const auto base = Read(body.canvasAt24.baseAt00);
    if (!base) return nullptr;
    const auto* const control = Read(body.canvasAt24.controlAt08);
    const auto stride = control ? Read(static_cast<const EnvironmentObjectReferenceControl*>(control)->strideAt0C) : 0u;
    const auto bias = Read(body.canvasAt24.biasAt04);
    const auto finalBase = LateBase ? Read(body.canvasAt24.baseAt00) : base;
    return reinterpret_cast<GraphicsSurfaceNativeBody*>(finalBase + bias * stride);
}

// 482D33/46D196等是四DWORD非零测试，顺序30/38/34/3C；不作IsRectEmpty或enabled门控。
bool HasContent(const Body& body) noexcept
{
    return Read(body.contentAt30.left) != 0 || Read(body.contentAt30.right) != 0 ||
        Read(body.contentAt30.top) != 0 || Read(body.contentAt30.bottom) != 0;
}
void DirtyContent(Body& body)
{
    auto* const parent = body.layer.parentAt10;
    parent->vptrAt00->addDirtyAt20(parent, &body.contentAt30);
}
void ClearRectBottomFirst(RECT& rect) noexcept
{
    // 46DC10/46D1F3/4C2978的提交顺序；418EE0的content.left另在此前写。
    volatile RECT& ordered = rect;
    ordered.bottom = 0;
    ordered.top = 0;
    ordered.right = 0;
    ordered.left = 0;
}
void ClearRects(Body& body) noexcept
{
    ClearRectBottomFirst(body.contentAt30);
    ClearRectBottomFirst(body.nameBoundsAt40);
}
void ClearOffsets(Body& body) noexcept
{
    // 45E141/46D22A只清四个offset，不触及D0/D4及播放状态字节。
    volatile Body& ordered = body;
    ordered.nameOffsetYAtE4 = 0;
    ordered.nameOffsetXAtE0 = 0;
    ordered.bodyOffsetYAtDC = 0;
    ordered.bodyOffsetXAtD8 = 0;
}
template<bool LateBase = false>
void ClearSurface(Body& body, const RECT* rect, TextWindowLayerNativeServices& services)
{
    // 4143A0/46DC69/46D1D2：固定Surface，再在4182F0之前固定handle/color。
    // 与46F745的先Capture后读正文不同；只能借值参数代理，不能调用body-aware Clear。
    auto* const surface = ResolveCanvas<LateBase>(body);
    const auto handle = Read(surface->handleAt04);
    if (!handle) return;
    const auto color = Read(surface->clearColorAt20);
    services.drawing.SurfaceClearRaw(handle, color, rect);
}
void ClearAndDirtyOldContent(Body& body, TextWindowLayerNativeServices& services)
{
    if (!HasContent(body)) return;
    ClearSurface(body, &body.contentAt30, services);
    // Clear的叶/解析/Free回调可改parent或rect；此处才取当前parent，传原字段地址。
    DirtyContent(body);
}

class BodyAllocationUnwind final
{
public:
    BodyAllocationUnwind(IEnvironmentRootMemory& memory, void* body, std::size_t size) noexcept
        : memory_(memory), body_(body), size_(size) {}
    // 596D81(raw24)/59AC11(rawE8)发生于展开期，释放异常终止，不挪到business catch。
    ~BodyAllocationUnwind() noexcept { if (active_) memory_.DeleteBody(body_, size_); }
    void Disarm() noexcept { active_ = false; }
private:
    IEnvironmentRootMemory& memory_;
    void* body_;
    std::size_t size_;
    bool active_ = true;
};

void DestroyReferencesAt461AC0(DrawLayerReferenceArray& references) noexcept
{
    // 461AC0与461750同核：begin门控、固定range释放、回调后读cap/begin、
    // signed距离/12再乘12、1000h阈值和raw[-1]/+23h/1Fh检查、delete后清三指针。
    // 48F980与452440逐槽指令相同，仅ret8/cdecl清栈不同；不依赖未知payload类型。
    DestroyDrawLayerReferencesAt461750(references);
}

void __fastcall Draw(Base* body, void*, const RECT* clip)
    { DrawTextWindowLayerNativeAt482D30(Derived(body), clip, Services()); }
void __fastcall Composite(Base* body, void*, GraphicsSurfaceNativeBody* target, const RECT* clip)
    { CompositeTextWindowLayerNativeAt481CD0(Derived(body), target, clip, Services()); }
Base* __fastcall Delete(Base* body, void*, std::uint32_t flags)
    { DeleteTextWindowLayerNativeAt469590(Derived(body), flags); return body; }
RECT* __fastcall Bounds(Base* body, void*, RECT* out)
    { *out = Derived(body).boundsAt14; return out; } // 479DB0：隐藏RECT*输出，ret4。
std::uint8_t __fastcall Request(Base* body, void*, std::uint8_t value, std::int32_t unused)
    { return RequestTextWindowLayerNativeAt487310(Derived(body), value, unused, Services()); }
void __fastcall Advance(Base*, void*) {} // 46B230：只有RET，不接B/C文字执行。
void __fastcall Reset(Base* body, void*)
    { ResetTextWindowLayerNativeAt47EE80(Derived(body), Services()); }
void __fastcall Clear(Base* body, void*)
    { ClearTextWindowLayerNativeAt46DC10(Derived(body), Services()); }

// PE 5B2C90的八槽顺序；使用现有x86 fastcall桥保持ECX及原ret4/ret8约定。
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const Table methods{
    {{reinterpret_cast<decltype(LayerItemNativeVtable::drawAt00)>(&Draw),
      reinterpret_cast<decltype(LayerItemNativeVtable::compositeAt04)>(&Composite),
      reinterpret_cast<decltype(LayerItemNativeVtable::deletingDestructorAt08)>(&Delete)},
     reinterpret_cast<decltype(LayerItemNativeFrameVtable::boundsAt0C)>(&Bounds),
     reinterpret_cast<decltype(LayerItemNativeFrameVtable::requestAt10)>(&Request),
     reinterpret_cast<decltype(LayerItemNativeFrameVtable::advanceAt14)>(&Advance),
     reinterpret_cast<decltype(LayerItemNativeFrameVtable::resetAt18)>(&Reset)},
    reinterpret_cast<decltype(Table::clearAt1C)>(&Clear)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}

TextWindowLayerNativeScope::TextWindowLayerNativeScope(TextWindowLayerNativeServices& services) noexcept
    : previous_(currentServices), surfaces_(services.surfaces) { currentServices = &services; }
TextWindowLayerNativeScope::~TextWindowLayerNativeScope() noexcept { currentServices = previous_; }
const TextWindowLayerNativeVtable& TextWindowLayerNativeMethods() noexcept { return methods; }

Body* ConstructTextWindowLayerNativeAt45DB30(Body& body, LayerListNativeBody18h* parent,
    TextWindowLayerNativeServices& services)
{
    TextWindowLayerNativeScope scope(services);
    // 45DB60..DCF5：注册先于derived初始化，注册抛出不进入本体business catch。
    ConstructLayerItemNativeAt441270(body.layer, parent);
    body.boundsAt14 = {};
    body.layer.vptrAt00 = methods.AsBase();
    body.canvasAt24 = {};
    body.contentAt30 = {};
    body.nameBoundsAt40 = {};
    body.referencesAt64 = {};
    body.nameAt70 = {};
    body.voicesAt7C = {};
    body.bodyAt88 = {};
    body.normalBodyColorAt9C = {};
    body.readBodyColorAtA8 = {};
    body.nameColorAtB4 = {};
    body.nameShadowColorAtC0 = {};
    body.unknownAtD0 = 0;
    body.unknownAtD4 = 0;
    body.bodyOffsetXAtD8 = 0;
    body.bodyOffsetYAtDC = 0;
    body.nameOffsetXAtE0 = 0;
    body.nameOffsetYAtE4 = 0;
    try
    {
    InitializeTextWindowNativeColorsAt45DCF5(body, services.colors, services.memory);
        // 45E067先禁用，再申请真实24h；构造完成才撤raw守卫，407A80失败不补删。
        body.layer.enabledAt0C = 0;
        void* const raw = services.memory.AllocateBody(0x24);
        BodyAllocationUnwind rawUnwind(services.memory, raw, 0x24);
        auto* const surface = raw ? ::new (raw) GraphicsSurfaceNativeBody(
            services.surfaces, services.strings) : nullptr;
        rawUnwind.Disarm();
        AdoptGraphicsSurfaceAt407A80(body.canvasAt24, surface);

        // 45E096..D8：两次独立4180C0，height先；第二次之后先读canvas.base再读width，
        // 然后才读control/stride/bias。Create的AL不决定后续bounds是否提交。
        const auto height = std::bit_cast<std::int32_t>(Read(services.environment.CaptureAt4180C0()->heightAt1C));
        auto* const program = services.environment.CaptureAt4180C0();
        const auto base = Read(body.canvasAt24.baseAt00);
        const auto width = std::bit_cast<std::int32_t>(Read(program->widthAt18));
        GraphicsSurfaceNativeBody* canvas = nullptr;
        if (base)
        {
            const auto* control = Read(body.canvasAt24.controlAt08);
            const auto stride = control ? Read(static_cast<const EnvironmentObjectReferenceControl*>(control)->strideAt0C) : 0u;
            canvas = reinterpret_cast<GraphicsSurfaceNativeBody*>(base + Read(body.canvasAt24.biasAt04) * stride);
        }
        static_cast<void>(canvas->CreateAt415010(services.surfaces, width, height, 1, 1));

        // 45E0DD..169再各捕获一次height/width，按原顺序发布bounds及尾字段。
        const auto boundsHeight = std::bit_cast<std::int32_t>(Read(services.environment.CaptureAt4180C0()->heightAt1C));
        const auto boundsWidth = std::bit_cast<std::int32_t>(Read(services.environment.CaptureAt4180C0()->widthAt18));
        volatile RECT& content = body.contentAt30;
        content.bottom = 0;
        content.top = 0;
        body.boundsAt14 = {0, 0, boundsWidth, boundsHeight};
        content.right = 0;
        content.left = 0;
        ClearRectBottomFirst(body.nameBoundsAt40);
        ClearOffsets(body);
        body.glyphCursorAt50 = -1;
    }
    catch (...)
    {
        // 45E172只清bounds：保留注册、四颜色、canvas、enabled和此前提交前缀。
        body.boundsAt14 = {};
    }
    return &body;
}

void DestroyTextWindowLayerNativeAt464510(Body& body) noexcept
{
    // 普通析构入口不写CTextWin vptr；5EB044五个尾state及4100C0均为终止边界。
    ReleaseYtlReference(body.nameShadowColorAtC0);
    ReleaseYtlReference(body.nameColorAtB4);
    ReleaseYtlReference(body.readBodyColorAtA8);
    ReleaseYtlReference(body.normalBodyColorAt9C);
    // 4100C0(+70)是逆序三Ref，不同于46D170的name/body/voices顺序。
    ReleaseYtlReference(body.bodyAt88);
    ReleaseYtlReference(body.voicesAt7C);
    ReleaseYtlReference(body.nameAt70);
    DestroyReferencesAt461AC0(body.referencesAt64);
    ReleaseYtlReference(body.canvasAt24);
    body.layer.vptrAt00 = LayerWindowNativeBaseMethods().AsBase();
    DestroyLayerItemNativeAt441310(body.layer);
}
Body* DeleteTextWindowLayerNativeAt469590(Body& body, std::uint32_t flags) noexcept
{
    // 469590只解释bit0，不把bit1扩成数组删除，也不保存现代owner。
    DestroyTextWindowLayerNativeAt464510(body);
    auto* const result = &body;
    if (flags & 1u) ::operator delete(result, std::size_t{0xE8});
    return result;
}
void DrawTextWindowLayerNativeAt482D30(Body& body, const RECT* clip,
    TextWindowLayerNativeServices& services)
{
    TextWindowLayerNativeScope scope(services);
    if (!HasContent(body)) return;
    auto* const canvas = ResolveCanvas(body);
    // 441980仅按当前handle门控；目标位置0/0，size/source为空，clip原指针，anchor0。
    const auto handle = Read(canvas->handleAt04);
    if (handle) static_cast<void>(services.drawing.SurfaceDrawRaw(handle, 0, 0, nullptr, nullptr, clip, 0));
}
void CompositeTextWindowLayerNativeAt481CD0(Body& body, GraphicsSurfaceNativeBody* target,
    const RECT* clip, TextWindowLayerNativeServices& services)
{
    TextWindowLayerNativeScope scope(services);
    if (!HasContent(body)) return;
    // 481CEB先解析source Ref；4425C0随后先门控target.handle，再访问source.handle。
    auto* const source = ResolveCanvas<true>(body);
    const auto destinationHandle = Read(target->handleAt04);
    if (!destinationHandle) return;
    const auto sourceHandle = Read(source->handleAt04);
    if (sourceHandle) static_cast<void>(services.drawing.SurfaceBlitRaw(
        destinationHandle, sourceHandle, 0, 0, nullptr, nullptr, clip, 0));
}
std::uint8_t RequestTextWindowLayerNativeAt487310(Body& body, std::uint8_t requested,
    std::int32_t, TextWindowLayerNativeServices& services)
{
    TextWindowLayerNativeScope scope(services);
    // 即便requested=0也先走Params和允许门控；两次413BE0分别取根，不固定旧manager。
    auto& state = services.state;
    const auto first = state.CaptureAfterEnsureAt413BE0();
    if (first.paramsAt47BDB0(first.object)->messageFormatIndexAt60 == -1)
        requested = 0;
    else
    {
        const auto gate = state.CaptureAfterEnsureAt413BE0();
        if (!state.MessageWindowAllowedAt419290(gate.drawManagerAt4175A0(gate.object))) requested = 0;
    }
    if (requested)
    {
        if (body.layer.enabledAt0C) return 0;
        body.layer.enabledAt0C = 1;
    }
    else
    {
        if (!body.layer.enabledAt0C) return 0;
        body.layer.enabledAt0C = 0;
    }
    // enabled先提交，dirty可抛；有变化也始终AL=0，不沿用CMesWin的返回值。
    if (HasContent(body)) DirtyContent(body);
    return 0;
}
void ClearTextWindowLayerNativeAt46DC10(Body& body, TextWindowLayerNativeServices& services)
{
    TextWindowLayerNativeScope scope(services);
    // 46DC10先清两RECT，之后全画布Clear(nullptr)；失败不还原RECT，不额外dirty。
    ClearRects(body);
    ClearSurface<true>(body, nullptr, services);
}
void ResetTextWindowLayerNativeAt47EE80(Body& body, TextWindowLayerNativeServices& services)
{
    TextWindowLayerNativeScope scope(services);
    // 47EE83必须读取当前vptr+1C；动态clear返回之后才禁用，不静态直调46DC10。
    const auto* const table = reinterpret_cast<const Table*>(body.layer.vptrAt00);
    table->clearAt1C(&body.layer);
    body.layer.enabledAt0C = 0;
}
void ResetTextWindowNativeAt46D170(Body& body, TextWindowLayerNativeServices& services)
{
    TextWindowLayerNativeScope scope(services);
    ClearAndDirtyOldContent(body, services);
    ClearRects(body);
    ClearOffsets(body);
    body.glyphCursorAt50 = -1;
    // 46D259先取end再begin；48F980固定范围且整体终止。成功后才晚读begin写end。
    auto* const end = Read(body.referencesAt64.end);
    auto* const begin = Read(body.referencesAt64.begin);
    ReleaseDrawLayerRangeAt452440(begin, end);
    body.referencesAt64.end = body.referencesAt64.begin;
    // 46D274..329三个普通尾允许异常外传；不把整个Reset声明成noexcept。
    ReleaseYtlReference(body.nameAt70);
    ReleaseYtlReference(body.bodyAt88);
    ReleaseYtlReference(body.voicesAt7C);
}
void ClearTextWindowPreviousRegionNativeAt418EE0(Body& body, TextWindowLayerNativeServices& services)
{
    TextWindowLayerNativeScope scope(services);
    ClearAndDirtyOldContent(body, services);
    // 418F2F先清left，接着3C/34/38；不能与46D170共用同一个清零次序。
    volatile RECT& content = body.contentAt30;
    content.left = 0;
    content.bottom = 0;
    content.top = 0;
    content.right = 0;
    ClearRectBottomFirst(body.nameBoundsAt40);
}
void ClearTextWindowFormatRegionNativeAt4C291B(Body& body, TextWindowLayerNativeServices& services)
{
    TextWindowLayerNativeScope scope(services);
    // 仅4C291B..4C29A8旧区片段；54/50此前已写，initialize DWORD及415150属于后续B组。
    ClearAndDirtyOldContent(body, services);
    ClearRects(body);
}
Body* CreateAndAppendTextWindowLayerNativeAt48A873(IDrawManagerNativeRootSource& source,
    TextWindowLayerNativeServices& services)
{
    TextWindowLayerNativeScope scope(services);
    // raw申请先于413BE0；state10在构造成功后撤回，后续append失败不能补删完成正文。
    void* const raw = services.memory.AllocateBody(0xE8);
    BodyAllocationUnwind rawUnwind(services.memory, raw, 0xE8);
    Body* created = nullptr;
    if (raw)
    {
        auto* const manager = source.CaptureDrawManagerAfter413BE0();
        auto* const parent = ResolveDrawLayerListAt47AE80(*manager);
        created = ConstructTextWindowLayerNativeAt45DB30(*::new (raw) Body, parent, services);
    }
    rawUnwind.Disarm();
    // 48A8C0另一次捕获决定拥有数组，构造时parent不随根切换；空new也提交空Ref槽。
    auto* const manager = source.CaptureDrawManagerAfter413BE0();
    AppendOwnedDrawLayerAt46A880(manager->ownedLayersAt14, created ? &created->layer : nullptr, services.memory);
    return created;
}
}
