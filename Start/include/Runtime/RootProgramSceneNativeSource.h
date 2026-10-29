#pragma once
#include <Runtime/ProgramSceneNative.h>
#include <Runtime/ReadScriptInputNative.h>
#include <Runtime/NativeInputRuntime.h>
#include <Runtime/QuickOperationsNative.h>

namespace KaGuYa::Start::Runtime
{
class NativeGraphicsRuntimeLibrary;
class ISavePreviewCaptureServices;
class IScrVmCommandErrorSink;

struct RootProgramSceneNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    NativeInputRuntime& input;
    NativeGraphicsRuntimeLibrary& graphics;
    QuickOperationsNativeServices& quick;
    ISavePreviewCaptureServices& previewCapture;
    ProgramSelectDialogNativeServices& dialog;
    GeneralConfirmDialogNativeServices& confirmation;
    IScrVmCommandErrorSink& errors;
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
    ScriptNamedValueNativeTables names=GameScriptNamedValueTables();
    decltype(&::PostMessageA) postMessage=&::PostMessageA;
};

// 原20h之外的借用适配器。固定Root的槽、Params及服务均从同一个receiver取，
// 不在getter中重新Ensure，也不借ambient SceneManager来选择另一代对象图。
class RootProgramSceneNativeSource final:public IProgramSceneNativeSource
{
public:
    explicit RootProgramSceneNativeSource(RootProgramSceneNativeDependencies d)noexcept:dependencies_(d){}
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0(unsigned)override;
    void EnsureRootAt413BE0(unsigned)override;
    void* RootAt6198C4(unsigned)override;
    YtlReferenceSlotRuntime& RootDrawSlotAt18(void*,unsigned)override;
    YtlReferenceSlotRuntime& RootScenesSlotAt30(void*,unsigned)override;
    YtlReferenceSlotRuntime& RootInformationSlotAt3C(void*,unsigned)override;
    ScriptParamsNativeBody* ParamsAt47BDB0(void*,unsigned)override;
    ScriptNativeBody* ScriptAt47CD00(void*,unsigned)override;
    int ResolveNamedAt498020(void*,const char*,unsigned)override;
    int ReadValueAt4C73A0(ScriptParamsNativeBody*,int,unsigned)override;
    GuiDialogNativeBody5Ch* ConstructDialog00At4DE1E0(void*)override;
    GuiDialogNativeBody5Ch* ConstructDialog01At4DF0E0(void*)override;
    GuiDialogNativeBody5Ch* ConstructProfileDialog(void*, const ProgramModeProfile&) override;
    std::uint8_t InputPredicate(void*,unsigned,unsigned,unsigned)override;
    YtlReferenceSlotRuntime* CaptureScreenAt4ACDD0(void*,YtlReferenceSlotRuntime*,unsigned)override;
    void SubmitPreviewAt48CFF0(void*,const YtlReferenceSlotRuntime*,unsigned)override;
    void SetTransitionAt4C1C60(void*,YtlReferenceSlotRuntime,unsigned)override;
    void ResetDrawAt47E940(void*,void*,unsigned)override;
    void AdvanceScenesAt4C1760(void*,void*,unsigned)override;
    void PushSceneAt4A9030(void*,void*,const char*,unsigned)override;
    void ClearScenesAt4C1460(void*,void*,unsigned)override;
    void QueueSceneAt47F920(void*,void*,const char*,unsigned)override;
    void WriteRootByteAt1AF(void*,std::uint8_t,unsigned)override;
    void QuickSaveAt485FC0(void*,std::uint8_t,unsigned)override;
    std::uint8_t QuickLoadAt485CE0(void*,unsigned)override;
    GuiDialogNativeBody5Ch* ConstructConfirmAt4E3330(void*,int)override;
    void AssignConfirmAt48CA30(GuiDialogNativeBody5Ch*,YtlReferenceSlotRuntime,unsigned)override;
    int RunModalAt5071D0(void*,YtlReferenceSlotRuntime*,unsigned)override;
    BOOL PostMessage(HWND,UINT,WPARAM,LPARAM,unsigned)override;
private:
    RootProgramSceneNativeDependencies dependencies_;
};

// 外部GUI服务必须含真实四child，并长于场景及逃逸末Ref；本集合不暗建替身。
class RootProgramSceneServicesRuntime final
{
public:
    explicit RootProgramSceneServicesRuntime(RootProgramSceneNativeDependencies)noexcept;
    RootProgramSceneServicesRuntime(const RootProgramSceneServicesRuntime&)=delete;
    RootProgramSceneServicesRuntime& operator=(const RootProgramSceneServicesRuntime&)=delete;
    ProgramSceneNativeServices& Scene()noexcept{return scene_;}
    RootProgramSceneNativeSource& Source()noexcept{return source_;}
private:
    RootProgramSceneNativeSource source_;
    ProgramSceneNativeServices scene_;
};
}
