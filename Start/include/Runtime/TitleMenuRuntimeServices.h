#pragma once

#include <Runtime/SceneManagerDispatchRuntime.h>
#include <Runtime/StartupScriptTransaction.h>
#include <Runtime/TitleMenuDispatchRuntime.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class ITitleMenuRootState
{
public:
    virtual ~ITitleMenuRootState() = default;
    virtual void WriteByteAt218(std::uint8_t value) = 0;
    virtual void WriteByteAt219(std::uint8_t value) = 0;
};

// 标题菜单末段的生产服务绑定。argument 仅允许 0x4BB2A4 传入的 -1；
// replay 分支属于 0x4D53D0 的另一条事务，不在此入口伪装实现。
class TitleMenuRuntimeServices final : public ITitleMenuRuntimeServices
{
public:
    TitleMenuRuntimeServices(
        NormalStartupScriptTransaction& normalStartup,
        SceneManagerDispatchRuntime& sceneManager,
        ITitleMenuRootState& rootState) noexcept;

    [[nodiscard]] bool LoadScript(
        const char* scriptNameCp932,
        std::int32_t argument) override;
    void ClearSceneController() override;
    void QueueScene(const char* sceneNameCp932) override;
    void SetRootByteAt218(std::uint8_t value) override;
    void SetRootByteAt219(std::uint8_t value) override;

private:
    NormalStartupScriptTransaction* normalStartup_ = nullptr;
    SceneManagerDispatchRuntime* sceneManager_ = nullptr;
    ITitleMenuRootState* rootState_ = nullptr;
};
}
