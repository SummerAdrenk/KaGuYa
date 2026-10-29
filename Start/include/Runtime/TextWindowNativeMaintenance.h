#pragma once

#include <Runtime/TextWindowSoundSource.h>

namespace KaGuYa::Start::Runtime
{
struct TextWindowLayerNativeBodyE8h;
class ITextWindowNativePlaybackServices;

// caller 为原内联段首址或413BE0的CALL地址；三个叶均必需，无固定正文/成功兜底。
class ITextWindowNativeMaintenanceSource
{
public:
    virtual ~ITextWindowNativeMaintenanceSource() = default;
    virtual EnvironmentRootStorage* CaptureEnvironmentInline(std::uint32_t caller) = 0;
    virtual TextSoundRootReceiver CaptureSoundRootAfter413BE0(std::uint32_t caller) = 0;
    virtual bool PreparePlaybackAt4C2A40(TextWindowLayerNativeBodyE8h&) = 0;
};

// 只借三个活来源；Save/UnSave/参数/声音manager均由本次正文在原读点解析。
class RootTextWindowNativeMaintenanceSource final : public ITextWindowNativeMaintenanceSource
{
public:
    RootTextWindowNativeMaintenanceSource(EnvironmentRootOwner&, ITextWindowSoundSource&,
        ITextWindowNativePlaybackServices&) noexcept;
    EnvironmentRootStorage* CaptureEnvironmentInline(std::uint32_t caller) override;
    TextSoundRootReceiver CaptureSoundRootAfter413BE0(std::uint32_t caller) override;
    bool PreparePlaybackAt4C2A40(TextWindowLayerNativeBodyE8h&) override;
private:
    EnvironmentRootOwner& environment_;
    ITextWindowSoundSource& sound_;
    ITextWindowNativePlaybackServices& playback_;
};

// 4C0A70无catch/finally：任一叶异常截断，只有正常控制流到达433F60。
void MaintainTextWindowVoiceNativeAt4C0A70(
    TextWindowLayerNativeBodyE8h&, ITextWindowNativeMaintenanceSource&);
}
