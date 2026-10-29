#pragma once
#include <Runtime/SoundParamsEnvironmentArchive.h>

namespace KaGuYa::Start::Runtime
{
EnvironmentSoundParamsArchive* CreateSoundParamsAt45B4D8(ISoundParamsSettingsSource& source,
    ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner(),
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void ReplaceSoundParamsAt451800(YtlReferenceSlotRuntime& slot, EnvironmentSoundParamsArchive* created,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
// 45D1A3..45D1F1：释放目标后重读源三字段，保留自赋值及释放回调的重入顺序。
void ShareSoundParamsAt45D1A3(YtlReferenceSlotRuntime& destination,
    const YtlReferenceSlotRuntime& source);

class SoundParamsReference final
{
public:
    SoundParamsReference() noexcept = default;
    explicit SoundParamsReference(const YtlReferenceSlotRuntime& source);
    SoundParamsReference(ISoundParamsSettingsSource& source, ISettingTagErrorSource& errors);
    SoundParamsReference(const ParamsGameSystem& gameSystem, ISettingTagErrorSource& errors);
    ~SoundParamsReference() noexcept;
    SoundParamsReference(const SoundParamsReference&) = delete;
    SoundParamsReference& operator=(const SoundParamsReference&) = delete;
    [[nodiscard]] EnvironmentSoundParamsArchive& Get() const noexcept;
    [[nodiscard]] YtlReferenceSlotRuntime& Native() noexcept { return slot_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& Native() const noexcept { return slot_; }
private:
    YtlReferenceSlotRuntime slot_{};
};
static_assert(sizeof(SoundParamsReference) == 12);
}
