#pragma once

#include <Runtime/ScrVmCore.h>
#include <Runtime/ScrVmExecutableProfile.h>
#include <Runtime/ScrVmMusicHandler.h>
#include <Runtime/ScrVmScriptCommandState.h>
#include <Runtime/ScrVmValueTable.h>
#include <Runtime/ScriptMenuCommandNative.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// 稳定槽view，不另存候选；root换CParams后窗口和VM必须看到同一新正文。
class ScrVmMenuState final
{
public:
    ScrVmMenuState();
    explicit ScrVmMenuState(ScrVmParamsGenerationBinding generation);
    [[nodiscard]] ScriptDwordVectorStorage& CandidateStorage();
    [[nodiscard]] const ScriptDwordVectorStorage& CandidateStorage() const;
    [[nodiscard]] std::uint32_t CandidateCount() const;
    [[nodiscard]] std::int32_t CandidateAt(std::size_t ordinal) const;
    [[nodiscard]] bool UsesParamsGenerationSlot(const ScrVmParamsGenerationBinding& generation) const noexcept;
private:
    ScrVmParamsGenerationBinding generation_;
};

class ScrVmMenuCommandException final : public std::runtime_error
{
public:
    explicit ScrVmMenuCommandException(std::string detail);
    [[nodiscard]] std::string_view Detail() const noexcept;

private:
    std::string detail_;
};

class IScrVmMenuRuntimeBoundary : public IScrVmMenuLayerBoundary
{
public:
    virtual ~IScrVmMenuRuntimeBoundary() = default;

    [[nodiscard]] virtual std::size_t FirstStringGroupCountAt50() const = 0;
    [[nodiscard]] virtual std::size_t SecondStringGroupCountAt48() const = 0;
    virtual void ApplyFirstStringSlot(
        std::int32_t value,
        std::size_t groupIndex) = 0;
    virtual void ApplySecondStringSlot(
        std::int32_t value,
        std::size_t groupIndex) = 0;

    [[nodiscard]] virtual std::uint16_t ReadSelectionWordAt10() const = 0;
    [[nodiscard]] virtual std::uint16_t ReadSelectionWordAt12() const = 0;
    // nullopt 对应 choice operand 不在未读标志表范围内，原函数直接跳过。
    [[nodiscard]] virtual std::optional<bool> ReadChoiceFlag(
        std::int32_t operand) const = 0;
    virtual void SetChoiceFlag(std::int32_t operand) = 0;
    virtual void PersistUnreadFlags() = 0;
};

class IScrVmMenuCommandErrorSink
{
public:
    virtual ~IScrVmMenuCommandErrorSink() = default;
    virtual void ReportMenuFailure(std::string_view detail) = 0;
};

// 对应 opcode 8 / 0x4CEDA0。
class ScrMenuInstructionHandler final : public IScrVmInstructionHandler
{
public:
    ScrMenuInstructionHandler(
        ScrVmValueTable& values,
        const ScrVmExecutableProfile& executableProfile,
        ScrVmScriptCommandState& scriptState,
        ScrVmMenuState& menuState,
        ScrMusicInstructionHandler& musicController,
        IScrVmMenuRuntimeBoundary& menuRuntime,
        IScrVmMenuCommandErrorSink& errorSink) noexcept;

    ScrMenuInstructionHandler(ScrVmValueTable& values, const ScrVmExecutableProfile& profile,
        ScrVmScriptCommandState& state, ScrVmMenuState& menuState,
        ScrMusicInstructionHandler& music, IScrVmMenuRuntimeBoundary& menu,
        IScrVmMenuCommandErrorSink& errors, ScriptMenuCommandNativeContext native) noexcept;
    // 原生入口不需要离线候选容器、profile或ReportMenuFailure。
    ScrMenuInstructionHandler(ScrVmScriptCommandState& state,
        ScrMusicInstructionHandler& music, IScrVmMenuLayerBoundary& layers,
        ScriptMenuCommandNativeContext native) noexcept;
    [[nodiscard]] std::int32_t ExecuteRuntimeEntry(
        ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;

    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    struct OfflineDependencies final
    {
        ScrVmValueTable& values;
        const ScrVmExecutableProfile& profile;
        ScrVmMenuState& state;
        IScrVmMenuRuntimeBoundary& menu;
        IScrVmMenuCommandErrorSink& errors;
    };
    std::optional<OfflineDependencies> offline_;
    ScrVmScriptCommandState& scriptState_;
    ScrMusicInstructionHandler& musicController_;
    IScrVmMenuLayerBoundary& layers_;
    std::optional<ScriptMenuCommandNativeContext> native_;
};
}
