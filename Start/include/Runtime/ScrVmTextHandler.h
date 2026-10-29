#pragma once

#include <Runtime/ScrVmCore.h>
#include <Runtime/ScrVmExecutableProfile.h>
#include <Runtime/ScrVmMusicHandler.h>
#include <Runtime/ScrVmScriptCommandState.h>
#include <Runtime/ScrVmValueTable.h>
#include <Runtime/ScriptTextCommandNative.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// 暂保留既有构造参数类型；原 +08 属于 CSaveParams2，不能在此保留状态副本。
struct ScrVmTextState final
{
};

class ScrVmTextCommandException final : public std::runtime_error
{
public:
    explicit ScrVmTextCommandException(std::string detail);
    [[nodiscard]] std::string_view Detail() const noexcept;

private:
    std::string detail_;
};

class IScrVmTextRuntimeBoundary
{
public:
    virtual ~IScrVmTextRuntimeBoundary() = default;

    [[nodiscard]] virtual std::size_t FirstStringGroupCountAt50() const = 0;
    [[nodiscard]] virtual std::size_t SecondStringGroupCountAt48() const = 0;
    virtual void ApplyResultLayerList(std::int32_t listIndex) = 0;
    virtual void ApplyFirstStringSlot(
        std::int32_t value,
        std::size_t groupIndex) = 0;
    virtual void ApplySecondStringSlot(
        std::int32_t value,
        std::size_t groupIndex) = 0;
    virtual void SelectEffect(std::int32_t selector) = 0;
};

class IScrVmTextCommandErrorSink
{
public:
    virtual ~IScrVmTextCommandErrorSink() = default;
    virtual void ReportTextFailure(std::string_view detail) = 0;
};

// 对应 opcode 7 / 0x4D1E50。
class ScrTextInstructionHandler final : public IScrVmInstructionHandler
{
public:
    ScrTextInstructionHandler(
        ScrVmValueTable& values,
        const ScrVmExecutableProfile& executableProfile,
        ScrVmScriptCommandState& scriptState,
        ScrVmTextState& textState,
        ScrMusicInstructionHandler& musicController,
        IScrVmTextRuntimeBoundary& textRuntime,
        IScrVmTextCommandErrorSink& errorSink) noexcept;
    ScrTextInstructionHandler(ScrVmValueTable& values, const ScrVmExecutableProfile& profile,
        ScrVmScriptCommandState& state, ScrVmTextState& textState, ScrMusicInstructionHandler& music,
        IScrVmTextRuntimeBoundary& text, IScrVmTextCommandErrorSink& errors,
        ScriptTextCommandNativeContext native) noexcept;
    ScrTextInstructionHandler(ScrVmScriptCommandState& state,ScrMusicInstructionHandler& music,
        IScrVmTextRuntimeBoundary& text,ScriptTextCommandNativeContext native) noexcept;
    std::int32_t ExecuteRuntimeEntry(ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;

    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    struct OfflineDependencies final
    {
        ScrVmValueTable& values;
        const ScrVmExecutableProfile& profile;
        IScrVmTextCommandErrorSink& errors;
    };
    std::optional<OfflineDependencies> offline_;
    ScrVmScriptCommandState& scriptState_;
    ScrMusicInstructionHandler& musicController_;
    IScrVmTextRuntimeBoundary& textRuntime_;
    std::optional<ScriptTextCommandNativeContext> native_;
};
}
