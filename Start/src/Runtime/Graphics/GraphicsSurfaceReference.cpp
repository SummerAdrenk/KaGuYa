#include <Runtime/GraphicsSurfaceReference.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
void ReleaseWithoutClearing(YtlReferenceSlotRuntime& slot)
{
    auto* control = slot.controlAt08;
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control = slot.controlAt08;
        if (control) control->DeleteControlAtVslot04(1);
    }
}
void ReleaseArgumentNormally(YtlReferenceSlotRuntime& slot) noexcept
{
    // 4C1CD7/48CAA4把EH state置1；正常参数析构抛出直接terminate，不重试。
    ReleaseWithoutClearing(slot);
}
void ReleasePopupArgumentNormally(YtlReferenceControlRuntime* control) noexcept
{
    // 50DDA6的state1：固定ESI，但正文回调后须重新从该control取删除虚槽。
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
}
struct ArgumentUnwind final
{
    YtlReferenceSlotRuntime& slot;
    ~ArgumentUnwind() noexcept { ReleaseYtlReference(slot); } // 原40D2F0。
};
void AssignConsumingArgument(YtlReferenceSlotRuntime& target, YtlReferenceSlotRuntime& argument)
{
    try
    {
        if (&target != &argument)
        {
            // 原setter直到旧control释放完成仍保持target三字段；来源在回调之后重读。
            ReleaseWithoutClearing(target);
            CopyGraphicsSurfaceReferenceAt47D740(target, argument);
        }
    }
    catch (...)
    {
        ArgumentUnwind unwind{argument};
        throw;
    }
    ReleaseArgumentNormally(argument);
}
}

void AssignGraphicsSurfaceReferenceAt4C1C60(YtlReferenceSlotRuntime& target,
    YtlReferenceSlotRuntime& argument)
{ AssignConsumingArgument(target, argument); }
void AssignGraphicsSurfaceReferenceAt48CA30(YtlReferenceSlotRuntime& target,
    YtlReferenceSlotRuntime& argument)
{ AssignConsumingArgument(target, argument); }

void CopyAssignGraphicsSurfaceReferenceAt465870(YtlReferenceSlotRuntime& target,
    const YtlReferenceSlotRuntime& source)
{
    if (&target == &source) return;
    ReleaseYtlReference(target);
    CopyGraphicsSurfaceReferenceAt47D740(target, source);
}

void ConsumeGraphicsSurfaceReferenceAt477C60(YtlReferenceSlotRuntime& out,
    YtlReferenceSlotRuntime& rootSlot)
{
    CopyGraphicsSurfaceReferenceAt47D740(out, rootSlot);
    try { ReleaseYtlReference(rootSlot); }
    catch (...)
    {
        ArgumentUnwind unwind{out};
        throw;
    }
}

void TransferGraphicsSurfaceReferenceAt50DCA0(YtlReferenceSlotRuntime& target,
    YtlReferenceSlotRuntime& rootSlot, YtlReferenceSlotRuntime& temporary)
{
    auto* const control = rootSlot.controlAt08;
    const auto bias = rootSlot.biasAt04;
    const auto base = rootSlot.baseAt00;
    temporary = {base, bias, control};
    if (control) ++control->referenceCountAt04;
    try
    {
        ReleaseYtlReference(rootSlot);
        if (&target != &temporary)
        {
            ReleaseWithoutClearing(target);
            // 50DD91..50DDA3不重读temporary；回调改工作槽不改变缓存的base/bias/ESI。
            target.baseAt00 = base;
            target.biasAt04 = bias;
            target.controlAt08 = control;
            if (control) ++control->referenceCountAt04;
        }
    }
    catch (...)
    {
        ArgumentUnwind unwind{temporary};
        throw;
    }
    ReleasePopupArgumentNormally(control);
}

void CopyGraphicsSurfaceReferenceAt47D740(YtlReferenceSlotRuntime& out,
    const YtlReferenceSlotRuntime& source) noexcept
{
    out = source;
    if (out.controlAt08) ++out.controlAt08->referenceCountAt04;
}

GraphicsSurfaceReference::GraphicsSurfaceReference(const YtlReferenceSlotRuntime& slot,
    IGraphicsSurfaceApi& api) noexcept : api_(&api)
{ CopyGraphicsSurfaceReferenceAt47D740(slot_, slot); }
GraphicsSurfaceReference::GraphicsSurfaceReference(const GraphicsSurfaceReference& other) noexcept
    : GraphicsSurfaceReference(other.slot_, *other.api_) {}
GraphicsSurfaceReference::GraphicsSurfaceReference(GraphicsSurfaceReference&& other) noexcept
    : slot_(other.slot_), api_(other.api_)
{ other.slot_ = {}; }
GraphicsSurfaceReference::~GraphicsSurfaceReference() noexcept { Reset(); }

GraphicsSurfaceReference& GraphicsSurfaceReference::operator=(const GraphicsSurfaceReference& other)
{
    if (this != &other)
    {
        // 仅现代返回值适配的赋值；先建立临时强引用，释放期间不会销毁复制来源。
        GraphicsSurfaceReference next(other);
        Reset();
        slot_ = next.slot_;
        api_ = next.api_;
        next.slot_ = {};
    }
    return *this;
}
GraphicsSurfaceReference& GraphicsSurfaceReference::operator=(GraphicsSurfaceReference&& other)
{
    if (this != &other)
    {
        Reset();
        slot_ = other.slot_;
        api_ = other.api_;
        other.slot_ = {};
    }
    return *this;
}
void GraphicsSurfaceReference::Reset()
{
    GraphicsSurfaceNativeScope scope(*api_);
    ReleaseYtlReference(slot_);
}
void GraphicsSurfaceReference::ConstructAt40C2E0()
{
    GraphicsSurfaceNativeScope scope(*api_);
    auto* body = NewGraphicsSurfaceAt40C2E0(*api_);
    AdoptGraphicsSurfaceAt407A80(slot_, body);
}
GraphicsSurfaceNativeBody* GraphicsSurfaceReference::Get() const noexcept
{ return ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(slot_); }
}
