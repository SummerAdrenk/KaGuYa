#pragma once
#include <Runtime/GuiDialogNative.h>
#include <Runtime/ToolWindowNative.h>

namespace KaGuYa::Start::Runtime
{
class IReadScriptSaveCaptureNativeSource
{
public:
    virtual ~IReadScriptSaveCaptureNativeSource()=default;
    virtual void EnsureRootAt413BE0(std::uint32_t caller)=0;
    virtual void* RootAt6198C4(std::uint32_t caller)=0;
    virtual DrawManagerNativeBody2Ch* DrawAt4B01D6(void* capturedRoot)=0;
    virtual YtlReferenceSlotRuntime* SelectToolAt417B50(ToolWindowNativeBody40h*,YtlReferenceSlotRuntime& fresh)=0;
    virtual void CapturePreviewAt48DBE0(void* capturedRoot)=0;
};
// 只按+28门控逐个调用child+08，不使用会改checkbox/enabled的532480。
void PrepareGuiChildrenForSaveNativeAt5325A0(GuiDialogNativeBody5Ch*);
// 4B01A0独立原生核：局部Dialog Ref先释放，再重新捕获Root采集预览。
void CaptureBeforeSaveNativeAt4B01A0(IReadScriptSaveCaptureNativeSource&);
}
