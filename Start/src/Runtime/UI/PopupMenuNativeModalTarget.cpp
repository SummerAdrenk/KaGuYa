#include <Runtime/PopupMenuNativeModalTarget.h>

namespace KaGuYa::Start::Runtime
{
IModalDialogRuntimeTarget* PopupMenuNativeModalTarget::ResolveAt4A5330()
{
    auto* body = ResolveEnvironmentObjectReference<PopupMenuNativeBodyA4h>(slot_);
    selected_ = body ? &body->common : nullptr;
    return selected_ ? this : nullptr;
}
void PopupMenuNativeModalTarget::BeginAtVslot44()
{
    auto* selected = selected_;
    PopupMenuNativeScope scope(dependencies_);
    selected->vptrAt00->beginAt44(selected);
}
bool PopupMenuNativeModalTarget::PollAtVslot48()
{
    auto* selected = selected_;
    PopupMenuNativeScope scope(dependencies_);
    return selected->vptrAt00->pollAt48(selected) != 0;
}
bool PopupMenuNativeModalTarget::EndAtVslot4C()
{
    auto* selected = selected_;
    PopupMenuNativeScope scope(dependencies_);
    return selected->vptrAt00->endAt4C(selected) != 0;
}
std::int32_t PopupMenuNativeModalTarget::ResultAtVslot50() const
{
    auto* selected = selected_;
    PopupMenuNativeScope scope(dependencies_);
    return selected->vptrAt00->resultAt50(selected);
}
}
