#pragma once

#include <Runtime/TitleDialogInputRuntime.h>

namespace KaGuYa::Start::Runtime
{
class ITitleDialogHoverSoundServices
{
public:
    virtual ~ITitleDialogHoverSoundServices() = default;
    virtual const char* ReadHoverSoundNameAt61A65C() = 0;
    virtual std::int32_t ReadEffectVolumeAtVslot24() = 0;
    virtual void PlayEffectAt4BCD70(const char* name, std::int32_t mode,
        std::int32_t volume, std::int32_t loop) = 0;
};

class ITitleDialogVisualServices : public ITitleDialogHoverSoundServices
{
public:
    virtual bool ReadPointerMovedAtVslot7C() = 0;
    virtual void SubmitChildRectAt50FC80(const RECT& rect) = 0;
};

// Title +88=518AE0，不同于 Opacity 的空通知 518A80。
class TitleDialogVisualNotificationsRuntime final : public ITitleDialogInputNotifications
{
public:
    TitleDialogVisualNotificationsRuntime(ITitleDialogVisualServices& services,
        CgOperationClickedEventManagerRuntime& events) noexcept;
    bool ReadPointerMovedAtVslot7C() override;
    void SubmitChildRectAt50FC80(const RECT& rect) override;
    void NotifyChildChangedAt518AE0(CgOperationChildControlRef child) override;

private:
    ITitleDialogVisualServices* services_;
    CgOperationClickedEventManagerRuntime* events_;
};
}
