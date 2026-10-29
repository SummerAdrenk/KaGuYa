#pragma once

#include <Runtime/ScrScriptRuntime.h>
#include <Runtime/ScrVmScriptCommandState.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// 0x4D4CC0 的外部对象边界。每个可见写入均保持独立，禁止适配器把恢复过程
// 合并成带回滚的原子操作；原函数在任一步失败时都会保留此前已经提交的副作用。
class IRestoredScriptTransactionServices
{
public:
    virtual ~IRestoredScriptTransactionServices() = default;

    [[nodiscard]] virtual std::int32_t ReadRestoredScriptId() const = 0;
    [[nodiscard]] virtual ScrScriptSwitchResult LoadScript(
        std::int32_t scriptId) = 0;

    virtual void SynchronizeRootOwnersAtVslot18() = 0;

    // 每次先取人名，随后Set内部一次捕获Graphics并门控；空模块仍读取名称。
    [[nodiscard]] virtual const char* ReadRegistrySlot1NameAt58() const = 0;
    [[nodiscard]] virtual const char* ReadRegistrySlot1NameAt5C() const = 0;
    virtual void SetGraphicsTextFirstName(const char* nameCp932) = 0;
    virtual void SetGraphicsTextSecondName(const char* nameCp932) = 0;

    virtual void WriteRegistrySlot2ByteAt0C(std::uint8_t value) = 0;
    virtual void WriteRegistrySlot2DwordAt18(std::uint32_t value) = 0;
    virtual void WriteRegistrySlot2DwordAt14(std::uint32_t value) = 0;
    virtual void WriteRegistrySlot2DwordAt1C(std::uint32_t value) = 0;
    virtual void WriteRegistrySlot2DwordAt20(std::uint32_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt08(std::uint8_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt09(std::uint8_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt28(std::uint8_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt29(std::uint8_t value) = 0;

    virtual void WriteScriptVariableReplay(std::int32_t value) = 0;

    [[nodiscard]] virtual std::int32_t ReadRestoredInstructionIndex() const = 0;
    [[nodiscard]] virtual std::int32_t ReadLoadedInstructionCount() const = 0;
    [[nodiscard]] virtual std::int32_t ReadLoadedInstructionProgramCounter(
        std::int32_t instructionIndex) const = 0;
    virtual void WriteEnvironmentProgramCounter(std::int32_t programCounter) = 0;

    [[nodiscard]] virtual std::int32_t DispatchCurrentOpcode() = 0;
};

// 存档恢复后的脚本事务与正常启动的 0x4D53D0 分支不是同一流程，不能复用
// NormalStartupScriptTransaction。返回值是原 opcode dispatcher 的 route；脚本加载
// 失败时严格返回 2。
class RestoredScriptTransaction final
{
public:
    RestoredScriptTransaction(
        ScrVmScriptCommandState& scriptCommandState,
        IRestoredScriptTransactionServices& services) noexcept;

    [[nodiscard]] std::int32_t Execute();

private:
    ScrVmScriptCommandState* scriptCommandState_ = nullptr;
    IRestoredScriptTransactionServices* services_ = nullptr;
};
}
