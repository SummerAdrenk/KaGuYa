#pragma once

#include <Runtime/ReadScriptTextInteractionRuntime.h>
#include <Runtime/ReadScriptMenuServicesRuntime.h>
#include <Runtime/ReadScriptInteractionStateSource.h>

#include <optional>

namespace KaGuYa::Start::Runtime
{
// 46B280 是 asave 文件事务，不是 SceneController 的通用 Run。
// 与 QuickSave 一样，真实持久化宿主必须提供此边界，不能以返回成功代替写盘。
class IReadScriptMenuInteractionHost : public IReadScriptTextInteractionHost
{
public:
    virtual void AutoSaveAt46B280(std::uint8_t argument) = 0;
};

// 4B11B0 全部分支；继续借用外层 ReadScript 的状态，不构造影子 substate。
class ReadScriptMenuInteractionRuntime final
{
public:
    ReadScriptMenuInteractionRuntime(IReadScriptMenuInteractionHost& host, IReadScriptToolServices& tools,
        IReadScriptLayerServices& layers, IReadScriptMenuServices& menu,
        SaveDataEnvironmentArchive& save, UnSaveDataRuntime& unsave, IReadScriptTextServices& text) noexcept;
    ReadScriptMenuInteractionRuntime(IReadScriptMenuInteractionHost& host, IReadScriptToolServices& tools,
        IReadScriptLayerServices& layers, IReadScriptMenuServices& menu,
        IReadScriptInteractionStateSource& stateSource, IReadScriptTextServices& text) noexcept;
    // 固定入口的 source 引用指向本对象内的 optional，禁止复制后仍借用旧对象。
    ReadScriptMenuInteractionRuntime(const ReadScriptMenuInteractionRuntime&) = delete;
    ReadScriptMenuInteractionRuntime& operator=(const ReadScriptMenuInteractionRuntime&) = delete;
    [[nodiscard]] bool RunAt4B11B0(ReadScriptSceneState& state);
private:
    [[nodiscard]] bool RunSelecting(ReadScriptSceneState& state);
    [[nodiscard]] bool RunCommand(ReadScriptSceneState& state);
    [[nodiscard]] bool SuspendForLayers(ReadScriptSceneState& state);
    [[nodiscard]] bool StartSaveLoad(ReadScriptSceneState& state, std::int32_t substate);
    void StopForScene();
    IReadScriptMenuInteractionHost& host_;
    IReadScriptToolServices& tools_;
    IReadScriptLayerServices& layers_;
    IReadScriptMenuServices& menu_;
    std::optional<FixedReadScriptInteractionStateSource> fixedState_;
    IReadScriptInteractionStateSource& stateSource_;
    IReadScriptTextServices& text_;
};
}
