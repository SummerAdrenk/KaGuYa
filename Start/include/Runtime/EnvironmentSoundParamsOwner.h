#pragma once

#include <Runtime/SoundParamsReference.h>

namespace KaGuYa::Start::Runtime
{
// 45D5C0 的独立 CSoundParams；45D1DA..45D1F1 共享此对象，不共享整个 SaveData。
// 真正44h/46槽正文由Ref12/control18h持有；现代播放接口只是外置借用适配器。
class EnvironmentSoundParamsOwner final
{
public:
    // 固定输入只供显式工具入口；生产在每次配置阶段通过 source 捕获当前 GS。
    explicit EnvironmentSoundParamsOwner(const ParamsGameSystem& gameSystem,
        ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner());
    explicit EnvironmentSoundParamsOwner(ISoundParamsSettingsSource& source,
        ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner());
    // manager+24拥有自己的Ref；仅共享正文，不共享SaveData中的现代适配器。
    explicit EnvironmentSoundParamsOwner(const YtlReferenceSlotRuntime& source);

    EnvironmentSoundParamsOwner(const EnvironmentSoundParamsOwner&) = delete;
    EnvironmentSoundParamsOwner& operator=(const EnvironmentSoundParamsOwner&) = delete;
    EnvironmentSoundParamsOwner(EnvironmentSoundParamsOwner&&) = delete;
    EnvironmentSoundParamsOwner& operator=(EnvironmentSoundParamsOwner&&) = delete;

    [[nodiscard]] EnvironmentSoundParamsArchive& Archive() noexcept;
    [[nodiscard]] const EnvironmentSoundParamsArchive& Archive() const noexcept;
    [[nodiscard]] EnvironmentSoundParamsRuntime& Runtime() noexcept;
    [[nodiscard]] const EnvironmentSoundParamsRuntime& Runtime() const noexcept;
    [[nodiscard]] const YtlReferenceSlotRuntime& NativeReference() const noexcept { return reference_.Native(); }

private:
    SoundParamsReference reference_;
    EnvironmentSoundParamsRuntime runtime_;
};
}
