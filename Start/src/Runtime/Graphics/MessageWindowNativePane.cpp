#include <Runtime/MessageWindowNativePane.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/DrawLayerReferenceArray.h>
#include <Runtime/GraphicsSurfaceRuntime.h>

#include <bit>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
using Pane = MessageWindowPaneNativeBody14h;
using Normal = MessageWindowNormalPaneNativeBody2Ch;
using Variable = MessageWindowVariablePaneNativeBody3Ch;
using Edge = MessageWindowEdgeNativeBody10h;
using Style = MessageWindowStyleNativeBody34h;
using Services = MessageWindowNativePaneServices;
thread_local Services* currentServices = nullptr;

Services& Current()
{
    if (!currentServices) throw std::logic_error("MessageWindow native pane services are not bound");
    return *currentServices;
}
std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); }
std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b)); }
std::int32_t Mul(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) * static_cast<std::uint32_t>(b)); }
template<class T> T* Resolve(const Slot& slot) noexcept
{ return ResolveEnvironmentObjectReference<T>(slot); }
void Initialize(Slot& slot) noexcept { ::new (static_cast<void*>(&slot)) Slot{}; }
Slot CopyParameter(const Slot& source) noexcept
{
    Slot value{source.baseAt00, source.biasAt04, source.controlAt08};
    if (value.controlAt08) ++value.controlAt08->referenceCountAt04;
    return value;
}
void PublishConfigMember(Slot& destination, const Slot& source) noexcept
{
    // 45F36F..45F381/45F28A..45F299：先发布三字段，再增持；只读control的AV须看到成员已写。
    // volatile约束该失败前缀的访问顺序，不把原非原子引用计数改成同步协议。
    volatile auto& member = destination;
    member.baseAt00 = source.baseAt00;
    member.biasAt04 = source.biasAt04;
    auto* const control = source.controlAt08;
    member.controlAt08 = control;
    if (control)
    {
        volatile auto& references = control->referenceCountAt04;
        references = references + 1u;
    }
}
void ReleaseParameter(Slot& slot) noexcept
{
    auto* control = slot.controlAt08;
    if (!control || --control->referenceCountAt04 != 0) return;
    if (control->owningAt08) control->DestroyPayloadAtVslot00();
    if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
}
void ReleaseCached(Slot& slot, YtlReferenceControlRuntime* control) noexcept
{
    // 45FAE5/45FB16：普通尾固定EDI/ESI，payload后不重读槽；异常直接终止。
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
    slot.controlAt08 = nullptr;
    slot.baseAt00 = 0;
}
struct ReferenceGuard final
{
    Slot& slot;
    bool armed = true;
    ~ReferenceGuard() noexcept { if (armed) ReleaseYtlReference(slot); }
};
struct RawGuard final
{
    IEnvironmentRootMemory& memory;
    void* p;
    std::size_t size;
    ~RawGuard() noexcept { if (p) memory.DeleteBody(p, size); }
};

void DestroyEdges(MessageWindowEdgeReferenceVector& edges) noexcept
{
    // 461450跳461E80；与461750的字段顺序/大块cookie及PE maxState0、EHFlags5相同。
    DestroyDrawLayerReferencesAt461750(edges);
}
void AppendEdge(MessageWindowEdgeReferenceVector& edges, const Slot& temporary, IEnvironmentRootMemory& memory)
{
    // 455420与454580共核已逐叶核实；45FAD2只在无grow路径对当前end加12。
    AppendDrawLayerReference(edges, temporary, memory);
}
struct EdgeVectorGuard final
{
    MessageWindowEdgeReferenceVector& edges;
    bool armed = true;
    ~EdgeVectorGuard() noexcept { if (armed) DestroyEdges(edges); }
};

void Destroy(Normal& body) noexcept { DestroyMessageWindowNormalPaneNativeAt465570(body); }
void Destroy(Variable& body) noexcept { DestroyMessageWindowVariablePaneNative(body); }
void Destroy(Edge& body) noexcept { DestroyMessageWindowEdgeNative(body); }
void Destroy(Style& body) noexcept { DestroyMessageWindowStyleNativeAt465480(body); }
template<class T> class Control final : public EnvironmentObjectReferenceControl
{
public:
    explicit Control(T* body) noexcept : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() noexcept override
    {
        // 493280/4933A0/4930E0/493260：固定payload；正文没有虚析构，不能按pane表删除。
        auto* const body = static_cast<T*>(payloadAt14);
        if (!body) return;
        Destroy(*body);
        ::operator delete(body, sizeof(T));
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(Control<Normal>) == 0x18);
static_assert(sizeof(Control<Variable>) == 0x18);
static_assert(sizeof(Control<Edge>) == 0x18);
static_assert(sizeof(Control<Style>) == 0x18);

template<class T> Control<T>* NewControl(T* body, IEnvironmentRootMemory& memory)
{
    void* storage = memory.AllocateBody(sizeof(Control<T>));
    return storage ? ::new (storage) Control<T>(body) : nullptr;
}
template<class T> void Adopt(Slot& slot, T* completed, IEnvironmentRootMemory& memory)
{
    // 451B20/451BC0/451D00及45F960：旧control为空时不主动清旧base/bias。
    ReleaseYtlReference(slot);
    if (!completed) return;
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(completed);
    auto* const control = NewControl(completed, memory);
    slot.controlAt08 = control;
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = sizeof(T);
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}
template<class T> Control<T>* ConstructLocal(Slot& slot, T* completed, IEnvironmentRootMemory& memory)
{
    Initialize(slot);
    if (!completed) return nullptr;
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(completed);
    auto* const control = NewControl(completed, memory);
    // 45A302/45F8F0：不同于Adopt，完整三个字段先写，发布control后才写count。
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = sizeof(T);
    slot.controlAt08 = control;
    control->countAt10 = 1;
    return control;
}
template<class T, class DescriptorSource, class Construct>
T* NewBody(DescriptorSource descriptor, const Slot& config, Services& services, Construct construct)
{
    void* const storage = services.memory.AllocateBody(sizeof(T));
    if (!storage) return nullptr;
    RawGuard cleanup{services.memory, storage, sizeof(T)};
    auto* const body = ::new (storage) T;
    // 原caller在raw分配返回后才复制参数；成员不接管这个独立参数的拥有份额。
    auto parameter = CopyParameter(config);
    construct(*body, descriptor(), parameter, services);
    cleanup.p = nullptr;
    return body;
}
ColorTransWindowNativeBody38h* Window(const Normal& body) noexcept
{ return Resolve<ColorTransWindowNativeBody38h>(body.colorTransAt14); }
Normal* Center(const Variable& body) noexcept { return Resolve<Normal>(body.centerAt20); }
Normal* EdgeNormal(const Slot& slot) noexcept { return Resolve<Normal>(Resolve<Edge>(slot)->normalAt04); }

template<bool Composite>
void Submit(GraphicsSurfaceNativeBody* source, GraphicsSurfaceNativeBody* destination,
    std::int32_t x, std::int32_t y, const RECT* clip, Services& services)
{
    if constexpr (Composite)
    {
        // 4425C0/481F87：目标handle先读且先门控，源handle后读；参数固定后代理才Capture。
        const auto target = destination->handleAt04;
        if (!target) return;
        const auto image = source->handleAt04;
        if (!image) return;
        static_cast<void>(services.colorTrans.drawing.SurfaceBlitRaw(target, image, x, y, nullptr, nullptr, clip, 0));
    }
    else
    {
        const auto image = source->handleAt04;
        if (!image) return;
        static_cast<void>(services.colorTrans.drawing.SurfaceDrawRaw(image, x, y, nullptr, nullptr, clip, 0));
    }
}
template<bool Composite>
void RenderNormal(Normal& body, GraphicsSurfaceNativeBody* destination, const RECT* clip, Services& services)
{
    MessageWindowNativePaneScope scope(services);
    if (!Window(body)->processedAt1C.baseAt00) return;
    auto* surface = ResolveMessageWindowNormalProcessedNativeAt47DFE0(body);
    const auto x = surface->originXAt10;
    const auto y = surface->originYAt14;
    surface = ResolveMessageWindowNormalProcessedNativeAt47DFE0(body);
    Submit<Composite>(surface, destination, x, y, clip, services);
}
template<bool Composite>
void RenderVariable(Variable& body, GraphicsSurfaceNativeBody* destination, const RECT* clip, Services& services)
{
    MessageWindowNativePaneScope scope(services);
    if (!Window(*Center(body))->processedAt1C.baseAt00) return;
    auto* surface = ResolveMessageWindowNormalProcessedNativeAt47DFE0(*Center(body));
    const auto startX = surface->originXAt10;
    const auto startY = surface->originYAt14;
    surface = ResolveMessageWindowNormalProcessedNativeAt47DFE0(*Center(body));
    const auto width = surface->widthAt08;
    const auto height = surface->heightAt0C;
    auto y = startY;
    for (std::int32_t row = 0; row < body.rowsAt34; row = Add(row, 1), y = Add(y, height))
    {
        auto x = startX;
        for (std::int32_t column = 0; column < body.columnsAt2C; column = Add(column, 1), x = Add(x, width))
        {
            // 482F84/481F24：每块重解当前center/ColorTrans/processed，尺寸和起点则保持入口快照。
            auto* const current = ResolveMessageWindowNormalProcessedNativeAt47DFE0(*Center(body));
            Submit<Composite>(current, destination, x, y, clip, services);
        }
    }
    for (auto* cursor = body.edgesAt14.begin; cursor != body.edgesAt14.end; ++cursor)
    {
        const auto index = Resolve<Edge>(*cursor)->indexAt00;
        if (index > 3u) continue;
        if (!(index & 1u))
        {
            auto* const pane = &EdgeNormal(*cursor)->pane;
            if constexpr (Composite) pane->vptrAt00->compositeAt10(pane, destination, clip);
            else pane->vptrAt00->drawAt0C(pane, clip);
            continue;
        }
        if (!Window(*EdgeNormal(*cursor))->processedAt1C.baseAt00) continue;
        surface = ResolveMessageWindowNormalProcessedNativeAt47DFE0(*EdgeNormal(*cursor));
        const auto x = index == 1 ? Add(startX, Mul(width, body.columnsAt2C)) : surface->originXAt10;
        const auto edgeY = index == 1 ? surface->originYAt14 : Add(startY, Mul(height, body.rowsAt34));
        surface = ResolveMessageWindowNormalProcessedNativeAt47DFE0(*EdgeNormal(*cursor));
        Submit<Composite>(surface, destination, x, edgeY, clip, services);
    }
}

std::int32_t __fastcall NormalType(Pane*, void*) { return 0; }
std::int32_t __fastcall VariableType(Pane*, void*) { return 1; }
RECT* __fastcall Bounds(Pane* body, void*, RECT* out) { return ReadMessageWindowPaneBoundsNativeAt479DD0(*body, out); }
std::uint8_t __fastcall NormalRefresh(Pane* body, void*)
{ return RefreshMessageWindowNormalPaneNativeAt4874C0(*reinterpret_cast<Normal*>(body), Current()); }
void __fastcall VariableRefresh(Pane* body, void*)
{ RefreshMessageWindowVariablePaneNativeAt487540(*reinterpret_cast<Variable*>(body), Current()); }
void __fastcall NormalDraw(Pane* body, void*, const RECT* clip)
{ DrawMessageWindowNormalPaneNativeAt482DB0(*reinterpret_cast<Normal*>(body), clip, Current()); }
void __fastcall VariableDraw(Pane* body, void*, const RECT* clip)
{ DrawMessageWindowVariablePaneNativeAt482E80(*reinterpret_cast<Variable*>(body), clip, Current()); }
void __fastcall NormalComposite(Pane* body, void*, GraphicsSurfaceNativeBody* destination, const RECT* clip)
{ CompositeMessageWindowNormalPaneNativeAt481D50(*reinterpret_cast<Normal*>(body), destination, clip, Current()); }
void __fastcall VariableComposite(Pane* body, void*, GraphicsSurfaceNativeBody* destination, const RECT* clip)
{ CompositeMessageWindowVariablePaneNativeAt481E20(*reinterpret_cast<Variable*>(body), destination, clip, Current()); }
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const MessageWindowPaneNativeVtable normalTable{
    reinterpret_cast<decltype(MessageWindowPaneNativeVtable::typeAt00)>(&NormalType),
    reinterpret_cast<decltype(MessageWindowPaneNativeVtable::boundsAt04)>(&Bounds),
    reinterpret_cast<decltype(MessageWindowPaneNativeVtable::refreshAt08)>(&NormalRefresh),
    reinterpret_cast<decltype(MessageWindowPaneNativeVtable::drawAt0C)>(&NormalDraw),
    reinterpret_cast<decltype(MessageWindowPaneNativeVtable::compositeAt10)>(&NormalComposite)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const MessageWindowPaneNativeVtable variableTable{
    reinterpret_cast<decltype(MessageWindowPaneNativeVtable::typeAt00)>(&VariableType),
    reinterpret_cast<decltype(MessageWindowPaneNativeVtable::boundsAt04)>(&Bounds),
    reinterpret_cast<decltype(MessageWindowPaneNativeVtable::refreshAt08)>(&VariableRefresh),
    reinterpret_cast<decltype(MessageWindowPaneNativeVtable::drawAt0C)>(&VariableDraw),
    reinterpret_cast<decltype(MessageWindowPaneNativeVtable::compositeAt10)>(&VariableComposite)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
constexpr MessageWindowNormalPaneNativeDescriptor sampleBody1{
    nullptr, "\x83\x81\x83\x62\x83\x5A\x98\x67.vrm", 0};
constexpr MessageWindowNormalPaneNativeDescriptor sampleBody2{
    nullptr, "\x83\x81\x83\x62\x83\x5A\x98\x67" "2.vrm", 0};
constexpr MessageWindowStyleNativeDescriptor sampleStyles[2]{
    {&sampleBody1, nullptr, nullptr, nullptr, {}, 0},
    {&sampleBody2, nullptr, nullptr, nullptr, {}, 1}};
}

MessageWindowNativePaneScope::MessageWindowNativePaneScope(Services& services) noexcept
    : previous_(currentServices), surfaces_(services.colorTrans.surfaces) { currentServices = &services; }
MessageWindowNativePaneScope::~MessageWindowNativePaneScope() noexcept { currentServices = previous_; }
const MessageWindowPaneNativeVtable& MessageWindowNormalPaneNativeMethods() noexcept { return normalTable; }
const MessageWindowPaneNativeVtable& MessageWindowVariablePaneNativeMethods() noexcept { return variableTable; }
const MessageWindowStyleNativeDescriptor* MessageWindowStylesNativeSample18At601408() noexcept { return sampleStyles; }
Pane* ResolveMessageWindowPaneNative(const Slot& slot) noexcept { return Resolve<Pane>(slot); }
Style* ResolveMessageWindowStyleNative(const Slot& slot) noexcept { return Resolve<Style>(slot); }
Edge* ResolveMessageWindowEdgeNative(const Slot& slot) noexcept { return Resolve<Edge>(slot); }
void AdoptMessageWindowNormalPaneNativeAt451B20(Slot& slot, Normal* body, IEnvironmentRootMemory& memory)
{ Adopt(slot, body, memory); }
void AdoptMessageWindowNormalPaneNativeAt451BC0(Slot& slot, Normal* body, IEnvironmentRootMemory& memory)
{ Adopt(slot, body, memory); }
void AdoptMessageWindowVariablePaneNativeAt451D00(Slot& slot, Variable* body, IEnvironmentRootMemory& memory)
{ Adopt(slot, body, memory); }
void ConstructMessageWindowStyleReferenceAt45A2C5(Slot& slot, Style* body, IEnvironmentRootMemory& memory)
{ static_cast<void>(ConstructLocal(slot, body, memory)); }
void DestroyMessageWindowNormalPaneNativeAt465570(Normal& body) noexcept
{ ReleaseYtlReference(body.configAt20); ReleaseYtlReference(body.colorTransAt14); }
void DestroyMessageWindowVariablePaneNative(Variable& body) noexcept
{ ReleaseYtlReference(body.centerAt20); DestroyEdges(body.edgesAt14); }
void DestroyMessageWindowEdgeNative(Edge& body) noexcept { ReleaseYtlReference(body.normalAt04); }
void DestroyMessageWindowStyleNativeAt465480(Style& body) noexcept
{ ReleaseYtlReference(body.configAt18); ReleaseYtlReference(body.nameAt0C); ReleaseYtlReference(body.bodyAt00); }

Normal* ConstructMessageWindowNormalPaneNativeAt45F2E0(Normal& body,
    const MessageWindowNormalPaneNativeDescriptor* descriptor, Slot config, Services& services)
{
    MessageWindowNativePaneScope scope(services);
    ReferenceGuard parameter{config};
    body.pane.boundsAt04 = {};
    body.pane.vptrAt00 = &normalTable;
    Initialize(body.colorTransAt14); ReferenceGuard colorTrans{body.colorTransAt14};
    Initialize(body.configAt20); ReferenceGuard memberConfig{body.configAt20};
    PublishConfigMember(body.configAt20, config);
    auto* raw = services.memory.AllocateBody(sizeof(ColorTransWindowNativeBody38h));
    ColorTransWindowNativeBody38h* completed = nullptr;
    if (raw)
    {
        RawGuard cleanup{services.memory, raw, sizeof(ColorTransWindowNativeBody38h)};
        completed = ::new (raw) ColorTransWindowNativeBody38h;
        // 45F399：descriptor三个DWORD在38h分配返回之后才读，不能提前作为New helper实参。
        ConstructColorTransWindowNativeAt45E970(*completed, descriptor->firstNameAt00,
            descriptor->secondNameAt04, descriptor->modeAt08, services.colorTrans);
        cleanup.p = nullptr;
    }
    AdoptColorTransWindowNativeAt45F3B7(body.colorTransAt14, completed);
    SetRectEmpty(&body.pane.boundsAt04);
    if (Window(body)->processedAt1C.baseAt00)
    {
        if (!RefreshMessageWindowNormalPaneNativeAt4874C0(body, services)) throw 1;
        auto* surface = ResolveMessageWindowNormalProcessedNativeAt47DFE0(body);
        const auto width = surface->widthAt08;
        const auto height = surface->heightAt0C;
        surface = ResolveMessageWindowNormalProcessedNativeAt47DFE0(body);
        const auto x = surface->originXAt10;
        const auto y = surface->originYAt14;
        body.pane.boundsAt04 = {x, y, Add(x, width), Add(y, height)};
    }
    memberConfig.armed = false; colorTrans.armed = false; parameter.armed = false;
    ReleaseParameter(config);
    return &body;
}
Normal* NewMessageWindowNormalPaneNative(const MessageWindowNormalPaneNativeDescriptor* descriptor,
    const Slot& config, Services& services)
{ return NewBody<Normal>([&] { return descriptor; }, config, services, ConstructMessageWindowNormalPaneNativeAt45F2E0); }
Variable* NewMessageWindowVariablePaneNative(const MessageWindowVariablePaneNativeDescriptor* descriptor,
    const Slot& config, Services& services)
{ return NewBody<Variable>([&] { return descriptor; }, config, services, ConstructMessageWindowVariablePaneNativeAt45F720); }
Style* NewMessageWindowStyleNative(const MessageWindowStyleNativeDescriptor* descriptor,
    const Slot& config, Services& services)
{ return NewBody<Style>([&] { return descriptor; }, config, services, ConstructMessageWindowStyleNativeAt45F040); }

Variable* ConstructMessageWindowVariablePaneNativeAt45F720(Variable& body,
    const MessageWindowVariablePaneNativeDescriptor* descriptor, Slot config, Services& services)
{
    MessageWindowNativePaneScope scope(services);
    ReferenceGuard parameter{config};
    body.pane.boundsAt04 = {};
    body.pane.vptrAt00 = &variableTable;
    ::new (static_cast<void*>(&body.edgesAt14)) MessageWindowEdgeReferenceVector{};
    EdgeVectorGuard edges{body.edgesAt14};
    Initialize(body.centerAt20); ReferenceGuard center{body.centerAt20};
    auto* normal = NewBody<Normal>([&] { return descriptor->centerAt00; }, config, services,
        ConstructMessageWindowNormalPaneNativeAt45F2E0);
    AdoptMessageWindowNormalPaneNativeAt451BC0(body.centerAt20, normal, services.memory);
    body.rowsAt34 = 1;
    body.columnsAt2C = 1;
    body.maxColumnsAt30 = descriptor->maxColumnsAt04;
    body.maxRowsAt38 = descriptor->maxRowsAt08;
    RECT bounds;
    auto* pane = &Center(body)->pane;
    pane->vptrAt00->boundsAt04(pane, &bounds);
    const auto bottom = Add(bounds.top, Mul(Sub(bounds.bottom, bounds.top), body.maxRowsAt38));
    const auto right = Add(bounds.left, Mul(Sub(bounds.right, bounds.left), body.maxColumnsAt30));
    for (std::uint32_t index = 0; index != 4; ++index)
    {
        if (!descriptor->edgesAt0C[index]) continue;
        void* const raw = services.memory.AllocateBody(sizeof(Edge));
        Edge* edge = nullptr;
        if (raw)
        {
            edge = ::new (raw) Edge;
            Initialize(edge->normalAt04);
        }
        Slot edgeSlot;
        // 45F8F0前没有edge清理资格；control申请失败不补删完成的裸10h。
        auto* const cachedEdgeControl = ConstructLocal(edgeSlot, edge, services.memory);
        ReferenceGuard edgeGuard{edgeSlot};
        edge->indexAt00 = index;
        normal = NewBody<Normal>([&] { return descriptor->edgesAt0C[index]; }, config, services,
            ConstructMessageWindowNormalPaneNativeAt45F2E0);
        AdoptMessageWindowNormalPaneNativeAt451BC0(edge->normalAt04, normal, services.memory);
        // 45F9F9直接取局部base，非Ref解析；随后normal才按其当前Ref解址。
        edge = reinterpret_cast<Edge*>(edgeSlot.baseAt00);
        pane = &Resolve<Normal>(edge->normalAt04)->pane;
        RECT local;
        const RECT* rectangle = pane->vptrAt00->boundsAt04(pane, &local);
        if (index & 1u)
        {
            // 奇数项原调用忽略返回地址，修改传入的栈RECT；偶数项Union使用虚槽返回地址。
            if (index == 1)
            {
                local.right = Add(Sub(local.right, local.left), right);
                local.left = right;
            }
            else
            {
                local.bottom = Add(Sub(local.bottom, local.top), bottom);
                local.top = bottom;
            }
            rectangle = &local;
        }
        UnionRect(&body.pane.boundsAt04, &body.pane.boundsAt04, rectangle);
        Slot temporary{edgeSlot.baseAt00, 0, cachedEdgeControl};
        ReferenceGuard temporaryGuard{temporary};
        if (cachedEdgeControl) ++cachedEdgeControl->referenceCountAt04;
        AppendEdge(body.edgesAt14, temporary, services.memory);
        auto* const cachedTemporaryControl = temporary.controlAt08;
        temporaryGuard.armed = false;
        ReleaseCached(temporary, cachedTemporaryControl);
        edgeGuard.armed = false;
        ReleaseCached(edgeSlot, cachedEdgeControl);
    }
    center.armed = false; edges.armed = false; parameter.armed = false;
    ReleaseParameter(config);
    return &body;
}

Style* ConstructMessageWindowStyleNativeAt45F040(Style& body,
    const MessageWindowStyleNativeDescriptor* descriptor, Slot config, Services& services)
{
    MessageWindowNativePaneScope scope(services);
    ReferenceGuard parameter{config};
    Initialize(body.bodyAt00); ReferenceGuard bodyGuard{body.bodyAt00};
    Initialize(body.nameAt0C); ReferenceGuard nameGuard{body.nameAt0C};
    Initialize(body.configAt18); ReferenceGuard configGuard{body.configAt18};
    body.nameBaselineAt24 = {};
    if (descriptor->normalBodyAt00)
        AdoptMessageWindowNormalPaneNativeAt451B20(body.bodyAt00,
            NewBody<Normal>([&] { return descriptor->normalBodyAt00; }, config, services,
                ConstructMessageWindowNormalPaneNativeAt45F2E0), services.memory);
    else if (descriptor->variableBodyAt04)
        AdoptMessageWindowVariablePaneNativeAt451D00(body.bodyAt00,
            NewBody<Variable>([&] { return descriptor->variableBodyAt04; }, config, services,
                ConstructMessageWindowVariablePaneNativeAt45F720), services.memory);
    if (descriptor->normalNameAt08)
        AdoptMessageWindowNormalPaneNativeAt451B20(body.nameAt0C,
            NewBody<Normal>([&] { return descriptor->normalNameAt08; }, config, services,
                ConstructMessageWindowNormalPaneNativeAt45F2E0), services.memory);
    else if (descriptor->variableNameAt0C)
    {
        AdoptMessageWindowVariablePaneNativeAt451D00(body.nameAt0C,
            NewBody<Variable>([&] { return descriptor->variableNameAt0C; }, config, services,
                ConstructMessageWindowVariablePaneNativeAt45F720), services.memory);
        CopyRect(&body.nameBaselineAt24, &descriptor->nameBaselineAt10);
    }
    // 45F259：先释放当前config；pane分配/构造回调可给此槽晚填，不能假定仍为空。
    ReleaseYtlReference(body.configAt18);
    PublishConfigMember(body.configAt18, config);
    configGuard.armed = false; nameGuard.armed = false; bodyGuard.armed = false; parameter.armed = false;
    ReleaseParameter(config);
    return &body;
}

RECT* ReadMessageWindowPaneBoundsNativeAt479DD0(Pane& body, RECT* out) noexcept
{ *out = body.boundsAt04; return out; }
GraphicsSurfaceNativeBody* ResolveMessageWindowNormalProcessedNativeAt47DFE0(const Normal& body) noexcept
{ return ResolveColorTransProcessedNative(*Window(body)); }
Variable* ResolveMessageWindowNamePaneNativeAt46CBF0(const Style& body) noexcept
{ return Resolve<Variable>(body.nameAt0C); }
std::uint8_t RefreshMessageWindowNormalPaneNativeAt4874C0(Normal& body, Services& services)
{
    MessageWindowNativePaneScope scope(services);
    union Output { MessageWindowColors colors; Output() noexcept {} } output;
    auto* const config = ResolveMessageWindowNativeConfig(body.configAt20);
    config->vptrAt00->readAt00(config, nullptr, &output.colors);
    return ApplyColorTransWindowNativeAt46F6D0(*Window(body), output.colors.color, output.colors.transparency, services.colorTrans);
}
void RefreshMessageWindowVariablePaneNativeAt487540(Variable& body, Services& services)
{
    MessageWindowNativePaneScope scope(services);
    auto* const center = &Center(body)->pane;
    center->vptrAt00->refreshAt08(center);
    // 48756A：先signed除12检查非零，再按当前end逐项，不把畸形短差当作一项。
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(body.edgesAt14.end) -
        reinterpret_cast<std::uintptr_t>(body.edgesAt14.begin));
    if (std::bit_cast<std::int32_t>(bytes) / 12 == 0) return;
    for (auto* cursor = body.edgesAt14.begin; cursor != body.edgesAt14.end; ++cursor)
    {
        auto* const pane = &EdgeNormal(*cursor)->pane;
        pane->vptrAt00->refreshAt08(pane);
    }
}
void DrawMessageWindowNormalPaneNativeAt482DB0(Normal& body, const RECT* clip, Services& services)
{ RenderNormal<false>(body, nullptr, clip, services); }
void CompositeMessageWindowNormalPaneNativeAt481D50(Normal& body, GraphicsSurfaceNativeBody* destination,
    const RECT* clip, Services& services) { RenderNormal<true>(body, destination, clip, services); }
void DrawMessageWindowVariablePaneNativeAt482E80(Variable& body, const RECT* clip, Services& services)
{ RenderVariable<false>(body, nullptr, clip, services); }
void CompositeMessageWindowVariablePaneNativeAt481E20(Variable& body, GraphicsSurfaceNativeBody* destination,
    const RECT* clip, Services& services) { RenderVariable<true>(body, destination, clip, services); }

void PrepareMessageWindowNativeNameAt46B470(Style& body, IMessageWindowNativeNameSource& source)
{
    // 46B49A/46B4C2在type检查之前原样写偏移；normalName越正文的原始边界不加sidecar兜底。
    auto raw = reinterpret_cast<std::uintptr_t>(ResolveMessageWindowPaneNative(body.nameAt0C));
    *reinterpret_cast<std::int32_t*>(raw + 0x34u) = 1;
    raw = reinterpret_cast<std::uintptr_t>(ResolveMessageWindowPaneNative(body.nameAt0C));
    *reinterpret_cast<std::int32_t*>(raw + 0x2Cu) = 1;
    auto* const pane = ResolveMessageWindowPaneNative(body.nameAt0C);
    if (pane->vptrAt00->typeAt00(pane) != 1) return;
    auto* const surface = ResolveMessageWindowNormalProcessedNativeAt47DFE0(*Center(
        *ResolveMessageWindowNamePaneNativeAt46CBF0(body)));
    const auto baseline = body.nameBaselineAt24;
    const auto width = surface->widthAt08;
    const auto height = surface->heightAt0C;
    const RECT text = *source.CaptureTextWindowNameBoundsAt46B558();
    auto bottom = baseline.bottom;
    while (Sub(bottom, baseline.top) < Sub(text.bottom, text.top))
    {
        const auto* const limit = ResolveMessageWindowNamePaneNativeAt46CBF0(body);
        if (ResolveMessageWindowNamePaneNativeAt46CBF0(body)->rowsAt34 >= limit->maxRowsAt38) break;
        auto* const current = ResolveMessageWindowNamePaneNativeAt46CBF0(body);
        bottom = Add(bottom, height);
        current->rowsAt34 = Add(current->rowsAt34, 1);
    }
    auto right = baseline.right;
    while (Sub(right, baseline.left) < Sub(text.right, text.left))
    {
        const auto* const limit = ResolveMessageWindowNamePaneNativeAt46CBF0(body);
        if (ResolveMessageWindowNamePaneNativeAt46CBF0(body)->columnsAt2C >= limit->maxColumnsAt30) break;
        auto* const current = ResolveMessageWindowNamePaneNativeAt46CBF0(body);
        right = Add(right, width);
        current->columnsAt2C = Add(current->columnsAt2C, 1);
    }
}
}
