#pragma once

#include <Runtime/TitleDialogProductionRuntime.h>
#include <Runtime/TitleSceneRuntime.h>
#include <Runtime/SceneManagerPublicDialogRuntime.h>
#include <Runtime/SceneManagerPublicDialogView.h>
#include <Runtime/ScrVmExecutableProfile.h>
#include <Runtime/ScrVmValueTable.h>

namespace KaGuYa::Start::Runtime
{
class ITitleSceneConstructionState
{
public:
    virtual ~ITitleSceneConstructionState() = default;
    virtual void WriteSlot2DwordAt04(std::uint32_t) = 0;
    virtual void WriteSlot2ByteAt24(std::uint8_t) = 0;
    virtual void WriteSlot2ByteAt25(std::uint8_t) = 0;
    virtual void WriteSlot2ByteAt26(std::uint8_t) = 0;
    virtual void WriteSlot2ByteAt27(std::uint8_t) = 0;
    virtual void WriteSlot2ByteAt08(std::uint8_t) = 0;
    virtual void WriteSlot2ByteAt09(std::uint8_t) = 0;
    virtual void WriteSlot2ByteAt0C(std::uint8_t) = 0;
    virtual void WriteSlot2DwordAt18(std::uint32_t) = 0;
    virtual void WriteSlot2DwordAt14(std::uint32_t) = 0;
    virtual void WriteSlot2DwordAt1C(std::uint32_t) = 0;
    virtual void WriteSlot2ByteAt29(std::uint8_t) = 0;
    virtual void WriteSlot2ByteAt28(std::uint8_t) = 0;
    // 4A2DD6..4A2E45：slot0 -> root -> 名称查询 -> 该root的CParams -> SetValue。
    virtual void ClearReplayAt4A2DD6() = 0;
};

struct TitleSceneProductionDependencies final
{
    TitleDialogProductionDependencies dialog;
    SceneManagerPublicDialogView publicDialogs;
    ITitleSceneConstructionState& construction;
    ITitleSceneRuntimeServices& scene;
    ITitleMenuRuntimeServices& menu;
};

// 4A2850 的外层 CTitle：先清公开 Dialog，再构造完整标题，最后写 registry/root/replay。
class TitleSceneProductionRuntime final
{
public:
    explicit TitleSceneProductionRuntime(TitleSceneProductionDependencies dependencies);
    TitleSceneProductionRuntime(const TitleSceneProductionRuntime&) = delete;
    TitleSceneProductionRuntime& operator=(const TitleSceneProductionRuntime&) = delete;
    TitleSceneRuntime& Scene() const noexcept { return *scene_; }
    TitleDialogProductionRuntime& Dialog() const noexcept { return *dialog_; }

private:
    // scene_ 借用 dialog_，必须先销毁 scene_ 再释放标题资源。
    std::unique_ptr<TitleDialogProductionRuntime> dialog_;
    std::unique_ptr<TitleSceneRuntime> scene_;
};

class TitleSceneConstructionServicesSample18 final : public SceneConstructionServicesForwarderRuntime
{
public:
    TitleSceneConstructionServicesSample18(ISceneConstructionServices& upstream,
        TitleSceneProductionDependencies dependencies) noexcept;
    std::shared_ptr<ISceneTask> CreateTitle() override;
private:
    TitleSceneProductionDependencies dependencies_;
};
}
