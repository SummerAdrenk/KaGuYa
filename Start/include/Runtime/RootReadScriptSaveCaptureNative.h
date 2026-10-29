#pragma once
#include <Runtime/ReadScriptSaveCaptureNative.h>
#include <Runtime/SavePreviewCaptureApi.h>

namespace KaGuYa::Start::Runtime
{
class NativeGraphicsRuntimeLibrary;
struct ToolWindowNativeInteractionServices;
struct RootReadScriptSaveCaptureNativeDependencies final
{
    RootRuntimeOwner& root;
    NativeGraphicsRuntimeLibrary& graphics;
    ToolWindowNativeInteractionServices& tool;
    ISavePreviewCaptureServices& preview;
};
class RootReadScriptSaveCaptureNativeSource final:public IReadScriptSaveCaptureNativeSource
{
public:
    explicit RootReadScriptSaveCaptureNativeSource(RootReadScriptSaveCaptureNativeDependencies d)noexcept:dependencies_(d){}
    void EnsureRootAt413BE0(std::uint32_t)override;
    void* RootAt6198C4(std::uint32_t)override;
    DrawManagerNativeBody2Ch* DrawAt4B01D6(void*)override;
    YtlReferenceSlotRuntime* SelectToolAt417B50(ToolWindowNativeBody40h*,YtlReferenceSlotRuntime&)override;
    void CapturePreviewAt48DBE0(void*)override;
private:
    RootReadScriptSaveCaptureNativeDependencies dependencies_;
};
}
