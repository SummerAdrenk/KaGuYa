#include "ReplaySceneNative.Internal.h"
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/RuntimeCStringInputNative.h>
#include <Runtime/ScrVmScriptCommandState.h>

namespace KaGuYa::Start::Runtime::ReplaySceneNativeDetail
{
namespace
{
const char* MissingReplay()noexcept
{return "\x83\x56\x83\x69\x83\x8A\x83\x49\x8D\xC4\x90\xB6\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49";}
const char* InvalidReplay()noexcept
{return "\x83\x56\x83\x69\x83\x8A\x83\x49\x8D\xC4\x90\xB6\x82\xCC\x90\xDD\x92\xE8\x82\xAA\x82\xA8\x82\xA9\x82\xB5\x82\xA2\x81\x49";}
}
void PrepareReplay(Body& body,ReplaySceneNativeServices& services)
{
    auto& source=services.source;
    auto* sound=Resolve<void>(source.RootSoundSlotAt24(Root(source,0x4B87B8)));
    source.StopMusicAt4383B0(sound,Read(Field<unsigned>(sound,0x10)));
    sound=Resolve<void>(source.RootSoundSlotAt24(Root(source,0x4B87F1)));source.ClearVoicesAt4384D0(sound);
    sound=Resolve<void>(source.RootSoundSlotAt24(Root(source,0x4B8827)));
    auto& voices=Field<EnvironmentVectorStorage<RuntimeCString>>(Registry(source,2,0x4B8855),0x30);
    source.StopMappedEffectsAt413270(sound,voices);
    const auto segment=Read(body.segmentAt18),item=Read(body.itemAt14),group=Read(body.groupAt0C);
    (void)Registry(source,4,0x4B887B);Slot selected;
    FindReplaySelectionNativeAt4ADAE0(selected,source,group,item,segment,services.memory);
    struct Selection{Slot& slot;bool active=true;~Selection()noexcept{if(active)ReleaseReplaySelectionNativeAt4A2F90(slot);}} selection{selected};
    if(!Read(selected.baseAt00))
    {
        auto* environment=source.CaptureEnvironmentAt4180C0(0x4B889C);source.ShowReplayError(environment,MissingReplay());
        body.phaseAt04=14;selection.active=false;ReleaseReplaySelectionNativeAt4A2F90(selected);return;
    }
    // 原4B8919前固定选择receiver，Params捕获后才读该receiver.fileIndex。
    auto* entry=Resolve<ReplaySelectionNativeBody8h>(selected);
    auto* params=source.CaptureParamsAt418480(0x4B8919);
    const auto* pattern=params->Pattern().get();const auto* name=source.PatternNameAt4AD1B0(pattern,Read(entry->fileIndexAt00));
    union Text{RuntimeCString value;Text(){}~Text(){}} text;
    ConstructRuntimeCStringInputNativeAt40B530(&text.value,name,services.buttons.strings);
    struct TextPrefix{RuntimeCString& value;bool active=true;~TextPrefix()noexcept{if(active)DestroyRuntimeCStringAt40CD40(&value);}} prefix{text.value};
    entry=Resolve<ReplaySelectionNativeBody8h>(selected);const auto* chars=text.value.GetString();
    source.EnsureRootAt413BE0(0x4B897A);
    const auto offset=Read(entry->entryAt04); // 原4B897F先于6198C4捕获，不合并为Root()。
    auto* root=source.RootAt6198C4(0x4B8982);
    (void)services.script.LoadStartupAt4D53D0(source.ScriptAt47CD00(root),chars,offset);
    const auto dispatched=source.ScriptAt47CD00(Root(source,0x4B899A));
    if(services.script.DispatchAt4D3FF0(dispatched)!=3)
    {
        auto* environment=source.CaptureEnvironmentAt4180C0(0x4B89B6);source.ShowReplayError(environment,InvalidReplay());body.phaseAt04=14;
    }
    else
    {
        const auto cleared=source.ScriptAt47CD00(Root(source,0x4B8A13));cleared.body->commandAt04.flags=0;cleared.body->commandAt04.reload[0]=0;
        cleared.publication->commandMask|=ScrVmScriptCommandState::FlagsAt04Mask|ScrVmScriptCommandState::ReloadByteAt08Mask;
        source.RestorePatternsAt4C4330(Draw(source,0x4B8A33));
        source.PropagatePatternsAt4C4250(Draw(source,0x4B8A58));
        source.DrawRegisteredAt4AC490(Draw(source,0x4B8A7D));
        CaptureScreen(body,source,0x4B8A99);source.ClearScreenAfter4182F0(0x4B8ACF,nullptr);
        SetRuntimeCStringAt437680(Field<RuntimeCString>(Scenes(source,0x4B8AE0),0x1C),"",0);
        ResetOperation(body.operationAt28);body.operationAt28.opcode=0;
        body.operationAt28.source=Resolve<GraphicsSurfaceNativeBody>(body.capturedAt68);
        body.operationAt28.previous=Read(Resolve<GraphicsSurfaceNativeBody>(body.blackAt5C)->handleAt04);body.stepAt58=16;
        body.savedModeAt08=Read(Field<unsigned>(Registry(source,2,0x4B8B32),4));
        Field<unsigned>(Registry(source,2,0x4B8B46),4)=0;++body.phaseAt04;
    }
    // 原normal Free时state已降到4；若Free抛出，只释放selection，不重试字符串。
    prefix.active=false;DestroyRuntimeCStringAt40CD40(&text.value);
    selection.active=false;ReleaseReplaySelectionNativeAt4A2F90(selected);
}
}
