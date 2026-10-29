#pragma once
#include <Runtime/GraphicsModuleNative.h>
#include <Runtime/SoundModuleNativeApi.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>

namespace KaGuYa::Start::Runtime
{
struct SoundManagerNativeSlot8h final
{
    std::int32_t handleAt00;
    union { RuntimeCString nameAt04; };
    SoundManagerNativeSlot8h() noexcept {}
    ~SoundManagerNativeSlot8h() noexcept {}
};
struct SoundLowerVolumeNative10h final
{
    std::uint32_t slotAt00;
    std::int32_t alternateAt04;
    // 45D3C3..45D3CB没有初始化+08；生产读当前正文，不加optional镜像。
    std::int32_t resolvedAt08;
    union { RuntimeCString relationAt0C; };
    SoundLowerVolumeNative10h() noexcept {}
    ~SoundLowerVolumeNative10h() noexcept {}
};
// 48AAFF申请的原50h正文；构造/463ED0拥有链由SoundManagerNativeLifecycle实现。
struct SoundManagerNativeBody50h final
{
    const void* vptrAt00;
    SoundManagerNativeSlot8h* slotsBeginAt04;
    SoundManagerNativeSlot8h* slotsEndAt08;
    SoundManagerNativeSlot8h* slotsCapacityAt0C;
    std::uint32_t musicAt10;
    std::uint32_t voicesBeginAt14;
    std::uint32_t firstEffectsBeginAt18;
    std::uint32_t secondEffectsBeginAt1C;
    std::uint32_t remainingEffectsBeginAt20;
    union { YtlReferenceSlotRuntime paramsAt24; };
    EnvironmentVectorStorage<YtlReferenceSlotRuntime> lowerAt30;
    std::uint32_t policyAt3C;
    GUID deviceAt40;
    SoundManagerNativeBody50h() noexcept {}
    ~SoundManagerNativeBody50h() noexcept {}
};
static_assert(sizeof(SoundManagerNativeSlot8h) == 8);
static_assert(sizeof(SoundLowerVolumeNative10h) == 0x10);
static_assert(offsetof(SoundLowerVolumeNative10h, relationAt0C) == 0x0C);
static_assert(sizeof(SoundManagerNativeBody50h) == 0x50);
static_assert(offsetof(SoundManagerNativeBody50h, paramsAt24) == 0x24);
static_assert(offsetof(SoundManagerNativeBody50h, lowerAt30) == 0x30);
static_assert(sizeof(decltype(SoundManagerNativeBody50h::lowerAt30)) == 12);
static_assert(offsetof(SoundManagerNativeBody50h, policyAt3C) == 0x3C);
static_assert(offsetof(SoundManagerNativeBody50h, deviceAt40) == 0x40);

EnvironmentSoundParamsArchive* ResolveSoundParamsAt418560(SoundManagerNativeBody50h&);

// 固定manager receiver，但Params/Sound每个原调用点独立捕获；依赖均借在正文外。
class SoundManagerNativeVolume final
{
public:
    SoundManagerNativeVolume(ParamsRuntimeRootOwner&, ISoundModuleNativeSource&,
        IRuntimeStringManagerSource&, IGraphicsModuleNativeWin32Api&) noexcept;
    bool AnyVoicePlayingAt4196C0(SoundManagerNativeBody50h&);
    std::uint8_t ReadRelationMaskAt419470(SoundManagerNativeBody50h&, const RuntimeCString* relation);
    void AdjustAt4C2410(SoundManagerNativeBody50h&);
    void RefreshAt433F60(SoundManagerNativeBody50h&);
private:
    std::int32_t ReadVoiceCount();
    bool IsPlaying(std::int32_t handle);
    void SetVolume(std::int32_t handle, std::int32_t value);
    ParamsRuntimeRootOwner& params_;
    SoundModuleNativeApi sound_;
};
}
