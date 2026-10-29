#pragma once
#include <Runtime/SoundManagerNativeVolume.h>

namespace KaGuYa::Start::Runtime
{
// 42BC90资源打开单列为必需依赖；无默认成功实现，不借现代slot镜像。
class ISoundManagerNativeOpen
{
public:
    virtual ~ISoundManagerNativeOpen() = default;
    virtual bool OpenAt42BC90(SoundManagerNativeBody50h&, std::uint32_t slot,
        const char* heldName, std::int32_t value, std::int32_t loop, std::uint8_t noAutoPlay) = 0;
};

// 直接借50h正文和names Ref12；不增持或复制整个输入数组。
class SoundManagerNativePlayback final
{
public:
    SoundManagerNativePlayback(ParamsRuntimeRootOwner&, SoundModuleNativeApi&) noexcept;
    bool StopAndClearAt4384D0(SoundManagerNativeBody50h&);
    bool PlaySingleAt4BCE10(SoundManagerNativeBody50h&, const char* capturedName,
        ISoundManagerNativeOpen&, IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource());
    bool PlayBatchAt42C4F0(SoundManagerNativeBody50h&, const YtlReferenceSlotRuntime& names,
        ISoundManagerNativeOpen&);
private:
    ParamsRuntimeRootOwner& params_;
    SoundModuleNativeApi& sound_;
};
// 各原调用点独立捕获；body不保存现代GS数量镜像。
std::int32_t ReadSoundVoiceCountNative(ParamsRuntimeRootOwner&);
}
