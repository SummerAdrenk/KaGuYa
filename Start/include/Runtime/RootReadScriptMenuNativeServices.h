#pragma once

#include <Runtime/ReadScriptMenuServicesRuntime.h>
#include <Runtime/ScriptNamedValueNative.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class RootRuntimePayload;
class EnvironmentRootOwner;
class IScrVmCommandErrorSink;

struct RootReadScriptMenuNativeServicesDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    SelectWindowNativeWorkflowServices& selection;
    IScrVmCommandErrorSink& errors;
    ScriptNamedValueNativeTables names = GameScriptNamedValueTables();
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
};

// 只借长寿服务；不缓存Root/Draw/Script/CParams，也不借固定values/profile。
// selection内部的来源与GUI服务由装配方绑定，须与真正Root/Environment同源。
class RootReadScriptMenuNativeServices final : public IReadScriptMenuServices
{
public:
    explicit RootReadScriptMenuNativeServices(RootReadScriptMenuNativeServicesDependencies, bool command = false) noexcept;
    void RebuildChoicesAt4369E0() override;
    void ClearToolByteAt38() override;
    [[nodiscard]] std::uint8_t ReadScriptByteAt24() override;
    [[nodiscard]] std::int32_t TickChoicesAt4BBFD0(std::uint8_t blocked) override;
    void WriteMenuResult(std::int32_t ordinal) override;

private:
    [[nodiscard]] RootRuntimePayload& CaptureRootAfter413BE0();
    [[nodiscard]] DrawLayersView CaptureLayersAfter413BE0();
    RootReadScriptMenuNativeServicesDependencies dependencies_;
    bool command_;
};
}
