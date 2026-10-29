#pragma once
#include <Runtime/SelectWindowNative.h>
#include <Runtime/GraphicsTextNativeApi.h>
#include <Runtime/MessageFormatSelectionRuntime.h>
#include <Runtime/InputRootNative.h>
#include <Runtime/TextWindowSoundSource.h>
#include <Runtime/ScriptNamedValueNative.h>

namespace KaGuYa::Start::Runtime
{
struct ScriptNativeBody;
struct ScriptParamsNativeBody;
class IScrVmCommandErrorSink;
class ScrVmExecutableProfile;
class IMessageEnvironmentPathSource;
class IStartFileApi;

// 每次调用表达一个原捕获点；不缓存message、CParams、script或鼠标正文。
class ISelectWindowNativeWorkflowSource
{
public:
    virtual ~ISelectWindowNativeWorkflowSource() = default;
    virtual ScriptParamsNativeBody* CaptureParamsAfter413BE0() = 0;
    virtual float ReadMessageVersionAfter413BE0() = 0;
    virtual std::int32_t ReadChoiceAfter413BE0(std::int32_t id, MessageFormatSelectionRawTarget) = 0;
    virtual std::int32_t ReadLegacyAfter413BE0(std::int32_t id, MessageFormatSelectionRawTarget) = 0;
    virtual ScriptNativeBody* CaptureScriptAfter413BE0() = 0;
    virtual std::int32_t ReadProgramFlagAfterRoot(std::uint32_t ordinal) = 0;
    virtual MouseManagerNativeBody3Ch* CaptureMouseAt418210() = 0;
    virtual void MarkChoiceAfter413BE0(std::int32_t id) = 0;
};
class RootSelectWindowNativeWorkflowSource final : public ISelectWindowNativeWorkflowSource
{
public:
    RootSelectWindowNativeWorkflowSource(RootRuntimeOwner&, EnvironmentRootOwner&, InputRootOwner&,
        ScriptNamedValueNativeTables, IScrVmCommandErrorSink&, IStartFileApi&, const IMessageEnvironmentPathSource&,
        IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource()) noexcept;
    ScriptParamsNativeBody* CaptureParamsAfter413BE0() override;
    float ReadMessageVersionAfter413BE0() override;
    std::int32_t ReadChoiceAfter413BE0(std::int32_t, MessageFormatSelectionRawTarget) override;
    std::int32_t ReadLegacyAfter413BE0(std::int32_t, MessageFormatSelectionRawTarget) override;
    ScriptNativeBody* CaptureScriptAfter413BE0() override;
    std::int32_t ReadProgramFlagAfterRoot(std::uint32_t) override;
    MouseManagerNativeBody3Ch* CaptureMouseAt418210() override;
    void MarkChoiceAfter413BE0(std::int32_t) override;
private:
    RootRuntimeOwner& root_;
    EnvironmentRootOwner& environment_;
    InputRootOwner& input_;
    ScriptNamedValueNativeTables names_;
    IScrVmCommandErrorSink& errors_;
    IStartFileApi& files_;
    const IMessageEnvironmentPathSource& paths_;
    IRuntimeStringManagerSource& strings_;
};
struct SelectWindowNativeWorkflowServices final
{
    ISelectWindowNativeWorkflowSource& source;
    EnvironmentRootOwner& environment;
    IGraphicsWindowSurfaceApi& drawing;
    GraphicsTextNativeApi& text;
    const char* const& fontFaceAt619824;
    ITextWindowSoundSource& sound;
};
// 4375BA只吞业务异常，不回滚已写画布/dirty/saved byte；正常临时Ref退役仍可terminate。
void RebuildSelectWindowNativeAt4369E0(SelectWindowNativeBody6Ch&, SelectWindowNativeWorkflowServices&);
std::int32_t TickSelectWindowNativeAt4BBFD0(SelectWindowNativeBody6Ch&, std::uint8_t blocked,
    SelectWindowNativeWorkflowServices&);
}
