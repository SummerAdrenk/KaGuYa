#pragma once

#include <cstdint>
#include <Runtime/DrawToolView.h>

namespace KaGuYa::Start::Runtime
{
class ToolWindowLayerRuntime;
class ToolWindowInteractionRuntime;
class ReadScriptInputPredicatesRuntime;

// 从场景/模态宿主分离已经实现的工具栏服务；所有调用必须接到同一生产 Tool。
class IReadScriptToolServices
{
public:
    virtual ~IReadScriptToolServices() = default;
    virtual void SetToolModeAt434FB0(std::int32_t value) = 0;
    virtual void InitializeToolAt4AB910(bool value) = 0;
    virtual bool PollConfiguredKeyAt4B0630(std::uint32_t fieldOffset) = 0;
    virtual bool ReadReplayValue() = 0;
    virtual std::uint32_t UpdateToolAt4BE200() = 0;
    virtual std::int32_t ToolEventAt34() = 0;
    virtual std::int32_t ToolSkipPredicateAt4B0760(bool argument) = 0;
    virtual std::int32_t ToolAutoPredicateAt4B0470(bool argument) = 0;
    virtual bool PollSaveAt47F830() = 0;
    virtual bool PollLoadAt47F650() = 0;
    virtual bool PollQuickSaveAt47F780() = 0;
    virtual bool PollQuickLoadAt47F6D0() = 0;
    virtual bool PollLikabilityAt47F530(std::int32_t argument) = 0;
    virtual void CaptureToolAt4C01F0() = 0;
    virtual void SetToolCaptureModeAt4C2300(bool value) = 0;
    virtual void RestoreToolAt4BF4E0() = 0;
};

class ReadScriptToolServicesRuntime final : public IReadScriptToolServices
{
public:
    ReadScriptToolServicesRuntime(DrawToolView window,ReadScriptInputPredicatesRuntime& input) noexcept;
    ReadScriptToolServicesRuntime(ToolWindowLayerRuntime& window,
        ToolWindowInteractionRuntime& interaction, ReadScriptInputPredicatesRuntime& input) noexcept;
    void SetToolModeAt434FB0(std::int32_t value) override;
    void InitializeToolAt4AB910(bool value) override;
    bool PollConfiguredKeyAt4B0630(std::uint32_t fieldOffset) override;
    bool ReadReplayValue() override;
    std::uint32_t UpdateToolAt4BE200() override;
    std::int32_t ToolEventAt34() override;
    std::int32_t ToolSkipPredicateAt4B0760(bool argument) override;
    std::int32_t ToolAutoPredicateAt4B0470(bool argument) override;
    bool PollSaveAt47F830() override;
    bool PollLoadAt47F650() override;
    bool PollQuickSaveAt47F780() override;
    bool PollQuickLoadAt47F6D0() override;
    bool PollLikabilityAt47F530(std::int32_t argument) override;
    void CaptureToolAt4C01F0() override;
    void SetToolCaptureModeAt4C2300(bool value) override;
    void RestoreToolAt4BF4E0() override;

private:
    DrawToolView window_;
    ReadScriptInputPredicatesRuntime& input_;
};
}
