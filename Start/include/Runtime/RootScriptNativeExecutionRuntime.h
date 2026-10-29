#pragma once
#include <Runtime/RootScriptReceiverNativeServicesRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 长期服务不包含某代Script/Params；首次调用由caller交付已选定payload/body。
struct RootScriptNativeExecutionDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentStartupRuntime& environment;
    ParamsRuntimeRootOwner& params;
    NativeInputRuntime& input;
    NativeGraphicsRuntimeLibrary& graphics;
    IDrawManagerNativeRootSource& draws;
    ILayerWindowEffectNativeSource& effects;
    IArchiveResourceSource& archives;
    IStartFileApi& files;
    const IEnvironmentKeyTableSource& key;
    ArchiveCodecServices codecs;
    IArchiveReaderMemoryApi& readers;
    IScrVmProgramRandSource& random;
    IScrVmCommandErrorSink& errors;
    const FileTransactionContext* transaction=nullptr;
    ScriptNamedValueNativeTables names=GameScriptNamedValueTables();
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
    decltype(&::MessageBoxA) messageBox=&::MessageBoxA;
};

class RootScriptNativeExecutionRuntime final:public IRestoredScriptNativeExecution
{
public:
    explicit RootScriptNativeExecutionRuntime(RootScriptNativeExecutionDependencies dependencies)noexcept
        :dependencies_(dependencies){}
    RootScriptNativeExecutionRuntime(const RootScriptNativeExecutionRuntime&)=delete;
    RootScriptNativeExecutionRuntime& operator=(const RootScriptNativeExecutionRuntime&)=delete;
    ScriptNativeExecutionSelectionScope BindSelectedReceiver(RootRuntimePayload&,ScriptNativeBody&)override;
    std::uint8_t LoadAt4D3C90(ScriptNativeBody&,std::int32_t)override;
    std::int32_t DispatchAt4D3FF0(ScriptNativeBody&)override;
private:
    IRestoredScriptNativeExecution& RequireSelected(ScriptNativeBody&)const;
    RootScriptNativeExecutionDependencies dependencies_;
};
}
