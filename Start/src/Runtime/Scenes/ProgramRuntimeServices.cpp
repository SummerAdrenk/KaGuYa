#include <Runtime/ProgramRuntimeServices.h>
#include <Runtime/RootGaugeRuntime.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
// 4D06FE 接收原 char* 细节，4D076C 对其余异常报告 syntax error。
// 服务仅透明转发；保留业务自抛异常，不将 std::exception::what() 改装为业务细节。

ProgramRuntimeRegistry::ProgramRuntimeRegistry(
    IProgramRegistrySlot1& slot1,
    IProgramRegistrySlot2& slot2,
    IProgramRegistryToggleSlot& slot3,
    IProgramRegistryToggleSlot& slot4,
    IProgramRegistrySlot7& slot7) noexcept
    : slot1_(&slot1), slot2_(&slot2), slot3_(&slot3), slot4_(&slot4), slot7_(&slot7)
{
}

IProgramRegistrySlot1& ProgramRuntimeRegistry::Slot1() const noexcept { return *slot1_; }
IProgramRegistrySlot2& ProgramRuntimeRegistry::Slot2() const noexcept { return *slot2_; }
IProgramRegistryToggleSlot& ProgramRuntimeRegistry::Slot3() const noexcept { return *slot3_; }
IProgramRegistryToggleSlot& ProgramRuntimeRegistry::Slot4() const noexcept { return *slot4_; }
IProgramRegistrySlot7& ProgramRuntimeRegistry::Slot7() const noexcept { return *slot7_; }

ProgramRuntimeServices::ProgramRuntimeServices(
    ScrVmValueTable& values,
    IProgramNamedValueRuntime& namedValues,
    IProgramCommand00StringSource& command00Strings,
    IProgramCurrentEnvironment& currentEnvironment,
    ProgramRuntimeRegistry& registry,
    IProgramCommand15Nullsub& command15Nullsub,
    IProgramRestartCoordinator& restartCoordinator, IProgramGaugeRequests* gauges) noexcept
    :ProgramRuntimeServices(values,namedValues,currentEnvironment,registry,command15Nullsub,gauges)
{
    command00Strings_=&command00Strings;
    restartCoordinator_=&restartCoordinator;
}
ProgramRuntimeServices::ProgramRuntimeServices(ScrVmValueTable& values,IProgramNamedValueRuntime& namedValues,
    IProgramCurrentEnvironment& currentEnvironment,ProgramRuntimeRegistry& registry,
    IProgramCommand15Nullsub& command15Nullsub, IProgramGaugeRequests* gauges) noexcept
    :values_(&values),namedValues_(namedValues),currentEnvironment_(&currentEnvironment),
     registry_(&registry),command15Nullsub_(&command15Nullsub),gauges_(gauges)
{
}

void ProgramRuntimeServices::RequestGauge(bool secondary, bool requested)
{
    if (!gauges_) throw std::logic_error("Program gauge services are absent");
    gauges_->RequestGauge(secondary, requested);
}

std::int32_t ProgramRuntimeServices::ReadValueAt(const std::uint16_t index) const
{
    return values_->Read(index);
}

std::int32_t ProgramRuntimeServices::ReadNamedValue(
    const ScrVmProgramNamedValue name) const
{
    // 命名源保留原查询异常及 GetValue 提示后的 int(1)，不翻译成 PROGRAM 业务异常。
    return namedValues_.ReadNamedValue(name);
}

void ProgramRuntimeServices::WriteNamedValue(
    const ScrVmProgramNamedValue name,
    const std::int32_t value)
{
    namedValues_.WriteNamedValue(name, value);
}

void ProgramRuntimeServices::WriteProgResultFromRegistrySlot1ByteAt6F()
{
    namedValues_.WriteProgResultFromRegistrySlot1ByteAt6F();
}

std::string ProgramRuntimeServices::ResolveCommand00String(
    const std::int32_t progParam00)
{
    if(!command00Strings_)throw std::logic_error("PROGRAM offline command00 source is not configured");
    return command00Strings_->Resolve(progParam00);
}

void ProgramRuntimeServices::WriteCurrentObjectStringAt3C(
    const char* const value)
{
    currentEnvironment_->WriteStringAt3C(value);
}

void ProgramRuntimeServices::WriteRegistrySlot7WordAt0C(
    const std::uint32_t index,
    const std::uint16_t value)
{
    registry_->Slot7().WriteWordAt0C(index, value);
}

std::uint16_t ProgramRuntimeServices::ReadRegistrySlot7WordAt0C(
    const std::uint32_t index) const
{
    return registry_->Slot7().ReadWordAt0C(index);
}

void ProgramRuntimeServices::CallRegistrySlot3Vslot18(const std::int32_t value)
{
    registry_->Slot3().CallVslot18(value);
}

void ProgramRuntimeServices::CallRegistrySlot4Vslot18(const std::int32_t value)
{
    registry_->Slot4().CallVslot18(value);
}

std::uint8_t ProgramRuntimeServices::ReadRegistrySlot1ByteAt84() const
{
    return registry_->Slot1().ReadByteAt84();
}

void ProgramRuntimeServices::WriteRegistrySlot2DwordAt14(const std::int32_t value)
{
    registry_->Slot2().WriteDwordAt14(value);
}

std::uint8_t ProgramRuntimeServices::ReadRegistrySlot1ByteAt6F() const
{
    return registry_->Slot1().ReadByteAt6F();
}

void ProgramRuntimeServices::RequireRegistrySlot4()
{
    registry_->Slot4().ResolveForNullsub();
}

void ProgramRuntimeServices::CallCommand15Nullsub2()
{
    command15Nullsub_->CallNullsub2();
}

void ProgramRuntimeServices::WriteRegistrySlot1ChildVslot84Byte(
    const std::int32_t childIndex,
    const bool value)
{
    registry_->Slot1().WriteChildVslot84Byte(childIndex, value);
}

bool ProgramRuntimeServices::ReadRegistrySlot1ChildVslot84Byte(
    const std::int32_t childIndex) const
{
    return registry_->Slot1().ReadChildVslot84Byte(childIndex);
}

void ProgramRuntimeServices::RestoreFromRestartPoint(
    const ScrVmProgramRestartPoint restartPoint)
{
    if(!restartCoordinator_)throw std::logic_error("PROGRAM offline restart coordinator is not configured");
    restartCoordinator_->Restore(restartPoint);
}
}
