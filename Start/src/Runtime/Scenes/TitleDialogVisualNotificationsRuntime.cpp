#include <Runtime/TitleDialogVisualNotificationsRuntime.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
TitleDialogVisualNotificationsRuntime::TitleDialogVisualNotificationsRuntime(
    ITitleDialogVisualServices& services, CgOperationClickedEventManagerRuntime& events) noexcept
    : services_(&services), events_(&events)
{
}

bool TitleDialogVisualNotificationsRuntime::ReadPointerMovedAtVslot7C()
{
    return services_->ReadPointerMovedAtVslot7C();
}

void TitleDialogVisualNotificationsRuntime::SubmitChildRectAt50FC80(const RECT& rect)
{
    services_->SubmitChildRectAt50FC80(rect);
}

void TitleDialogVisualNotificationsRuntime::NotifyChildChangedAt518AE0(CgOperationChildControlRef child)
{
    if (!child) throw std::invalid_argument("Title visual notification child is null");
    const auto id = child->EventValueAt04();
    if (id < 3 || id > 8) return;
    const auto state = child->VisualStateAtVslot38();
    if (state != 3 && state != 4) return;
    // 50FDB0(clear=0) 只查首个已触发事件，不是判断整个表中是否包含该 ID。
    if (events_->PollFirstAt50FDB0(0) == id) return;
    const char* const name = services_->ReadHoverSoundNameAt61A65C();
    const auto volume = services_->ReadEffectVolumeAtVslot24();
    services_->PlayEffectAt4BCD70(name, 1, volume, 1);
}
}
