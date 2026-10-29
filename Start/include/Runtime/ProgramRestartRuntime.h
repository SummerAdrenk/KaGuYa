#pragma once

#include <Runtime/ProgramRuntimeServices.h>
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class ParamsRuntimeRootOwner;
class ScrScriptRuntimeState;
class IScrVmUpdateRuntimeBoundary;
class ScrMusicInstructionHandler;
class MappedSoundSlotRuntime;

// 4D4F20：restart_point已由PROGRAM的原生命名源读取，此处不读取存档或再次解释TEXT。
// 依赖全部借用，script必须是当前CScript正文，图层和音效与VM共用生产执行器。
class ProgramRestartRuntime final : public IProgramRestartCoordinator
{
public:
    ProgramRestartRuntime(RootRuntimeOwner& root, ParamsRuntimeRootOwner& params,
        ScrScriptRuntimeState& script, IScrVmUpdateRuntimeBoundary& layers,
        ScrMusicInstructionHandler& music, MappedSoundSlotRuntime& sound,
        IEnvironmentRootMemory& commandMemory = DefaultEnvironmentRootMemory()) noexcept;
    void Restore(ScrVmProgramRestartPoint restartPoint) override;

private:
    [[nodiscard]] std::int32_t ReadGroupCount(std::size_t field);
    RootRuntimeOwner& root_;
    ParamsRuntimeRootOwner& params_;
    ScrScriptRuntimeState& script_;
    IScrVmUpdateRuntimeBoundary& layers_;
    ScrMusicInstructionHandler& music_;
    MappedSoundSlotRuntime& sound_;
    // 只注入原4-byte/18h申请，成功内存须与全局delete兼容，不驻留原控制块。
    IEnvironmentRootMemory& commandMemory_;
};
}
