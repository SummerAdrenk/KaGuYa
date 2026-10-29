#pragma once
#include <Runtime/ScriptLoadNative.h>
#include <Runtime/ArchiveReaderFactoryRuntime.h>
#include <Runtime/ScriptDispatchNative.h>
#include <Runtime/RootRestoredScriptNativeServices.h>

namespace KaGuYa::Start::Runtime
{
class ParamsRuntimeRootOwner;
class RootRuntimeOwner;
struct RootScriptLoadNativeDependencies final
{
    ParamsRuntimeRootOwner& params;
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    IArchiveResourceSource& archives;
    IStartFileApi& files;
    const IEnvironmentKeyTableSource& key;
    ArchiveCodecServices codecs;
    IArchiveReaderMemoryApi& readers;
    const FileTransactionContext* transaction=nullptr;
    decltype(&::MessageBoxA) messageBox=&::MessageBoxA;
};
class RootScriptLoadNativeSource final:public IScriptLoadNativeSource
{
public:
    explicit RootScriptLoadNativeSource(RootScriptLoadNativeDependencies) noexcept;
    const char* FindNameAt4AD1B0(std::int32_t) override;
    void LoadAt4D44C0(ScriptReaderFactoryNativeBody14h&,const char*) override;
    ScriptParamsNativeBody* CaptureParamsAfter413BE0() override;
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override;
    int MessageBoxAt4D3EE6(HWND,const char*,const char*,UINT) override;
private:
    RootScriptLoadNativeDependencies dependencies_;
};
// 实际原thiscall表/dispatch循环；context提供现有29 handler的正文外借用，不替换route。
class RestoredScriptNativeExecutionRuntime final:public IRestoredScriptNativeExecution
{
public:
    RestoredScriptNativeExecutionRuntime(ScriptLoadNativeServices&,IScriptExecutionRootSource&,
        IScriptNativeEntryContext&,IEnvironmentRootMemory& = DefaultEnvironmentRootMemory()) noexcept;
    ScriptNativeExecutionSelectionScope BindSelectedReceiver(RootRuntimePayload&,ScriptNativeBody&)override;
    std::uint8_t LoadAt4D3C90(ScriptNativeBody&,std::int32_t) override;
    std::int32_t DispatchAt4D3FF0(ScriptNativeBody&) override;
private:
    ScriptLoadNativeServices& loading_;
    IScriptExecutionRootSource& root_;
    IScriptNativeEntryContext& context_;
    IEnvironmentRootMemory& memory_;
};
}
