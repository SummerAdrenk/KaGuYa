#pragma once

#include <Runtime/SceneControllerRuntime.h>

#include <cstdint>
#include <memory>

namespace KaGuYa::Start::Runtime
{
class IGraffitiPageDialog
{
public:
    virtual ~IGraffitiPageDialog() = default;

    virtual void BeginAtVslot44() = 0;
    [[nodiscard]] virtual bool PollAtVslot48() = 0;
    virtual void CommitAtVslot4C() = 0;
    virtual void DrawAtVslot58() = 0;
};

using GraffitiPageDialogRef = std::shared_ptr<IGraffitiPageDialog>;

enum class GraffitiPagePhase : std::uint32_t
{
    BeginDialog = 0,
    PollDialog = 1,
    AdvanceScene = 2,
};

class IGraffitiPageSceneServices
{
public:
    virtual ~IGraffitiPageSceneServices() = default;

    [[nodiscard]] virtual std::uint32_t ReadScriptSlot2DwordAt04() const = 0;
    virtual void WriteScriptSlot2DwordAt04(std::uint32_t value) = 0;
    [[nodiscard]] virtual std::int32_t ReadProgramParameter00() const = 0;
    [[nodiscard]] virtual GraffitiPageDialogRef CreateLikableCountDialog() = 0;
    [[nodiscard]] virtual GraffitiPageDialogRef CreateWeightCountDialog() = 0;

    // 46A980 无业务参数；旧名称为源兼容保留，不要求 EBX/ESI FrameContext。
    // 生产接 manager.PrepareLayersAt46A980()，此场景忽略聚合 AL。
    virtual void PrepareSevenResultLayersWithFrameContext() = 0;
    virtual void AdvanceActiveSceneFromPending() = 0;

    virtual void DrawGlobalDisplayLayers() = 0;
    virtual void FlushResultLayer0DirtyRects() = 0;
    virtual void ExchangeDialogAndResultLayer0DirtyRects(
        const GraffitiPageDialogRef& dialog) = 0;
    virtual void FinalizeResultLayer0DirtyRectsAtVslot18() = 0;
};

// GraffitiPageScene::CGraffitiScene（49DCA0/4B6130/4B5860）的三阶段状态机。
class GraffitiPageSceneRuntime final : public ISceneTask
{
public:
    explicit GraffitiPageSceneRuntime(IGraffitiPageSceneServices& services);

    // 4C3630：每帧严格执行 Update 后 Draw；argument 未被场景使用。
    void Run(std::int32_t argument) override;

    [[nodiscard]] GraffitiPagePhase Phase() const noexcept;
    [[nodiscard]] std::uint32_t SavedScriptSlot2DwordAt04() const noexcept;
    [[nodiscard]] bool DrawEnabled() const noexcept;
    [[nodiscard]] const GraffitiPageDialogRef& Dialog() const noexcept;

private:
    void Update();
    void Draw();

    IGraffitiPageSceneServices* services_ = nullptr;
    GraffitiPagePhase phase_ = GraffitiPagePhase::BeginDialog;
    GraffitiPageDialogRef dialog_;
    std::uint32_t savedScriptSlot2DwordAt04_ = 0;
    bool drawEnabled_ = true;
};
}
