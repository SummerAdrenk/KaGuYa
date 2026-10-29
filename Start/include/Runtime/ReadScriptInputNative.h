#pragma once
#include <Runtime/InputRootNative.h>
#include <Runtime/ScriptNamedValueNative.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class IScrVmCommandErrorSink;
// 下列为callee本身；caller的Env slot5或413BE0步骤必须由caller保留，不能重复取源。
bool ReadScriptSuppressInputAt4C3430(EnvironmentRootOwner&,InputRootOwner&);
bool ReadScriptAccelerateInputAt4AA970(InputRootOwner&);
bool ReadScriptSaveKeyAt47F830(EnvironmentRootOwner&,InputRootOwner&);
bool ReadScriptLoadKeyAt47F650(EnvironmentRootOwner&,InputRootOwner&);
bool ReadScriptQuickSaveKeyAt47F780(EnvironmentRootOwner&,InputRootOwner&,RootRuntimeOwner&,
    IScrVmCommandErrorSink&,ScriptNamedValueNativeTables,IRuntimeStringManagerSource&);
bool ReadScriptQuickLoadKeyAt47F6D0(EnvironmentRootOwner&,InputRootOwner&,RootRuntimeOwner&,
    IScrVmCommandErrorSink&,ScriptNamedValueNativeTables,IRuntimeStringManagerSource&);
bool ReadScriptLikabilityKeyAt47F530(EnvironmentRootOwner&,InputRootOwner&,RootRuntimeOwner&,
    std::uint32_t checkFlag,IScrVmCommandErrorSink&,ScriptNamedValueNativeTables,IRuntimeStringManagerSource&);
}
