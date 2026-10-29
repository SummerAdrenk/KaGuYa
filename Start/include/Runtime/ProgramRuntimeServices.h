#pragma once

#include <Runtime/RootProgramNamedValueRuntime.h>
#include <Runtime/ScrVmProgramHandler.h>
#include <Runtime/ScrVmValueTable.h>

#include <cstdint>
#include <string>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
class IProgramGaugeRequests;
// command 0 的版本化字符串查询与当前动作宿主 +0x3C 写入属于两个对象。
class IProgramCommand00StringSource
{
public:
    virtual ~IProgramCommand00StringSource() = default;
    [[nodiscard]] virtual std::string Resolve(std::int32_t index) = 0;
};

class IProgramCurrentEnvironment
{
public:
    virtual ~IProgramCurrentEnvironment() = default;
    virtual void WriteStringAt3C(const char* value) = 0;
};

class IProgramRegistrySlot1
{
public:
    virtual ~IProgramRegistrySlot1() = default;
    [[nodiscard]] virtual std::uint8_t ReadByteAt84() const = 0;
    [[nodiscard]] virtual std::uint8_t ReadByteAt6F() const = 0;
    virtual void WriteChildVslot84Byte(std::int32_t index, bool value) = 0;
    [[nodiscard]] virtual bool ReadChildVslot84Byte(std::int32_t index) const = 0;
};

class IProgramRegistrySlot2
{
public:
    virtual ~IProgramRegistrySlot2() = default;
    virtual void WriteDwordAt14(std::int32_t value) = 0;
};

class IProgramRegistryToggleSlot
{
public:
    virtual ~IProgramRegistryToggleSlot() = default;
    virtual void CallVslot18(std::int32_t value) = 0;
    // 4D03E7仍执行真实417990，但其返回值可为空；4980F0不访问正文。
    virtual void ResolveForNullsub() = 0;
};

class IProgramRegistrySlot7
{
public:
    virtual ~IProgramRegistrySlot7() = default;
    virtual void WriteWordAt0C(std::uint32_t index, std::uint16_t value) = 0;
    [[nodiscard]] virtual std::uint16_t ReadWordAt0C(std::uint32_t index) const = 0;
};

class IProgramCommand15Nullsub
{
public:
    virtual ~IProgramCommand15Nullsub() = default;
    virtual void CallNullsub2() = 0;
};

class IProgramRestartCoordinator
{
public:
    virtual ~IProgramRestartCoordinator() = default;
    virtual void Restore(ScrVmProgramRestartPoint restartPoint) = 0;
};

// 只表达 0x417990 已确认的 slot 1/2/3/4/7 消费者；未知类名和注册来源不在此伪造。
class ProgramRuntimeRegistry final
{
public:
    ProgramRuntimeRegistry(
        IProgramRegistrySlot1& slot1,
        IProgramRegistrySlot2& slot2,
        IProgramRegistryToggleSlot& slot3,
        IProgramRegistryToggleSlot& slot4,
        IProgramRegistrySlot7& slot7) noexcept;

    [[nodiscard]] IProgramRegistrySlot1& Slot1() const noexcept;
    [[nodiscard]] IProgramRegistrySlot2& Slot2() const noexcept;
    [[nodiscard]] IProgramRegistryToggleSlot& Slot3() const noexcept;
    [[nodiscard]] IProgramRegistryToggleSlot& Slot4() const noexcept;
    [[nodiscard]] IProgramRegistrySlot7& Slot7() const noexcept;

private:
    IProgramRegistrySlot1* slot1_ = nullptr;
    IProgramRegistrySlot2* slot2_ = nullptr;
    IProgramRegistryToggleSlot* slot3_ = nullptr;
    IProgramRegistryToggleSlot* slot4_ = nullptr;
    IProgramRegistrySlot7* slot7_ = nullptr;
};

// opcode 20 的生产服务聚合器。它不拥有 registry 对象，只按原根对象的所有权关系引用。
class ProgramRuntimeServices final : public IScrVmProgramRuntimeBoundary
{
public:
    // 原生PROGRAM0/19分别由原生消息来源/4D4F20执行，不要求离线字符串和重启对象。
    ProgramRuntimeServices(ScrVmValueTable& values,IProgramNamedValueRuntime& namedValues,
        IProgramCurrentEnvironment& currentEnvironment,ProgramRuntimeRegistry& registry,
        IProgramCommand15Nullsub& command15Nullsub, IProgramGaugeRequests* gauges = nullptr) noexcept;
    ProgramRuntimeServices(
        ScrVmValueTable& values,
        IProgramNamedValueRuntime& namedValues,
        IProgramCommand00StringSource& command00Strings,
        IProgramCurrentEnvironment& currentEnvironment,
        ProgramRuntimeRegistry& registry,
        IProgramCommand15Nullsub& command15Nullsub,
        IProgramRestartCoordinator& restartCoordinator, IProgramGaugeRequests* gauges = nullptr) noexcept;

    [[nodiscard]] std::int32_t ReadValueAt(std::uint16_t index) const override;
    [[nodiscard]] std::int32_t ReadNamedValue(
        ScrVmProgramNamedValue name) const override;
    void WriteNamedValue(
        ScrVmProgramNamedValue name,
        std::int32_t value) override;
    void WriteProgResultFromRegistrySlot1ByteAt6F() override;
    [[nodiscard]] std::string ResolveCommand00String(
        std::int32_t progParam00) override;
    void WriteCurrentObjectStringAt3C(const char* value) override;
    void WriteRegistrySlot7WordAt0C(
        std::uint32_t index,
        std::uint16_t value) override;
    [[nodiscard]] std::uint16_t ReadRegistrySlot7WordAt0C(
        std::uint32_t index) const override;
    void CallRegistrySlot3Vslot18(std::int32_t value) override;
    void CallRegistrySlot4Vslot18(std::int32_t value) override;
    [[nodiscard]] std::uint8_t ReadRegistrySlot1ByteAt84() const override;
    void WriteRegistrySlot2DwordAt14(std::int32_t value) override;
    [[nodiscard]] std::uint8_t ReadRegistrySlot1ByteAt6F() const override;
    void RequireRegistrySlot4() override;
    void CallCommand15Nullsub2() override;
    void WriteRegistrySlot1ChildVslot84Byte(
        std::int32_t childIndex,
        bool value) override;
    [[nodiscard]] bool ReadRegistrySlot1ChildVslot84Byte(
        std::int32_t childIndex) const override;
    void RestoreFromRestartPoint(ScrVmProgramRestartPoint restartPoint) override;
    void RequestGauge(bool secondary, bool requested) override;

private:
    ScrVmValueTable* values_ = nullptr;
    IProgramNamedValueRuntime& namedValues_;
    IProgramCommand00StringSource* command00Strings_ = nullptr;
    IProgramCurrentEnvironment* currentEnvironment_ = nullptr;
    ProgramRuntimeRegistry* registry_ = nullptr;
    IProgramCommand15Nullsub* command15Nullsub_ = nullptr;
    IProgramRestartCoordinator* restartCoordinator_ = nullptr;
    IProgramGaugeRequests* gauges_ = nullptr;
};
}
