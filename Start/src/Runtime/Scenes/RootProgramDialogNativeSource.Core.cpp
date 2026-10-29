#include <Runtime/RootProgramDialogNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/ReadScriptInputNative.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
void RootProgramDialogNativeSource::SetProgramModalFlag(bool active)
{
    auto* root = dependencies_.root.ReadPublishedAt6198C4();
    if (!root) throw std::logic_error("Program modal requires a published root");
    root->Frame().Lifecycle().valueAt1B8 = active ? 1u : 0u;
}
void RootProgramDialogNativeSource::BindChildren(ProgramMain00DialogNativeServices& main00,
    ProgramMain01DialogNativeServices& main01,ProgramSystemDialogNativeServices& system)noexcept
{main00_=&main00;main01_=&main01;system_=&system;}
EnvironmentRootStorage* RootProgramDialogNativeSource::CaptureEnvironmentAt4180C0(unsigned)
{return dependencies_.environment.CaptureAt4180C0();}
void* RootProgramDialogNativeSource::RegistryAt417990(EnvironmentRootStorage* env,unsigned index,unsigned)
{return ResolveEnvironmentArchiveAt417990(*env,index);}
void RootProgramDialogNativeSource::EnsureRootAt413BE0(unsigned){dependencies_.root.EnsureCreatedAt413BE0();}
void* RootProgramDialogNativeSource::RootAt6198C4(unsigned){return dependencies_.root.ReadPublishedAt6198C4();}
YtlReferenceSlotRuntime& RootProgramDialogNativeSource::RootDrawSlotAt18(void* root)
{return static_cast<RootRuntimePayload*>(root)->DrawManagerSlotAt18();}
YtlReferenceSlotRuntime& RootProgramDialogNativeSource::RootSoundSlotAt24(void* root)
{return static_cast<RootRuntimePayload*>(root)->SoundManagerSlotAt24();}
ScriptParamsNativeBody* RootProgramDialogNativeSource::ParamsAt47BDB0(void* root,unsigned)
{return &static_cast<RootRuntimePayload*>(root)->ParamsAt47BDB0();}
int RootProgramDialogNativeSource::ResolveNamedAt498020(void*,const char* name,unsigned)
{return ResolveScriptNamedValueNativeAt498020(name,dependencies_.names,dependencies_.strings);}
int RootProgramDialogNativeSource::ReadValueAt4C73A0(ScriptParamsNativeBody* params,int index,unsigned)
{return ReadScriptValueAt4C73A0(*params,index,dependencies_.errors);}
void RootProgramDialogNativeSource::WriteValueAt4C8220(ScriptParamsNativeBody* params,int index,int value,unsigned)
{WriteScriptValueAt4C8220(*params,index,value,dependencies_.errors);}
std::uint8_t RootProgramDialogNativeSource::InputAt4C3430(void*,unsigned)
{return ReadScriptSuppressInputAt4C3430(dependencies_.environment,dependencies_.input.RootSource());}
std::uint8_t RootProgramDialogNativeSource::InputAt4AA970(void*,unsigned)
{return ReadScriptAccelerateInputAt4AA970(dependencies_.input.RootSource());}
void RootProgramDialogNativeSource::EnsureEnvironmentInline(unsigned)
{static_cast<void>(dependencies_.environment.CaptureAt4180C0());}
EnvironmentRootStorage* RootProgramDialogNativeSource::EnvironmentAt6198C8(unsigned)
{return *static_cast<EnvironmentRootStorage* const volatile*>(&dependencies_.environment.PublicationSlot());}
InputRootNativeBody24h* RootProgramDialogNativeSource::InputAt418210(unsigned)
{return dependencies_.input.CaptureAt418210();}
int RootProgramDialogNativeSource::PollClickedEventManagerAt50FDB0(GuiClickedEventReferences* manager,int clear,unsigned)
{return PollGuiClickedEventManagerNativeAt50FDB0(manager,clear);}
YtlReferenceSlotRuntime& RootProgramDialogNativeSource::RootProgramSlotAt30(void* root)
{return static_cast<RootRuntimePayload*>(root)->SceneManagerSlotAt30();}
GuiDialogNativeBody5Ch* RootProgramDialogNativeSource::ConstructMain00At4DCEB0(void* raw,GuiDialogNativeBody5Ch* parent,int id)
{
    if(!main00_)throw std::logic_error("Program Main00 native services not bound");
    return &ConstructProgramMain00DialogNativeAt4DCEB0(raw,parent,id,*main00_)->common;
}
GuiDialogNativeBody5Ch* RootProgramDialogNativeSource::ConstructMain01At4DE520(void* raw,GuiDialogNativeBody5Ch* parent,int id)
{
    if(!main01_)throw std::logic_error("Program Main01 native services not bound");
    return &ConstructProgramMain01DialogNativeAt4DE520(raw,parent,id,*main01_)->common;
}
GuiDialogNativeBody5Ch* RootProgramDialogNativeSource::ConstructSystem00At4DF420(void* raw,GuiDialogNativeBody5Ch* parent,int id)
{
    if(!system_)throw std::logic_error("Program System native services not bound");
    return &ConstructProgramSystem00DialogNativeAt4DF420(raw,parent,id,*system_)->common;
}
GuiDialogNativeBody5Ch* RootProgramDialogNativeSource::ConstructSystem01At4DFCA0(void* raw,GuiDialogNativeBody5Ch* parent,int id)
{
    if(!system_)throw std::logic_error("Program System native services not bound");
    return &ConstructProgramSystem01DialogNativeAt4DFCA0(raw,parent,id,*system_)->common;
}
RootProgramDialogServicesRuntime::RootProgramDialogServicesRuntime(RootProgramDialogNativeDependencies d)noexcept
    :source_(d),input_(d.input),screen_(d.graphics),
     dialog_{source_,d.graphics,d.graphics.GuiCalls(),input_,screen_,d.graphics,d.strings},
     main00_{dialog_,source_,d.pictures},main01Source_(source_),main01_{main01Source_,dialog_,d.pictures},
     system_{source_,dialog_,d.pictures}
{
    source_.BindChildren(main00_,main01_,system_);
    dialog_.main00=&main00_;dialog_.main01=&main01_;dialog_.system=&system_;
}
}
