#include <Runtime/GraffitiPageSceneRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
GraffitiPageSceneRuntime::GraffitiPageSceneRuntime(
    IGraffitiPageSceneServices& services)
    : services_(&services)
{
    // 49DCFA..49DD9D：保存与清零发生在参数读取和子对话框构造之前。
    savedScriptSlot2DwordAt04_ = services_->ReadScriptSlot2DwordAt04();
    services_->WriteScriptSlot2DwordAt04(0u);
    phase_ = GraffitiPagePhase::BeginDialog;

    const std::int32_t dialogMode = services_->ReadProgramParameter00();
    dialog_ = dialogMode == 0 ?
        services_->CreateLikableCountDialog() :
        services_->CreateWeightCountDialog();
    if (!dialog_)
        throw std::runtime_error("GraffitiPage dialog construction returned null");
}

void GraffitiPageSceneRuntime::Run(const std::int32_t argument)
{
    static_cast<void>(argument);
    Update();
    Draw();
}

GraffitiPagePhase GraffitiPageSceneRuntime::Phase() const noexcept
{
    return phase_;
}

std::uint32_t GraffitiPageSceneRuntime::SavedScriptSlot2DwordAt04() const noexcept
{
    return savedScriptSlot2DwordAt04_;
}

bool GraffitiPageSceneRuntime::DrawEnabled() const noexcept
{
    return drawEnabled_;
}

const GraffitiPageDialogRef& GraffitiPageSceneRuntime::Dialog() const noexcept
{
    return dialog_;
}

void GraffitiPageSceneRuntime::Update()
{
    switch (phase_)
    {
    case GraffitiPagePhase::BeginDialog:
        services_->PrepareSevenResultLayersWithFrameContext();
        dialog_->BeginAtVslot44();
        phase_ = GraffitiPagePhase::PollDialog;
        return;

    case GraffitiPagePhase::PollDialog:
        if (!dialog_->PollAtVslot48())
        {
            dialog_->CommitAtVslot4C();
            phase_ = GraffitiPagePhase::AdvanceScene;
        }
        return;

    case GraffitiPagePhase::AdvanceScene:
        // 4B614D..4B6197：原顺序是先推进场景，再恢复 script slot 2 +4。
        services_->AdvanceActiveSceneFromPending();
        services_->WriteScriptSlot2DwordAt04(savedScriptSlot2DwordAt04_);
        return;
    }
}

void GraffitiPageSceneRuntime::Draw()
{
    if (!drawEnabled_)
        return;

    services_->DrawGlobalDisplayLayers();
    services_->FlushResultLayer0DirtyRects();
    services_->ExchangeDialogAndResultLayer0DirtyRects(dialog_);
    services_->FinalizeResultLayer0DirtyRectsAtVslot18();
    dialog_->DrawAtVslot58();
}
}
