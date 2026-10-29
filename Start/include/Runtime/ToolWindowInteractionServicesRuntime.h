#pragma once
#include <Platform/InputViews.h>

#include <Runtime/GuiInputStateRuntime.h>
#include <Runtime/ToolWindowInteractionRuntime.h>
#include <Runtime/DrawLayersView.h>

namespace KaGuYa::Start::Runtime
{
class DrawManagerLayerRuntime;
class ScrVmValueTable;
class ScrVmExecutableProfile;
class UnSaveDataRuntime;

// CToolWin 的实际查询来源：环境对象与 VM 借用同一 owner，图层每次按原槽索引。
// 不提前解引用所有依赖；短路分支未读取的缺失槽不能导致提前失败。
class ToolWindowInteractionServicesRuntime final : public IToolWindowInteractionServices
{
public:
    ToolWindowInteractionServicesRuntime(const UnSaveDataRuntime& unsave,
        DrawLayersView layers, const ScrVmValueTable& values,
        const ScrVmExecutableProfile& profile, Platform::MouseView mouse,
        Platform::KeyboardView keyboard) noexcept;

    std::uint32_t ReadUnSaveModeAt04() override;
    std::uint8_t ReadUnSaveByteAt08() override;
    std::uint8_t ReadUnSaveByteAt09() override;
    std::uint8_t ReadUnSaveByteAt0C() override;
    std::uint32_t ReadUnSaveFlagsAt14() override;
    std::uint32_t ReadUnSaveFlagsAt18() override;
    std::uint32_t ReadUnSaveFlagsAt1C() override;
    std::uint32_t ReadUnSaveFlagsAt20() override;
    bool HasVoiceArrayStorageAt3C() override;
    std::int32_t ReadTextGlyphCursorAt50() override;
    std::uint8_t ReadMessageWindowEnabledAt0C() override;
    bool IsSkipKeyHeldAt4198D0() override;
    std::int32_t ReadReplayValueAt4C73A0() override;
    std::int32_t ReadAffectionDisplayFlagAt4C73A0() override;

private:
    const UnSaveDataRuntime& unsave_;
    DrawLayersView layers_;
    const ScrVmValueTable& values_;
    const ScrVmExecutableProfile& profile_;
    GameGuiInputStateRuntime input_;
};
}
