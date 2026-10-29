#pragma once
#include <Runtime/ScrVmParamsGeneration.h>
#include <Runtime/ScriptNamedValueNative.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class EnvironmentStartupRuntime;
class IEnvironmentArchiveRuntime;
class IScrVmCommandErrorSink;

struct ScriptDateRootReceiver final
{
    void* object;
    std::uint8_t (*windowEnabledAt4D1D33)(void*);
    std::uint8_t (*patternEnabledAt4D1D7A)(void*);
    ScrVmParamsGeneration& (*paramsAt47BDB0)(void*);
    std::uint8_t (*dateWindowEnabled)(void*) = nullptr;
};

class IScriptDateCommandSource
{
public:
    virtual ~IScriptDateCommandSource() = default;
    [[nodiscard]] virtual ScriptDateRootReceiver CaptureAfterEnsureAt413BE0() = 0;
    [[nodiscard]] virtual const IEnvironmentArchiveRuntime* CaptureSlot0At4180C0() = 0;
    [[nodiscard]] virtual std::int32_t ResolveAt498020(
        const IEnvironmentArchiveRuntime* receiver, const char* name) = 0;
};

class RootScriptDateCommandSource final : public IScriptDateCommandSource
{
public:
    RootScriptDateCommandSource(RootRuntimeOwner& root, EnvironmentStartupRuntime& environment,
        ScriptNamedValueNativeTables tables,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    ScriptDateRootReceiver CaptureAfterEnsureAt413BE0() override;
    const IEnvironmentArchiveRuntime* CaptureSlot0At4180C0() override;
    std::int32_t ResolveAt498020(const IEnvironmentArchiveRuntime*, const char* name) override;
private:
    RootRuntimeOwner& root_;
    EnvironmentStartupRuntime& environment_;
    ScriptNamedValueNativeTables tables_;
    IRuntimeStringManagerSource& strings_;
};

// 这两个无正文入口不申请命令Ref，也不取PC/flags/length。
[[nodiscard]] std::int32_t ExecuteScriptDateWindowAt4D1CE0(
    IScriptDateCommandSource& source, IScrVmCommandErrorSink& errors);
[[nodiscard]] std::int32_t ExecuteScriptDatePlaceAt4D1BF0(
    IScriptDateCommandSource& source, IScrVmCommandErrorSink& errors);
}
