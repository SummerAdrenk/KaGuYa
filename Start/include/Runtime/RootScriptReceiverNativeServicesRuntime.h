#pragma once

#include <Runtime/RootScriptLoadNativeSource.h>
#include <Runtime/ScriptNamedValueNative.h>
#include <memory>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;
class NativeInputRuntime;
class NativeGraphicsRuntimeLibrary;
class IDrawManagerNativeRootSource;
class ILayerWindowEffectNativeSource;
class IScrVmProgramRandSource;
class IScrVmCommandErrorSink;
class ScrVmCore;
class ScrVmParamsGenerationSlot;
struct ScrVmRuntimeStateBundle;
class ScrScriptRuntimeState;

struct RootScriptReceiverNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentStartupRuntime& environment;
    ParamsRuntimeRootOwner& params;
    NativeInputRuntime& input;
    NativeGraphicsRuntimeLibrary& graphics;
    IDrawManagerNativeRootSource& draws;
    ILayerWindowEffectNativeSource& effects;
    // 在原取根/选定Script之后一次性传入，不能在装配中再次查询Root换成别的receiver。
    ScriptNativeBody& body;
    ScriptNativePublication& publication;
    ScrVmParamsGenerationSlot& paramsSlot;
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

// 拥有完整29项命令及其服务，不拥有/增持原CCh、publication或Params槽。
// 同一receiver只装配一次；外层须先退役本图，再销毁被借对象。477620退出场景不退役Script。
class RootScriptReceiverNativeServicesRuntime final
{
public:
    explicit RootScriptReceiverNativeServicesRuntime(RootScriptReceiverNativeDependencies);
    ~RootScriptReceiverNativeServicesRuntime();
    RootScriptReceiverNativeServicesRuntime(const RootScriptReceiverNativeServicesRuntime&)=delete;
    RootScriptReceiverNativeServicesRuntime& operator=(const RootScriptReceiverNativeServicesRuntime&)=delete;
    IRestoredScriptNativeExecution& Execution() noexcept;
    ScrVmRuntimeStateBundle& State() noexcept;
    ScrScriptRuntimeState& Runtime() noexcept;
    const ScrVmCore& Core() const noexcept;
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
}
