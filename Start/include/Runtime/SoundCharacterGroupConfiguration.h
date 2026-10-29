#pragma once
#include <Runtime/SoundParameterNative.h>

namespace KaGuYa::Start::Runtime
{
class ISoundParamsSettingsSource;
class ISettingTagErrorSource;

class SoundCharacterGroupOwner final
{
public:
    SoundCharacterGroupOwner() noexcept = default;
    ~SoundCharacterGroupOwner() noexcept;
    SoundCharacterGroupOwner(const SoundCharacterGroupOwner&) = delete;
    SoundCharacterGroupOwner& operator=(const SoundCharacterGroupOwner&) = delete;
    [[nodiscard]] YtlReferenceSlotRuntime& Native() noexcept { return slot_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& Native() const noexcept { return slot_; }
    [[nodiscard]] SoundCharacterGroupNative* Get() const noexcept;
    void Serialize(SerializeBuffer& serializer);
    void Reset() noexcept;
private:
    YtlReferenceSlotRuntime slot_{};
};
static_assert(sizeof(SoundCharacterGroupOwner) == 12);

void ConfigureSoundCharacterGroupAt471FB0(SoundCharacterGroupOwner& destination,
    ISoundParamsSettingsSource& source, ISettingTagErrorSource& errors,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
// 46C2D0跳过Using=0角色，首个匹配即结束；master即使静音也先赋值。
[[nodiscard]] bool IsSoundCharacterAllowedAt46C2D0(SoundCharacterGroupNative* group,
    const char* resourceName, RuntimeCString* master);
// 两种查询都不检查角色Using/Display或注册静音，返回不同层次的真实Ref。
void FindSoundCharacterAt489630(YtlReferenceSlotRuntime& result, SoundCharacterGroupNative* group,
    const char* resourceName);
void FindSoundRegistrationAt489880(YtlReferenceSlotRuntime& result, SoundCharacterGroupNative* group,
    const char* resourceName);
// +B0/478B60：out尚未构造；先建立返回CString，再解析当前group Ref并查询角色。
void ConstructSoundCharacterNameAt478B60(RuntimeCString* out,
    const YtlReferenceSlotRuntime& group, const char* resourceName,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
