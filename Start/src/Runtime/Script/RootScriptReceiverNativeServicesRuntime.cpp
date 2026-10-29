#include <Runtime/RootScriptReceiverNativeServicesRuntime.h>
#include <Runtime/ScrVmRuntimeComposition.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/EnvironmentSceneRuntime.h>
#include <Runtime/EnvironmentUpdateErrorRuntime.h>
#include <Runtime/EnvironmentScriptRegistryState.h>
#include <Runtime/EnvironmentMusicNameRuntime.h>
#include <Runtime/NativeInputRuntime.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/NativeScriptSwitchBoundary.h>
#include <Runtime/NativeLayerWindowScrVmUpdateRuntime.h>
#include <Runtime/NativeLayerWindowScrVmTextRuntime.h>
#include <Runtime/NativeLayerWindowScrVmMenuRuntime.h>
#include <Runtime/ParamsResourceNameRuntime.h>
#include <Runtime/ParamsVoiceNameRuntime.h>
#include <Runtime/PatternLayerNativeResourceTarget.h>
#include <Runtime/PatternResourceLoadRuntime.h>
#include <Runtime/ProgramRuntimeServices.h>
#include <Runtime/RootGaugeRuntime.h>
#include <Runtime/RootSceneDemoNameRuntime.h>
#include <Runtime/RootSceneReplayRuntime.h>
#include <Runtime/RootScriptSoundOpenGate.h>
#include <Runtime/RootUpdateValueRuntime.h>
#include <Runtime/ScriptDateCommandNative.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/ScriptMenuSource.h>
#include <Runtime/ScriptMusicSource.h>
#include <Runtime/ScriptProgramMessageRuntime.h>
#include <Runtime/ScriptSaveCommandNative.h>
#include <Runtime/ScriptVoiceSource.h>

namespace KaGuYa::Start::Runtime
{
struct RootScriptReceiverNativeServicesRuntime::Impl final
{
    ParamsResourceNameRuntime names;
    ParamsVoiceNameRuntime voiceNames;
    ScriptExecutionRootSource pc;
    RootScriptControlCommandSource control;
    RootScriptArithmeticCommandSource arithmetic;
    RootScriptTitleCommandSource title;
    RootScriptFileCommandSource files;
    RootScriptDateCommandSource date;
    ParamsScriptSoundGroupSource groups;
    RootScriptSoundLeafSource soundLeaves;
    RootScriptSoundOpenGate soundGate;
    EnvironmentMusicNameRuntime musicNames;
    RootScriptMusicSource music;
    RootScriptVoiceSource voice;
    RootScriptMenuSource menu;
    RootProgramNamedValueRuntime named;
    RootSceneDemoNameRuntime currentScene;
    EnvironmentScriptRegistryState registry;
    RootScriptProgramMessageSource programMessages;
    RootUpdateValueRuntime updateValues;
    EnvironmentUpdateErrorRuntime updateErrors;
    EnvironmentSceneRuntime scene;
    RootSceneReplayRuntime replay;
    RootScriptLoadNativeSource loading;
    ScriptLoadNativeServices loadServices;
    NativeScriptSwitchBoundary switching;
    PatternResourceNativeInitializationSource initialization;
    PatternResourceLoadServices loads;
    PatternLayerNativeResourceServices resources;
    ScriptResultLayerNativeBindingServices results;
    ScrVmRuntimeStateBundle state;
    ScrScriptRuntimeState runtime;
    NativeLayerWindowScrVmUpdateRuntime update;
    NativeLayerWindowScrVmTextRuntime text;
    NativeLayerWindowScrVmMenuRuntime menuLayers;
    RootProgramGaugeRequests gauges;
    ProgramRuntimeServices program;
    RootScriptSaveCommandSource save;
    ScrVmRuntimeComposition composition;
    ScrVmNativeEntryContextRuntime context;
    RestoredScriptNativeExecutionRuntime execution;

    explicit Impl(const RootScriptReceiverNativeDependencies& d)
        :names(d.params),voiceNames(d.params),pc(d.root),control(d.root),arithmetic(d.root),title(d.root),
        files(d.root,d.environment,d.names,names,d.strings),date(d.root,d.environment,d.names,d.strings),
        groups(d.params),soundLeaves(d.environment.Archives(),d.root),
        soundGate(d.draws,d.input.RootSource(),d.environment.RootSource()),
        musicNames(d.environment),music(d.environment.Archives(),d.root),voice(d.root),
        menu(d.environment.Archives(),d.root),named(d.environment,d.root,d.names,d.messageBox,d.strings),
        currentScene(d.root),registry(d.environment.Archives()),programMessages(d.root),
        updateValues(d.environment,d.root,d.messageBox),updateErrors(d.environment,d.messageBox),
        scene(d.params,d.environment,d.messageBox),replay(d.environment,d.root,d.names,d.messageBox,d.strings),
        loading({d.params,d.root,d.environment.RootSource(),d.archives,d.files,d.key,d.codecs,d.readers,d.transaction,d.messageBox}),
        loadServices{loading,d.strings,ProcessScriptHeaderSource()},switching(loadServices),
        initialization(d.environment.RootSource()),
        loads{d.archives,d.transaction,d.files,d.readers,d.key,initialization,d.codecs,d.strings},
        resources{loads,d.graphics,d.graphics.RootSource(),d.graphics.Win32Source()},
        results{d.draws,d.params,d.environment.RootSource(),resources,d.strings,d.memory},
        state(d.body,d.publication,ScrVmParamsGenerationBinding::Borrowed(d.paramsSlot),d.memory,d.strings),
        runtime(state.CreateScriptView()),update(d.body,results,d.effects),
        text(update,names,groups,soundLeaves,soundGate),menuLayers(update),gauges(d.root),
        program(state.values,named,currentScene,registry.ProgramRegistry(),registry,&gauges),
        save(d.root,named,state.scriptCommand,runtime),
        composition(state,{pc,control,arithmetic,title,save},names,voiceNames,scene,replay,
            {switching,update,text,menuLayers,soundGate,program,musicNames,updateValues,date,files,
                groups,soundLeaves,music,voice,menu,programMessages},
            {d.errors,d.errors,d.errors,updateErrors,scene},d.random,d.memory,d.strings),
        context(composition.Core().BorrowNativeEntryContext(runtime)),
        execution(loadServices,pc,context,d.memory)
    {
        // 只验证借用身份，不取根、不登记原函数表、不初始化业务字段。
        state.RequireScriptView(runtime);
    }
};

RootScriptReceiverNativeServicesRuntime::RootScriptReceiverNativeServicesRuntime(
    RootScriptReceiverNativeDependencies dependencies):impl_(std::make_unique<Impl>(dependencies)){}
RootScriptReceiverNativeServicesRuntime::~RootScriptReceiverNativeServicesRuntime()=default;
IRestoredScriptNativeExecution& RootScriptReceiverNativeServicesRuntime::Execution() noexcept{return impl_->execution;}
ScrVmRuntimeStateBundle& RootScriptReceiverNativeServicesRuntime::State() noexcept{return impl_->state;}
ScrScriptRuntimeState& RootScriptReceiverNativeServicesRuntime::Runtime() noexcept{return impl_->runtime;}
const ScrVmCore& RootScriptReceiverNativeServicesRuntime::Core() const noexcept{return impl_->composition.Core();}
}
