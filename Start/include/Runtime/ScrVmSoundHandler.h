#pragma once

#include <Runtime/ScrScriptRuntime.h>
#include <Runtime/ScrResourceNameSource.h>
#include <Runtime/ScrVmCore.h>
#include <Runtime/SoundMappingParameterReceiver.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
class ISoundMappingRestoreParameterSource;
class IScriptFileCommandSource;
class IScrVmCommandErrorSink;
class IScriptSoundGroupSource;
class IScriptSoundLeafSource;

enum class ScrVmSoundSlotGroup : std::uint8_t
{
    FirstAt50,
    SecondAt48,
    Remaining,
};

enum class ScrVmSoundOpenResult : std::uint8_t
{
    Skipped,
    Opened,
    Failed,
};

class ScrVmSoundCommandException final : public std::runtime_error
{
public:
    explicit ScrVmSoundCommandException(std::string detail);
    [[nodiscard]] std::string_view Detail() const noexcept;

private:
    std::string detail_;
};

// 原生声音叶只借用开流门控；槽、映射和参数各有独立原生来源。
class IScrVmSoundOpenGate
{
public:
    virtual ~IScrVmSoundOpenGate() = default;
    [[nodiscard]] virtual bool ShouldSkipOpening(ScrVmSoundSlotGroup group,std::size_t groupIndex) const = 0;
    [[nodiscard]] virtual bool ShouldSkipRemainingOpeningAt4D196A() const = 0;
};

// 离线sound_cmd消费的完整槽接口；原生入口不要求创建第二套声音管理器。
class IScrVmSoundRuntimeBoundary : public IScrVmSoundOpenGate
{
public:
    virtual ~IScrVmSoundRuntimeBoundary() = default;

    [[nodiscard]] virtual std::size_t FirstSlotGroupCountAt50() const = 0;
    [[nodiscard]] virtual std::size_t SecondSlotGroupCountAt48() const = 0;
    virtual void StopAndClearAllEffectSlots() = 0;
    virtual void ClearMappedResourceNameAtPort(std::size_t port) = 0;
    virtual void StopAndClearMappedSlotAtPort(std::size_t port) = 0;
    virtual void StopAndClearRemainingSlot(std::size_t groupIndex) = 0;
    virtual void StoreMappedResourceNameAtPort(
        std::size_t port,
        std::string_view resourceName) = 0;
    // 4D66EE/4D63D3 先捕获 slot2+30，随后查询已捕获的 Pattern 并写入首 NUL 前内容。
    virtual void StoreMappedPatternNameAtPort(std::size_t port,
        ScrResourceNameReceiver receiver, std::int32_t resourceId) = 0;
    // gate 后重新读取映射；不得传入 gate 前查询的名字。
    [[nodiscard]] virtual ScrVmSoundOpenResult OpenMappedSlotAtPort(
        ScrVmSoundSlotGroup group, std::size_t groupIndex, std::size_t port) = 0;
    // 4D19D5 捕获 Env.slot1+1C；不读参数值、不增加共享引用。
    [[nodiscard]] virtual SoundMappingParameterReceiver
        CaptureEffectParametersAt1C() const = 0;
    // 4D1A8C 的 nullable 原指针直接进入后续 +58/+5C，不预复制、不合并 null/empty。
    [[nodiscard]] virtual ScrVmSoundOpenResult OpenRemainingSlot(
        std::size_t groupIndex, const char* resourceName, std::int32_t fallbackValue) = 0;
    // 显式固定名字的受控入口；不承担脚本 Pattern/slot2/Env.slot1 的晚取语义。
    [[nodiscard]] virtual ScrVmSoundOpenResult OpenAndPlaySlot(
        ScrVmSoundSlotGroup group,
        std::size_t groupIndex,
        std::string_view resourceName) = 0;
};

class IScrVmSoundCommandErrorSink
{
public:
    virtual ~IScrVmSoundCommandErrorSink() = default;
    virtual void ReportSoundFailure(std::string_view detail) = 0;
};

// 对应 opcode 9 / 0x4D1520；正文为 i8 port 与未对齐 i32 resourceId。
class ScrSoundInstructionHandler final : public IScrVmInstructionHandler
{
public:
    ScrSoundInstructionHandler(
        const IScrResourceNameSource& resourceNames,
        IScrVmSoundRuntimeBoundary& soundRuntime,
        IScrVmSoundCommandErrorSink& errorSink) noexcept;
    ScrSoundInstructionHandler(const IScrResourceNameSource& resourceNames,
        IScrVmSoundRuntimeBoundary& soundRuntime, IScrVmSoundCommandErrorSink& errorSink,
        IScriptFileCommandSource& pcSource, IScrVmCommandErrorSink& nativeErrors,
        IScriptSoundGroupSource& groups, IScriptSoundLeafSource& leaves) noexcept;
    ScrSoundInstructionHandler(const IScrResourceNameSource& resourceNames,
        IScrVmSoundOpenGate& gate, IScriptFileCommandSource& pcSource,
        IScrVmCommandErrorSink& errors, IScriptSoundGroupSource& groups,
        IScriptSoundLeafSource& leaves) noexcept;
    [[nodiscard]] std::int32_t ExecuteRuntimeEntry(ScrScriptRuntimeState& runtime,
        IEnvironmentRootMemory& memory) override;
    void ApplyOperands(std::int8_t port, std::int32_t resourceId);
    void ApplyNativeOperands(std::int8_t port, std::int32_t resourceId);

    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    const IScrResourceNameSource& resourceNames_;
    struct OfflineDependencies final
    {
        IScrVmSoundRuntimeBoundary& sound;
        IScrVmSoundCommandErrorSink& errors;
    };
    std::optional<OfflineDependencies> offline_;
    IScrVmSoundOpenGate& gate_;
    IScriptFileCommandSource* pcSource_ = nullptr;
    IScrVmCommandErrorSink* nativeErrors_ = nullptr;
    IScriptSoundGroupSource* groups_ = nullptr;
    IScriptSoundLeafSource* leaves_ = nullptr;
};
}
