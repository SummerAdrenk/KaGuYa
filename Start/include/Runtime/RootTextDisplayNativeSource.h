#pragma once
#include <Runtime/TextDisplayNative.h>
#include <Runtime/GraphicsTextNativeApi.h>
#include <Runtime/ReadScriptWaitStateSource.h>
#include <Runtime/ScriptNamedValueNative.h>

namespace KaGuYa::Start::Runtime
{
class NativeGraphicsRuntimeLibrary;
class IStartFileApi;
class IMessageEnvironmentPathSource;
class IReadScriptEffectMaskNativeServices;
class ParamsRuntimeRootOwner;
class IEnvironmentArchiveRuntime;
struct TextDisplayFontContextNative14h final
{
    const LOGFONTA* fontAt00;
    std::int32_t heightAt04;
    std::uint32_t characterSpacingAt08;
    std::uint32_t lineSpacingAt0C;
    const COLORREF* colorAt10;
};
static_assert(sizeof(TextDisplayFontContextNative14h)==0x14);
const TextDisplayFontContextNative14h* TextDisplayFontContextsSample18At602198() noexcept;
void BuildTextDisplayFontNativeAt4A2113(GraphicsTextNativeFontParameters&,std::int32_t,
    const TextDisplayFontContextNative14h*);

struct RootTextDisplayNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    InputRootOwner& input;
    ParamsRuntimeRootOwner& params;
    NativeGraphicsRuntimeLibrary& graphics;
    IStartFileApi& files;
    const IMessageEnvironmentPathSource& messagePaths;
    IReadScriptEffectMaskNativeServices& masks;
    IScrVmCommandErrorSink& errors;
    ScriptNamedValueNativeTables names;
    IRuntimeStringManagerSource& strings;
    const TextDisplayFontContextNative14h* fonts;
};
// 构造只借进程级owner/平台叶；每个原调用点分别取源，不保活某代正文。
class RootTextDisplayNativeSource final : public ITextDisplayNativeSource,public IDrawLayersViewSource
{
public:
    explicit RootTextDisplayNativeSource(RootTextDisplayNativeDependencies) noexcept;
    DrawLayersView CaptureDrawLayersAt413BE0() override;
    std::int32_t ReadNamedAt4C73A0(const char*) override;
    bool CreateFontAt4A2113(std::int32_t) override;
    TextDisplayTextInitializationResult InitializeMessageAt4A2239(std::int32_t,MessageFormatSelectionRawTarget) override;
    bool LoadMaskAt4A26E4(YtlReferenceSlotRuntime&,std::int32_t) override;
    std::uint32_t ReadModeAt04() override;
    void WriteModeAt04(std::uint32_t) override;
    bool SuppressAt4C3430() override;
    bool AccelerateAt4AA970() override;
    bool ReadFlagAt14Bit1() override;
    bool ReadFlagAt18Bit1() override;
    std::uint32_t ReadPaceAt3C() override;
    std::int32_t ReadStepAt497900() override;
    bool ReadAdvanceAt4B0550() override;
    bool ReadNegativeWheelAt4AD740() override;
    bool ReadHeldSixAt4198D0() override;
    bool SuppressFrameAt4C3350() override;
    std::int32_t CaptureDirtyCountAt4B5CF9() override;
    const RECT* CaptureDirtyDataAt4B5D5C() override;
    void AdvanceSceneAt4C1760() override;
private:
    IEnvironmentArchiveRuntime* CaptureArchive(std::uint32_t);
    void* CaptureMessage();
    RootTextDisplayNativeDependencies dependencies_;
    RootReadScriptWaitStateSource wait_;
};
}
