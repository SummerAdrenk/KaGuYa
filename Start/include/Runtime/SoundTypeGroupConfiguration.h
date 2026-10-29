#pragma once
#include <Runtime/SoundParameterNative.h>

namespace KaGuYa::Start::Runtime
{
class ISoundParamsSettingsSource;

// 外围CSoundParams迁移期间保留唯一12-byte分组Ref；不持有配置/存档镜像。
class SoundTypeGroupOwner final
{
public:
    SoundTypeGroupOwner() noexcept = default;
    ~SoundTypeGroupOwner() noexcept;
    SoundTypeGroupOwner(const SoundTypeGroupOwner&) = delete;
    SoundTypeGroupOwner& operator=(const SoundTypeGroupOwner&) = delete;
    [[nodiscard]] YtlReferenceSlotRuntime& Native() noexcept { return slot_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& Native() const noexcept { return slot_; }
    [[nodiscard]] SoundTypeGroupNative* Get() const noexcept;
    void Serialize(SerializeBuffer& serializer);
    void Reset() noexcept;
private:
    YtlReferenceSlotRuntime slot_{};
};
static_assert(sizeof(SoundTypeGroupOwner) == 12);

// 474740完整类型配置顺序；expression求值仍沿既有47BBE0适配器，见审计保留项。
void ConfigureSoundTypeGroupAt474740(SoundTypeGroupOwner& destination,
    ISoundParamsSettingsSource& source, ISettingTagErrorSource& errors,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

bool ReadSoundBoolOrDefault(ISettingTagErrorSource& errors,
    const YtlReferenceSlotRuntime& tag, const char* key, bool originalDefault);
std::uint32_t ReadSoundVolumeOrDefault(ISettingTagErrorSource& errors,
    const YtlReferenceSlotRuntime& tag);
[[nodiscard]] bool IsSoundTypeAllowedAt46C570(SoundTypeGroupNative* group, const char* resourceName);
// 返回槽在字符串正常释放前增持；释放失败时调用方尚未取得此返回Ref的清理资格。
void FindSoundTypeAt489AC0(YtlReferenceSlotRuntime& result, SoundTypeGroupNative* group,
    const char* resourceName);
}
