#pragma once
#include <Platform/InputViews.h>

#include <Runtime/GuiInputStateRuntime.h>

namespace KaGuYa::Start::Runtime
{
class ClickInfoArchive;
class UnSaveDataRuntime;
class ScrVmValueTable;
class ScrVmExecutableProfile;

// 文字/工具条/选项场景共用的实际输入条件，借用同一环境及当前 generation。
class ReadScriptInputPredicatesRuntime final
{
public:
    ReadScriptInputPredicatesRuntime(const ClickInfoArchive& click,
        const UnSaveDataRuntime& unsave, const ScrVmValueTable& values,
        const ScrVmExecutableProfile& profile, Platform::MouseView mouse,
        Platform::KeyboardView keyboard) noexcept;

    [[nodiscard]] bool MouseSelectorAt4B0630(std::uint32_t selector) const noexcept;
    [[nodiscard]] bool ConfiguredMouseAtClickOffset(std::uint32_t fieldOffset) const;
    [[nodiscard]] std::int32_t SkipAt4B0760(bool includeHeldKey) const;
    [[nodiscard]] std::int32_t AutoAt4B0470(bool includeHeldKey) const;
    [[nodiscard]] bool SaveAt47F830() const;
    [[nodiscard]] bool LoadAt47F650() const;
    [[nodiscard]] bool QuickSaveAt47F780() const;
    [[nodiscard]] bool QuickLoadAt47F6D0() const;
    [[nodiscard]] bool LikabilityAt47F530(std::uint8_t checkFlag) const;
    [[nodiscard]] bool ReplayValue() const;
    [[nodiscard]] bool HeldSkipKey() const;

private:
    const ClickInfoArchive& click_;
    const UnSaveDataRuntime& unsave_;
    const ScrVmValueTable& values_;
    const ScrVmExecutableProfile& profile_;
    Platform::MouseView mouse_;
    GameGuiInputStateRuntime input_;
};
}
