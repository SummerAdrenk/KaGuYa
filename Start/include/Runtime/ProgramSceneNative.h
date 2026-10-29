#pragma once
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/GeneralConfirmDialogNative.h>
#include <Runtime/ScriptParamsNativeBody.h>
#include <Runtime/ScriptNativeBody.h>
#include <Runtime/ProgramSelectDialogNative.Scope.h>
#include <Runtime/ProgramResourceProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
struct ProgramSceneNativeBody20h;
struct ProgramSceneNativeVtable4 final
{
    std::uint8_t(__thiscall* runAt00)(ProgramSceneNativeBody20h*,int);
    ProgramSceneNativeBody20h*(__thiscall* deleteAt04)(ProgramSceneNativeBody20h*,unsigned);
    void(__thiscall* updateAt08)(ProgramSceneNativeBody20h*);
    void(__thiscall* drawAt0C)(ProgramSceneNativeBody20h*);
};
struct ProgramSceneNativeBody20h final
{
    const ProgramSceneNativeVtable4* vptrAt00;
    unsigned phaseAt04;
    YtlReferenceSlotRuntime dialogAt08;
    unsigned savedAt14;
    const ProgramModeProfile* profileAt18;
    int modeAt1C;
    ProgramSceneNativeBody20h()noexcept{}
};
static_assert(sizeof(ProgramSceneNativeBody20h)==0x20&&sizeof(ProgramSceneNativeVtable4)==0x10);
static_assert(offsetof(ProgramSceneNativeBody20h,dialogAt08)==8&&offsetof(ProgramSceneNativeBody20h,modeAt1C)==0x1C);

class IProgramSceneNativeSource
{
public:
    virtual ~IProgramSceneNativeSource()=default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0(unsigned site)=0;
    virtual void EnsureRootAt413BE0(unsigned site)=0;
    virtual void* RootAt6198C4(unsigned site)=0;
    virtual YtlReferenceSlotRuntime& RootDrawSlotAt18(void*,unsigned)=0;
    virtual YtlReferenceSlotRuntime& RootScenesSlotAt30(void*,unsigned)=0;
    virtual YtlReferenceSlotRuntime& RootInformationSlotAt3C(void*,unsigned)=0;
    virtual ScriptParamsNativeBody* ParamsAt47BDB0(void*,unsigned)=0;
    virtual ScriptNativeBody* ScriptAt47CD00(void*,unsigned)=0;
    virtual int ResolveNamedAt498020(void* archive,const char*,unsigned)=0;
    virtual int ReadValueAt4C73A0(ScriptParamsNativeBody*,int,unsigned)=0;
    virtual GuiDialogNativeBody5Ch* ConstructDialog00At4DE1E0(void*)=0;
    virtual GuiDialogNativeBody5Ch* ConstructDialog01At4DF0E0(void*)=0;
    virtual GuiDialogNativeBody5Ch* ConstructProfileDialog(void*, const ProgramModeProfile&)
    { throw std::logic_error("Configured Program dialog source is not bound"); }
    virtual std::uint8_t InputPredicate(void* capturedRoot,unsigned function,unsigned argument,unsigned site)=0;
    virtual YtlReferenceSlotRuntime* CaptureScreenAt4ACDD0(void* draw,YtlReferenceSlotRuntime*,unsigned)=0;
    virtual void SubmitPreviewAt48CFF0(void* root,const YtlReferenceSlotRuntime*,unsigned)=0;
    virtual void SetTransitionAt4C1C60(void* root,YtlReferenceSlotRuntime owned,unsigned)=0;
    virtual void ResetDrawAt47E940(void* capturedRoot,void* draw,unsigned)=0;
    virtual void AdvanceScenesAt4C1760(void* root,void* scenes,unsigned)=0;
    virtual void PushSceneAt4A9030(void* capturedRoot,void* scenes,const char*,unsigned)=0;
    virtual void ClearScenesAt4C1460(void* capturedRoot,void* scenes,unsigned)=0;
    virtual void QueueSceneAt47F920(void* capturedRoot,void* scenes,const char*,unsigned)=0;
    virtual void WriteRootByteAt1AF(void* root,std::uint8_t,unsigned)=0;
    virtual void QuickSaveAt485FC0(void* unusedScenes,std::uint8_t,unsigned)=0;
    virtual std::uint8_t QuickLoadAt485CE0(void* unusedScenes,unsigned)=0;
    virtual GuiDialogNativeBody5Ch* ConstructConfirmAt4E3330(void* raw,int mode)=0;
    // 以下按值参数由callee正常/异常均消费；caller仅在进入callee前持展开资格。
    virtual void AssignConfirmAt48CA30(GuiDialogNativeBody5Ch*,YtlReferenceSlotRuntime owned,unsigned)=0;
    virtual int RunModalAt5071D0(void* unusedInformation,YtlReferenceSlotRuntime* borrowed,unsigned)=0;
    virtual BOOL PostMessage(HWND,UINT,WPARAM,LPARAM,unsigned)=0;
};
struct ProgramSceneNativeServices final
{
    IProgramSceneNativeSource& source;
    IEnvironmentRootMemory& memory;
    ProgramSelectDialogNativeServices* gui=nullptr;
    GeneralConfirmDialogNativeServices* confirmation=nullptr;
    const ProgramResourceProfile* resources=nullptr;
};
class ProgramSceneNativeScope final
{
public:
    explicit ProgramSceneNativeScope(ProgramSceneNativeServices&)noexcept;
    ~ProgramSceneNativeScope()noexcept;
    ProgramSceneNativeScope(const ProgramSceneNativeScope&)=delete;
    ProgramSceneNativeScope& operator=(const ProgramSceneNativeScope&)=delete;
private:
    ProgramSceneNativeServices* previous_;
    GeneralConfirmDialogNativeScope confirmation_;
    ProgramSelectDialogNativeScope gui_;
};
ProgramSceneNativeServices& RequireProgramSceneNativeServices();
const ProgramSceneNativeVtable4& ProgramSceneNativeMethods()noexcept;
ProgramSceneNativeBody20h* ConstructProgramSceneNativeAt4A11E0(ProgramSceneNativeBody20h&,ProgramSceneNativeServices&);
void UpdateProgramSceneNativeAt4B9710(ProgramSceneNativeBody20h&,ProgramSceneNativeServices&);
void DestroyProgramSceneNativeAt4A6ED0(ProgramSceneNativeBody20h&)noexcept;
ProgramSceneNativeBody20h* DeleteProgramSceneNativeAt4A6ED0(ProgramSceneNativeBody20h*,unsigned)noexcept;
const char* ProgramSceneNativeName(unsigned address);
}
