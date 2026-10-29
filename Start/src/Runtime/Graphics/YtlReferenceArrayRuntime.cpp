#include <Runtime/YtlReferenceArrayRuntime.h>

namespace KaGuYa::Start::Runtime
{
void ReleaseYtlReference(YtlReferenceSlotRuntime& reference)
{
    auto* control = reference.controlAt08;
    if (!control) return; // 45176C / 48F9BC：无control时base/bias不变。
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        // 45177E / 48F9CE：正文回调可改槽，重读control且不对新control再递减。
        control = reference.controlAt08;
        if (control) control->DeleteControlAtVslot04(1);
    }
    reference.controlAt08 = nullptr;
    reference.baseAt00 = 0;
}

void ReleaseYtlReferencesAt48F980(std::span<YtlReferenceSlotRuntime> references) noexcept
{
    for (auto& reference : references)
        ReleaseYtlReference(reference);
}

YtlReferenceArrayRuntime::~YtlReferenceArrayRuntime() { DestroyStorage(); }
void YtlReferenceArrayRuntime::AdoptReference(YtlReferenceSlotRuntime reference)
    { references_.push_back(reference); }
void YtlReferenceArrayRuntime::ClearPreservingStorage() noexcept
{
    ReleaseYtlReferencesAt48F980(references_);
    references_.clear(); // 元素回调结束前不修改end；保留容量。
}
void YtlReferenceArrayRuntime::DestroyStorage() noexcept
{
    ClearPreservingStorage();
    std::vector<YtlReferenceSlotRuntime>{}.swap(references_);
}
}
