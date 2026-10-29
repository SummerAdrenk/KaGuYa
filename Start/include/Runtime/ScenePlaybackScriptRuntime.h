#pragma once

#include <Runtime/ScenePlaybackCatalogRuntime.h>
#include <Runtime/StartupScriptTransaction.h>
#include <Runtime/ScrVmCore.h>

namespace KaGuYa::Start::Runtime
{
// 4B7CE0 的回放调用链：目录、完整启动事务、VM 和字段写入共用生产对象。
// 不拥有这些根依赖，不另建 Params generation 或 CScript。
class ScenePlaybackScriptRuntime final : public IScenePlaybackScriptServices
{
public:
    ScenePlaybackScriptRuntime(const ParamsRuntimeProfile& profile,
        const IScrResourceNameSource& names, StartupScriptTransaction& startup,
        const ScrVmCore& vm, ScrScriptRuntimeState& script,
        ScrVmScriptCommandState& command) noexcept;

    ScenePlaybackSelectionRef FindSelectionAt4ADAE0(
        std::int32_t group, std::int32_t item, std::int32_t segment) override;
    std::string ResolveScriptNameAt4AD1B0(const ScenePlaybackSelection& selection) override;
    void LoadReplayScriptAt4D53D0(const char* name, std::int32_t entry) override;
    std::int32_t ExecuteScriptAt4D3FF0() override;
    void WriteScriptDwordAt04(std::uint32_t value) override;
    void WriteScriptByteAt08(std::uint8_t value) override;

private:
    ScenePlaybackCatalogRuntime catalog_;
    const IScrResourceNameSource* names_;
    StartupScriptTransaction* startup_;
    const ScrVmCore* vm_;
    ScrScriptRuntimeState* script_;
    ScrVmScriptCommandState* command_;
};
}
