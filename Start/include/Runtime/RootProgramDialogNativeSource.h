#pragma once
#include <Runtime/ProgramSelectDialogNative.h>
#include <Runtime/RootSceneManagerNativeServicesRuntime.h>
#include <Runtime/ScriptNamedValueNative.h>
#include <Runtime/ProgramControlSoundNativeGlobals.h>

namespace KaGuYa::Start::Runtime
{
class IScrVmCommandErrorSink;
struct ProgramControlSoundNativeNames final
{
    const RuntimeCString& confirmAt61A658;
    const RuntimeCString& hoverAt61A65C;
    const RuntimeCString& increaseAt61A670;
    const RuntimeCString& decreaseAt61A674;
};
struct RootProgramDialogNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    NativeInputRuntime& input;
    NativeGraphicsRuntimeLibrary& graphics;
    IGuiControlResourceLoadServices& pictures;
    IScrVmCommandErrorSink& errors;
    ProgramControlSoundNativeNames cues{
        ProcessProgramControlSoundNative(ProgramControlSoundNativeId::Confirm),
        ProcessProgramControlSoundNative(ProgramControlSoundNativeId::Hover),
        ProcessProgramControlSoundNative(ProgramControlSoundNativeId::Increase),
        ProcessProgramControlSoundNative(ProgramControlSoundNativeId::Decrease)};
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
    ScriptNamedValueNativeTables names=GameScriptNamedValueTables();
};
// 全部业务留在原EC/C0/88/68正文；此处仅接同根Env/输入/声音/图形叶。
class RootProgramDialogNativeSource final:public IProgramSelectDialogNativeSource,
    public IProgramMain00DialogNativeSource,public IProgramSystemDialogNativeSource
{
public:
    explicit RootProgramDialogNativeSource(RootProgramDialogNativeDependencies d)noexcept:dependencies_(d){}
    void BindChildren(ProgramMain00DialogNativeServices&,ProgramMain01DialogNativeServices&,ProgramSystemDialogNativeServices&)noexcept;
    void SetProgramModalFlag(bool) override;
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0(unsigned)override;
    void* RegistryAt417990(EnvironmentRootStorage*,unsigned,unsigned)override;
    void EnsureRootAt413BE0(unsigned)override;
    void* RootAt6198C4(unsigned)override;
    YtlReferenceSlotRuntime& RootDrawSlotAt18(void*)override;
    YtlReferenceSlotRuntime& RootSoundSlotAt24(void*)override;
    ScriptParamsNativeBody* ParamsAt47BDB0(void*,unsigned)override;
    int ResolveNamedAt498020(void*,const char*,unsigned)override;
    int ReadValueAt4C73A0(ScriptParamsNativeBody*,int,unsigned)override;
    void WriteValueAt4C8220(ScriptParamsNativeBody*,int,int,unsigned)override;
    std::uint8_t LoadSurfaceAt480000(void*,YtlReferenceSlotRuntime*,const char*,unsigned)override;
    void* GraphicsAt4182F0(unsigned)override;
    POINT* ScreenSizeAt47CC00(void*,POINT*,unsigned)override;
    std::uint8_t InputAt4C3430(void*,unsigned)override;
    std::uint8_t InputAt4AA970(void*,unsigned)override;
    const char* TimeSoundNameAt61A664(unsigned)override;
    const int* BgmVolumeAtVslot14(void*)override;
    std::uint8_t PlayBgmAt42BFE0(void*,const char*,int,int,unsigned)override;
    std::uint8_t ClearSoundAt4384D0(void*,unsigned)override;
    void StopSoundAt413270(void*,EnvironmentVectorStorage<RuntimeCString>*,unsigned)override;
    void CommitAt4B02E0(void*,int,unsigned)override;
    GuiDialogNativeBody5Ch* ConstructMain00At4DCEB0(void*,GuiDialogNativeBody5Ch*,int)override;
    GuiDialogNativeBody5Ch* ConstructMain01At4DE520(void*,GuiDialogNativeBody5Ch*,int)override;
    GuiDialogNativeBody5Ch* ConstructSystem00At4DF420(void*,GuiDialogNativeBody5Ch*,int)override;
    GuiDialogNativeBody5Ch* ConstructSystem01At4DFCA0(void*,GuiDialogNativeBody5Ch*,int)override;
    void EnsureEnvironmentInline(unsigned)override;
    EnvironmentRootStorage* EnvironmentAt6198C8(unsigned)override;
    InputRootNativeBody24h* InputAt418210(unsigned)override;
    const char* SoundNameAt61A6xx(unsigned)override;
    const int* SeVolumeAtVslot24(void*)override;
    std::uint8_t PlaySeAt4BCD70(void*,const char*,int,int,std::uint8_t,unsigned)override;
    int PollClickedEventManagerAt50FDB0(GuiClickedEventReferences*,int,unsigned)override;
    YtlReferenceSlotRuntime& RootProgramSlotAt30(void*)override;
private:
    RootProgramDialogNativeDependencies dependencies_;
    ProgramMain00DialogNativeServices* main00_=nullptr;
    ProgramMain01DialogNativeServices* main01_=nullptr;
    ProgramSystemDialogNativeServices* system_=nullptr;
};
class RootProgramDialogServicesRuntime final
{
public:
    explicit RootProgramDialogServicesRuntime(RootProgramDialogNativeDependencies)noexcept;
    RootProgramDialogServicesRuntime(const RootProgramDialogServicesRuntime&)=delete;
    RootProgramDialogServicesRuntime& operator=(const RootProgramDialogServicesRuntime&)=delete;
    ProgramSelectDialogNativeServices& Dialog()noexcept{return dialog_;}
    RootProgramDialogNativeSource& Source()noexcept{return source_;}
private:
    RootProgramDialogNativeSource source_;
    GuiModalInputSource input_;
    RootSceneGuiNativeScreenSource screen_;
    ProgramSelectDialogNativeServices dialog_;
    ProgramMain00DialogNativeServices main00_;
    ProgramMain01DialogNativeSharedSource main01Source_;
    ProgramMain01DialogNativeServices main01_;
    ProgramSystemDialogNativeServices system_;
};
}
