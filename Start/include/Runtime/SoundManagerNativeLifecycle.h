#pragma once
#include <Runtime/SoundManagerNativeVolume.h>

namespace KaGuYa::Start::Runtime
{
struct SoundManagerNativeVtable final
{
    SoundManagerNativeBody50h* (__thiscall* deleteAt00)(SoundManagerNativeBody50h*, std::uint32_t);
};
static_assert(sizeof(SoundManagerNativeVtable) == 4);

// 原正文/control不含服务字段；与GraphicsModule相同的外部调用期scope，嵌套恢复。
class SoundManagerNativeScope final
{
public:
    explicit SoundManagerNativeScope(IEnvironmentRootMemory&) noexcept;
    ~SoundManagerNativeScope() noexcept;
    SoundManagerNativeScope(const SoundManagerNativeScope&) = delete;
    SoundManagerNativeScope& operator=(const SoundManagerNativeScope&) = delete;
private:
    IEnvironmentRootMemory* previous_;
};
const SoundManagerNativeVtable& SoundManagerNativeMethods() noexcept;

class ISoundManagerSavedParamsSource
{
public:
    virtual ~ISoundManagerSavedParamsSource() = default;
    virtual const YtlReferenceSlotRuntime* BorrowSoundParamsAt48AB24() = 0;
};
class SoundManagerSavedParamsSource final : public ISoundManagerSavedParamsSource
{
public:
    explicit SoundManagerSavedParamsSource(EnvironmentRootOwner& root) noexcept : root_(root) {}
    const YtlReferenceSlotRuntime* BorrowSoundParamsAt48AB24() override;
private:
    EnvironmentRootOwner& root_;
};

SoundManagerNativeBody50h* ConstructSoundManagerAt45D120(SoundManagerNativeBody50h&,
    const YtlReferenceSlotRuntime&, ParamsRuntimeRootOwner&, SoundModuleNativeApi&,
    IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& = DefaultEnvironmentRootMemory());
// 构造和432C20均直接CALL42CA60；只重建既有槽handle，不resize/重置名字或其它正文。
void InitializeSoundHandlesNativeAt42CA60(SoundManagerNativeBody50h&,ParamsRuntimeRootOwner&,SoundModuleNativeApi&);
SoundManagerNativeBody50h* CreateSoundManagerAt48AAFF(ISoundManagerSavedParamsSource&,
    ParamsRuntimeRootOwner&, SoundModuleNativeApi&,
    IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& = DefaultEnvironmentRootMemory());
void DestroySoundManagerAt463ED0(SoundManagerNativeBody50h&,
    IEnvironmentRootMemory& = DefaultEnvironmentRootMemory()) noexcept;
SoundManagerNativeBody50h* DeleteSoundManagerAt469480(SoundManagerNativeBody50h&, std::uint32_t flags,
    IEnvironmentRootMemory& = DefaultEnvironmentRootMemory()) noexcept;
void InstallSoundManagerReferenceAt48AB35(YtlReferenceSlotRuntime&, SoundManagerNativeBody50h*,
    IEnvironmentRootMemory& = DefaultEnvironmentRootMemory());
void ReleaseSoundManagerReference(YtlReferenceSlotRuntime&,
    IEnvironmentRootMemory& = DefaultEnvironmentRootMemory());

// 10h正文、独立18h控制块；写入先后/未初始化+08在构造入口保留。
SoundLowerVolumeNative10h* CreateSoundLowerAt45D394(SoundManagerNativeBody50h&, std::uint32_t iteration,
    IRuntimeStringManagerSource&, IEnvironmentRootMemory&);
void BuildSoundLowerReferenceAt45D3E7(YtlReferenceSlotRuntime&, SoundLowerVolumeNative10h*, IEnvironmentRootMemory&);
void ReleaseSoundLowerCapturedAt45D469(YtlReferenceSlotRuntime&, YtlReferenceControlRuntime*) noexcept;
}
