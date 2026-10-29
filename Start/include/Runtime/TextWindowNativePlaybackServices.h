#pragma once
#include <Runtime/TextWindowNativePlayback.h>
#include <Runtime/InputRootNative.h>
#include <Runtime/TextWindowSoundSource.h>

namespace KaGuYa::Start::Runtime
{
// 固定本次root+24解址所得receiver；三个声音叶均没有默认实现。
// 这是正文外装配边界，不是CSoundManager布局，也不能以现代槽镜像冒充原正文。
class TextWindowNativeSoundReceiver final
{
public:
    using PlayFunction = bool (*)(void*, const YtlReferenceSlotRuntime*);
    using VoidFunction = void (*)(void*);
    TextWindowNativeSoundReceiver(void* p, PlayFunction play, VoidFunction adjust, VoidFunction refresh) noexcept
        : callbacks_{p, play, adjust, refresh}, bound_(false) {}
    explicit TextWindowNativeSoundReceiver(TextSoundManagerReceiver receiver) noexcept : manager_(receiver), bound_(true) {}
    bool Play(const YtlReferenceSlotRuntime* names) const
    { return bound_ ? manager_.voice.PlayBatchRawAt42C4F0(names) : callbacks_.play(callbacks_.object, names); }
    void Adjust() const
    { if (bound_) manager_.volume.Adjust(); else callbacks_.adjust(callbacks_.object); }
    void Refresh() const
    { if (bound_) manager_.volume.Refresh(); else callbacks_.refresh(callbacks_.object); }
private:
    struct Callbacks { void* object; PlayFunction play; VoidFunction adjust; VoidFunction refresh; };
    // 两种显式构造来源，不在失败后切换；按值捕获，不存mutable Source镜像或指向返回临时。
    union { Callbacks callbacks_; TextSoundManagerReceiver manager_; };
    bool bound_;
};
class ITextWindowNativePlaybackSource
{
public:
    virtual ~ITextWindowNativePlaybackSource() = default;
    virtual TextWindowNativeSoundReceiver CaptureSoundAfter413BE0() = 0;
    // 4C3140..4C315D是Ref12按值的原始varargs，不增持、不展开成现代字符串名列表。
    virtual void FormatVoiceFailureAt417130(RuntimeCString&, YtlReferenceSlotRuntime names) = 0;
    virtual int ShowMessageBoxA(HWND, const char* text, const char* caption, UINT) = 0;
    virtual void ClearRootUpdateEnabledAt1B0After413BE0() = 0;
};

// TextDisplay和TextWin共用原4B0550；只借同一Click和独立Input来源。
bool ReadAdvanceInputNativeAt4B0550(ClickInfoArchive&,InputRootOwner&);

// 4C2A40和三个输入叶借实际E8h/Environment/Input24h；不持有第二份文字或语音状态。
class TextWindowNativePlaybackServices final : public ITextWindowNativePlaybackServices
{
public:
    TextWindowNativePlaybackServices(EnvironmentRootOwner&, InputRootOwner&,
        ITextWindowNativePlaybackSource&, IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource()) noexcept;
    bool PreparePlaybackAt4C2A40(TextWindowLayerNativeBodyE8h&) override;
    bool ReadAdvanceInputAt4B0550(ClickInfoArchive&) override;
    bool IsPacedMouseAt4BB5D1() override;
    bool ReadSkipAt4AA540(TextWindowLayerNativeBodyE8h&) override;
    void ClearRootUpdateEnabledAt1B0After413BE0() override;
private:
    void ShowVoiceFailure();
    EnvironmentRootOwner& environment_;
    InputRootOwner& input_;
    ITextWindowNativePlaybackSource& source_;
    IRuntimeStringManagerSource& strings_;
};
}
