#pragma once
#include <Runtime/TextWindowNativePlayback.h>
#include <Runtime/TextWindowEffectRestoreRuntime.h>
#include <Runtime/DrawLayersView.h>
#include <Runtime/InputRootNative.h>

namespace KaGuYa::Start::Runtime
{
class IReadScriptAdvanceCommandGateNativeSource;
class ITextWindowNativeMaintenanceSource;
class ITextWindowNativeAdvanceSource
{
public:
    virtual ~ITextWindowNativeAdvanceSource()=default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t caller)=0;
    virtual MouseManagerNativeBody3Ch* CaptureMouseAt418210(std::uint32_t caller)=0;
    virtual DrawLayersView CaptureLayersAfter413BE0(std::uint32_t caller)=0;
};
struct TextWindowNativeAdvanceServices final
{
    ITextWindowNativeAdvanceSource& source;
    ITextWindowNativePlaybackServices& playback;
    ITextWindowEffectRestoreSource& restore;
    ITextWindowSoundSource& sound;
    IReadScriptAdvanceCommandGateNativeSource& command;
    ITextWindowNativeMaintenanceSource& maintenance;
};
// 只消费传入同一E8h，不复制窗口/等待/语音状态；4BC260无caller EH或catch。
bool AdvanceTextWindowNativeAt4BC260(TextWindowLayerNativeBodyE8h&,std::uint32_t input,
    TextWindowNativeAdvanceServices&);
class RootTextWindowNativeAdvanceSource final:public ITextWindowNativeAdvanceSource
{
public:
    RootTextWindowNativeAdvanceSource(RootRuntimeOwner& root,EnvironmentRootOwner& environment,
        InputRootOwner& input,IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory())noexcept
        :root_(root),environment_(environment),input_(input),memory_(memory){}
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t)override;
    MouseManagerNativeBody3Ch* CaptureMouseAt418210(std::uint32_t)override;
    DrawLayersView CaptureLayersAfter413BE0(std::uint32_t)override;
private:
    RootRuntimeOwner& root_;EnvironmentRootOwner& environment_;InputRootOwner& input_;IEnvironmentRootMemory& memory_;
};
}
