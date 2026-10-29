#pragma once

#include <Runtime/GuiModalDialogState.h>
#include <span>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
// 必须由同一根对象/窗口环境提供；不在对话框里缓存 mode、viewport 或场景 manager。
class ISystemMenuDialogServices
{
public:
    virtual ~ISystemMenuDialogServices() = default;
    virtual std::uint8_t ReadCurrentWindowModeAt04() = 0;
    virtual std::uint8_t ReadSettingsEnabledAtRoot219() = 0;
    virtual bool IsGameWindowIconic() = 0;
    virtual RECT ReadRootLogicalRectangleAt478E50() = 0;
    virtual void PostGameWindowMessage(UINT message, WPARAM wParam, LPARAM lParam) = 0;
    virtual void WriteSceneManagerPendingCommandAt1C(std::string_view nameCp932) = 0;
};

// 4F41A0 / 5BE868；持续存在于 CInfomationDisplay 中，不是确认框的 Yes/No 模式。
class SystemMenuDialogRuntime final : public IModalDialogRuntimeTarget,
    public IModalDialogRuntimeTargetReference
{
public:
    SystemMenuDialogRuntime(GuiModalDialogDependencies dependencies, ISystemMenuDialogServices& services);
    ~SystemMenuDialogRuntime() noexcept;
    SystemMenuDialogRuntime(const SystemMenuDialogRuntime&) = delete;
    SystemMenuDialogRuntime& operator=(const SystemMenuDialogRuntime&) = delete;

    bool CanRemainOpenAt5104A0();
    void BeginAtVslot44() override;
    bool PollAtVslot48() override;
    bool EndAtVslot4C() override;
    std::int32_t ResultAtVslot50() const override;
    IModalDialogRuntimeTarget* ResolveAt4A5330() override;
    void DrawAt5230B0(const RECT& clip);
    void CompositeAt5231C0(GraphicsSurfaceDestination destination, const RECT* clip);
    void SubmitControlRectAt50FC80(const RECT& rect);
    void FlushDirtyRectsAt5267E0();
    void AssignCapturedFrameAt48CA30(GraphicsSurfaceReference frame);
    std::optional<std::int32_t> PhaseAt6C() const noexcept;
    std::int32_t SuppressReopenAt84() const noexcept;
    std::span<const CgOperationChildControlRef> ChildrenInInsertionOrder() const noexcept;
    std::span<const CgOperationClickedEventRef> ClickedEventsInInsertionOrder() const noexcept;
    const GraphicsSurfaceReference& CapturedFrameAt5C() const noexcept;

private:
    struct State;
    std::unique_ptr<State> state_;
};
}
