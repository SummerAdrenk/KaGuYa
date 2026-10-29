#pragma once
#include <Runtime/ScrVmParamsGeneration.h>

#include <Runtime/ScrResourceNameSource.h>
#include <Runtime/ScriptNamedValueNative.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class EnvironmentStartupRuntime;
class IEnvironmentArchiveRuntime;

struct ScriptFileRootReceiver final
{
    void* object;
    ScrVmParamsGeneration& (*paramsAt47BDB0)(void*);
};

// 文件控制命令分别捕获root、slot0、Pattern；不把三者合并成持有快照。
class IScriptFileCommandSource
{
public:
    virtual ~IScriptFileCommandSource() = default;
    virtual ScriptFileRootReceiver CaptureAfterEnsureAt413BE0() = 0;
    virtual const IEnvironmentArchiveRuntime* CaptureSlot0At4180C0() = 0;
    virtual std::int32_t ResolveAt498020(const IEnvironmentArchiveRuntime*, const char* name) = 0;
    virtual ScrResourceNameReceiver CapturePatternNames() = 0;
};

class RootScriptFileCommandSource final : public IScriptFileCommandSource
{
public:
    RootScriptFileCommandSource(RootRuntimeOwner& root, EnvironmentStartupRuntime& environment,
        ScriptNamedValueNativeTables tables, const IScrResourceNameSource& names,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    ScriptFileRootReceiver CaptureAfterEnsureAt413BE0() override;
    const IEnvironmentArchiveRuntime* CaptureSlot0At4180C0() override;
    std::int32_t ResolveAt498020(const IEnvironmentArchiveRuntime*, const char* name) override;
    ScrResourceNameReceiver CapturePatternNames() override;
private:
    RootRuntimeOwner& root_;
    EnvironmentStartupRuntime& environment_;
    ScriptNamedValueNativeTables tables_;
    const IScrResourceNameSource& names_;
    IRuntimeStringManagerSource& strings_;
};
}
