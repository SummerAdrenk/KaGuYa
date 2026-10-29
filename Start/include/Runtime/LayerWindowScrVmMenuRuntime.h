#pragma once

#include <Runtime/LayerWindowEffectRuntime.h>
#include <Runtime/LayerWindowScrResultApplier.h>
#include <Runtime/MappedSoundSlotRuntime.h>
#include <Runtime/ProgramRuntimeServices.h>
#include <Runtime/ScrScriptRuntime.h>
#include <Runtime/ScrVmMenuHandler.h>
#include <Runtime/ScrVmSoundHandler.h>

#include <cstdint>
#include <optional>

namespace KaGuYa::Start::Runtime
{
// 对应 unread.ini 对象的第二组 byte 数组。0x4CEDA0 只在 operand 落入
// 该组范围时置位，并仅在 0 -> 1 时调用 0x43D3D0 持久化整个 unread 对象。
class IMenuUnreadChoiceState
{
public:
    virtual ~IMenuUnreadChoiceState() = default;

    [[nodiscard]] virtual std::optional<bool> ReadChoiceFlag(
        std::int32_t operand) const = 0;
    virtual void SetChoiceFlag(std::int32_t operand) = 0;
    virtual void PersistUnreadFlags() = 0;
};

// 对应 opcode 8 / 0x4CEDA0 访问的生产对象边界。
class LayerWindowScrVmMenuRuntime final : public IScrVmMenuRuntimeBoundary
{
public:
    LayerWindowScrVmMenuRuntime(
        ScrScriptRuntimeState& scriptRuntime,
        const IScrResourceNameSource& resourceNames,
        LayerWindowScrResultApplier& resultApplier,
        LayerWindowRuntime& layerWindow,
        IScrVmSoundRuntimeBoundary& soundRuntime,
        IProgramRegistrySlot7& registrySlot7,
        IMenuUnreadChoiceState& unreadChoiceState) noexcept;

    [[nodiscard]] std::size_t FirstStringGroupCountAt50() const override;
    [[nodiscard]] std::size_t SecondStringGroupCountAt48() const override;
    void ApplyResultLayerList(std::int32_t listIndex) override;
    void ApplyFirstStringSlot(
        std::int32_t value,
        std::size_t groupIndex) override;
    void ApplySecondStringSlot(
        std::int32_t value,
        std::size_t groupIndex) override;
    void SelectEffect(std::int32_t selector) override;
    [[nodiscard]] std::uint16_t ReadSelectionWordAt10() const override;
    [[nodiscard]] std::uint16_t ReadSelectionWordAt12() const override;
    [[nodiscard]] std::optional<bool> ReadChoiceFlag(
        std::int32_t operand) const override;
    void SetChoiceFlag(std::int32_t operand) override;
    void PersistUnreadFlags() override;

private:
    ScrScriptRuntimeState* scriptRuntime_ = nullptr;
    LayerWindowScrResultApplier* resultApplier_ = nullptr;
    LayerWindowRuntime* layerWindow_ = nullptr;
    IProgramRegistrySlot7* registrySlot7_ = nullptr;
    IMenuUnreadChoiceState* unreadChoiceState_ = nullptr;
    MappedSoundSlotRuntime mappedSound_;
};
}
