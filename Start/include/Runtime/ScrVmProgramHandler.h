#pragma once

#include <Runtime/ScrVmCore.h>
#include <Runtime/ScrVmScriptCommandState.h>
#include <Runtime/ScriptProgramCommandNative.h>

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
enum class ScrVmProgramCommand : std::uint16_t
{
    Command00 = 0,
    Command01 = 1,
    Command02 = 2,
    Command03 = 3,
    Command04 = 4,
    Command05 = 5,
    Command06 = 6,
    Command07 = 7,
    Command08 = 8,
    Command09 = 9,
    Command10 = 10,
    Command11 = 11,
    Command12 = 12,
    Command13 = 13,
    Command14 = 14,
    Command15 = 15,
    Command16 = 16,
    Command17 = 17,
    Command18 = 18,
    Command19 = 19,
    Command20 = 20,
    Command21 = 21,
    Command22 = 22,
    Command23 = 23,
};

enum class ScrVmProgramOperandMode : std::uint8_t
{
    Immediate,
    ValueTableIndex,
};

struct ScrVmProgramRecord final
{
    std::uint8_t flags = 0;
    ScrVmProgramOperandMode operandMode = ScrVmProgramOperandMode::Immediate;
    std::uint16_t rawOperand = 0;
};

enum class ScrVmProgramNamedValue : std::uint8_t
{
    Replay,
    ProgResult,
    ProgParam00,
    ProgParam01,
    RestartPoint,
    // 原可执行文件中的拼写就是 gague_disp，不能擅自修正。
    GagueDisp,
    // 对应 CP932 固定名称“魔素ゲージ　表示フラグ”。
    MasoGaugeDisplayFlag,
};

enum class ScrVmProgramError : std::uint8_t
{
    InvalidFlags,
    InvalidCommand,
    ParameterWordIndexOutOfRange,
    RuntimeBoundaryFailure,
};

class ScrVmProgramCommandException final : public std::runtime_error
{
public:
    ScrVmProgramCommandException(
        ScrVmProgramError error,
        std::string detail);

    [[nodiscard]] ScrVmProgramError Error() const noexcept;
    [[nodiscard]] std::string_view Detail() const noexcept;

private:
    ScrVmProgramError error_;
    std::string detail_;
};

struct ScrVmProgramRestartPoint final
{
    std::int32_t value = 0;
};

class IScrVmProgramRandSource
{
public:
    virtual ~IScrVmProgramRandSource() = default;

    // 原版调用 _rand；实现应返回与目标 x86 CRT 相同宽度的 int。
    [[nodiscard]] virtual std::int32_t NextRand() = 0;
};

// 仅为 opcode 20 暴露已经确认的运行时操作。未知对象继续使用 slot/offset/vslot 名称。
// 原局部错误路径分别接收文本异常与其他C++异常；后者报告 syntax error 并返回2。
// 已经发生的写入和调用不得由边界自动回滚，报告过程自身的异常仍向外传播。
class IScrVmProgramRuntimeBoundary
{
public:
    virtual ~IScrVmProgramRuntimeBoundary() = default;

    [[nodiscard]] virtual std::int32_t ReadValueAt(std::uint16_t index) const = 0;
    [[nodiscard]] virtual std::int32_t ReadNamedValue(
        ScrVmProgramNamedValue name) const = 0;
    virtual void WriteNamedValue(
        ScrVmProgramNamedValue name,
        std::int32_t value) = 0;

    [[nodiscard]] virtual std::string ResolveCommand00String(
        std::int32_t progParam00) = 0;
    virtual void WriteCurrentObjectStringAt3C(const char* value) = 0;

    virtual void WriteRegistrySlot7WordAt0C(
        std::uint32_t index,
        std::uint16_t value) = 0;
    [[nodiscard]] virtual std::uint16_t ReadRegistrySlot7WordAt0C(
        std::uint32_t index) const = 0;

    virtual void CallRegistrySlot3Vslot18(std::int32_t value) = 0;
    virtual void CallRegistrySlot4Vslot18(std::int32_t value) = 0;
    [[nodiscard]] virtual std::uint8_t ReadRegistrySlot1ByteAt84() const = 0;
    virtual void WriteRegistrySlot2DwordAt14(std::int32_t value) = 0;
    [[nodiscard]] virtual std::uint8_t ReadRegistrySlot1ByteAt6F() const = 0;
    // 4D01AD..4D0203：先准备目标CParams，再从此前捕获的Environment读取slot1。
    // 不可用 WriteNamedValue(name, ReadByte()!=0) 替代，否则源值会过早求值。
    virtual void WriteProgResultFromRegistrySlot1ByteAt6F() = 0;
    // command 15 先解析/验证 registry slot 4，随后调用独立的 nullsub_2；
    // 后者不是 slot 4 的虚方法，两个对象归属不得合并。
    virtual void RequireRegistrySlot4() = 0;
    virtual void CallCommand15Nullsub2() = 0;

    virtual void WriteRegistrySlot1ChildVslot84Byte(
        std::int32_t childIndex,
        bool value) = 0;
    [[nodiscard]] virtual bool ReadRegistrySlot1ChildVslot84Byte(
        std::int32_t childIndex) const = 0;

    // 边界实现必须保持 0x4D4F20 已确认的顺序：+0x60、sub_4CB030、
    // sub_4D59E0、第一组 sub_4D6640、第二组 sub_4D62A0；失败不回滚。
    virtual void RestoreFromRestartPoint(ScrVmProgramRestartPoint restartPoint) = 0;
    virtual void RequestGauge(bool secondary, bool requested);
};

class IScrVmProgramCommandErrorSink
{
public:
    virtual ~IScrVmProgramCommandErrorSink() = default;
    virtual void ReportProgramFailure(std::string_view detail) = 0;
};

[[nodiscard]] ScrVmProgramRecord DecodeScrVmProgramRecord(
    const ScrInstructionView& instruction);

// 对应 opcode 20 / 0x4CFB30。
class ScrProgramInstructionHandler final : public IScrVmInstructionHandler
{
public:
    ScrProgramInstructionHandler(
        ScrVmScriptCommandState& scriptState,
        IScrVmProgramRuntimeBoundary& programRuntime,
        IScrVmProgramRandSource& randSource,
        IScrVmProgramCommandErrorSink& errorSink) noexcept;

    ScrProgramInstructionHandler(ScrVmScriptCommandState& state,
        IScrVmProgramRuntimeBoundary& program, IScrVmProgramRandSource& random,
        IScrVmProgramCommandErrorSink& errors, ScriptProgramCommandNativeContext native) noexcept;
    ScrProgramInstructionHandler(ScrVmScriptCommandState& state,
        IScrVmProgramRuntimeBoundary& program,IScrVmProgramRandSource& random,
        ScriptProgramCommandNativeContext native) noexcept;
    [[nodiscard]] std::int32_t ExecuteRuntimeEntry(
        ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;

    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    friend std::int32_t ExecuteScriptProgramAt4CFB30(ScrScriptRuntimeState&,
        ScrProgramInstructionHandler&, const ScriptProgramCommandNativeContext&,
        IEnvironmentRootMemory&, IRuntimeStringManagerSource&);
    std::int32_t ExecuteResolvedCommand(ScrVmProgramCommand command,
        ScrScriptRuntimeState& runtime, const ScriptProgramCommandNativeContext* native,
        IEnvironmentRootMemory* memory);
    ScrVmScriptCommandState& scriptState_;
    IScrVmProgramRuntimeBoundary& programRuntime_;
    IScrVmProgramRandSource& randSource_;
    IScrVmProgramCommandErrorSink* offlineErrors_=nullptr;
    std::optional<ScriptProgramCommandNativeContext> native_;
};
}
