#include <Runtime/ScrVmValueTable.h>
#include <Runtime/ScriptParamsHistoryNative.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
void ScrVmValueTable::AppendCurrentMessageToHistoryAt3C()
{
    auto& generation = generationSlot_->Current();
    const auto format = static_cast<std::uint32_t>(generation.messageFormatIndexAt60);
    AppendScriptParamsHistoryAt4C81F0(generation.historyAt3C, format, generationSlot_->Memory());
}

ScrVmValueTable::ScrVmValueTable()
    : ScrVmValueTable(CreateScrVmParamsGenerationSlot())
{
}

ScrVmValueTable::ScrVmValueTable(
    ScrVmParamsGenerationBinding generationSlot)
    : generationSlot_(std::move(generationSlot))
{
    if (!generationSlot_)
        throw std::invalid_argument("SCR VM params generation slot is null");
}

ScrVmValueException::ScrVmValueException(
    const ScrVmValueError error,
    const std::int32_t index,
    const char* const message)
    : std::out_of_range(message),
      error_(error),
      index_(index)
{
}

ScrVmValueError ScrVmValueException::Error() const noexcept
{
    return error_;
}

std::int32_t ScrVmValueException::Index() const noexcept
{
    return index_;
}

std::int32_t ScrVmValueTable::Read(const std::uint16_t index) const
{
    const auto& values = generationSlot_->Current().SaveParams2At30().values;
    if (index >= values.size())
    {
        throw ScrVmValueException(
            ScrVmValueError::IndexOutOfRange,
            index,
            "SCR VM value index is outside the 512-slot table");
    }
    return values[index];
}

std::int32_t ScrVmValueTable::ReadSigned(const std::int32_t index) const
{
    if (index == -1)
    {
        throw ScrVmValueException(
            ScrVmValueError::IndexMinusOne,
            index,
            "SCR VM GetValue received the reserved -1 index");
    }
    const auto& values = generationSlot_->Current().SaveParams2At30().values;
    if (index < 0 || static_cast<std::size_t>(index) >= values.size())
    {
        throw ScrVmValueException(
            ScrVmValueError::IndexOutOfRange,
            index,
            "SCR VM value index is outside the 512-slot table");
    }
    return values[static_cast<std::size_t>(index)];
}

const ScrVmParamsGeneration& ScrVmValueTable::CurrentParamsGeneration() const
{
    return generationSlot_->Current();
}

ScrVmParamsGeneration& ScrVmValueTable::CurrentParamsGeneration()
{
    return generationSlot_->Current();
}

bool ScrVmValueTable::UsesParamsGenerationSlot(
    const ScrVmParamsGenerationBinding& generationSlot) const noexcept
{
    return generationSlot_ == generationSlot;
}

void ScrVmValueTable::SetMessageFormatIndexAt60(const std::int32_t index)
{
    generationSlot_->Current().messageFormatIndexAt60 = index;
}

void ScrVmValueTable::SetInstructionIndexAtSaveParams08(const std::int32_t index)
{
    generationSlot_->Current().SaveParams2At30().instructionIndexAtSaveParams08 = index;
}

void ScrVmValueTable::Clear()
{
    // CSaveParams2(0x46CE30) 在每个新 CParams generation 中清零完整 0x800 字节值区。
    generationSlot_->Current().SaveParams2At30().values.fill(0);
}

void ScrVmValueTable::Write(const std::uint16_t index, const std::int32_t value)
{
    auto& values = generationSlot_->Current().SaveParams2At30().values;
    if (index >= values.size())
    {
        throw ScrVmValueException(
            ScrVmValueError::IndexOutOfRange,
            index,
            "SCR VM value index is outside the 512-slot table");
    }
    values[index] = value;
}

void ScrVmValueTable::WriteSigned(
    const std::int32_t index,
    const std::int32_t value)
{
    if (index == -1)
    {
        throw ScrVmValueException(
            ScrVmValueError::IndexMinusOne,
            index,
            "SCR VM SetValue received the reserved -1 index");
    }
    auto& values = generationSlot_->Current().SaveParams2At30().values;
    if (index < 0 || static_cast<std::size_t>(index) >= values.size())
    {
        throw ScrVmValueException(
            ScrVmValueError::IndexOutOfRange,
            index,
            "SCR VM value index is outside the 512-slot table");
    }
    values[static_cast<std::size_t>(index)] = value;
}
}
