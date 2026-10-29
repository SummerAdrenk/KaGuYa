#include <Runtime/ScrVmScriptCommandState.h>

#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
ScrVmScriptCommandState::ScrVmScriptCommandState() noexcept
    : prefix_(fixturePrefix_), tail_(fixtureTail_), publication_(fixturePublication_)
{
}

ScrVmScriptCommandState::ScrVmScriptCommandState(
    ScriptNativeBody& body, ScriptNativePublication& publication) noexcept
    : body_(&body), prefix_(body.commandAt04), tail_(body.commandAt24), publication_(publication)
{
}

ScrVmScriptCommandState::ScrVmScriptCommandState(const ScrVmScriptCommandState& source) noexcept
    : ScrVmScriptCommandState()
{
    *this = source;
}

ScrVmScriptCommandState& ScrVmScriptCommandState::operator=(const ScrVmScriptCommandState& source) noexcept
{
    if (this == &source) return *this;
    const auto mask = source.publication_.commandMask & CommandFieldsMask;
    if (mask & FlagsAt04Mask) prefix_.flags = source.prefix_.flags;
    for (unsigned index = 0; index != 4; ++index)
        if (mask & (ReloadByteAt08Mask << index)) prefix_.reload[index] = source.prefix_.reload[index];
    for (unsigned index = 0; index != 5; ++index)
        if (index != 1 && (mask & (StateByteAt24Mask << index))) tail_.bytes[index] = source.tail_.bytes[index];
    // 未发布字段不求值、不写入；只撤其有效位，保留receiver的+25及其它适配记录。
    publication_.commandMask = (publication_.commandMask & ~CommandFieldsMask) | mask;
    return *this;
}

bool operator==(const ScrVmScriptCommandState& left, const ScrVmScriptCommandState& right) noexcept
{
    const auto mask = left.publication_.commandMask & ScrVmScriptCommandState::CommandFieldsMask;
    if (mask != (right.publication_.commandMask & ScrVmScriptCommandState::CommandFieldsMask)) return false;
    if ((mask & ScrVmScriptCommandState::FlagsAt04Mask) && left.prefix_.flags != right.prefix_.flags) return false;
    for (unsigned index = 0; index != 4; ++index)
        if ((mask & (ScrVmScriptCommandState::ReloadByteAt08Mask << index)) &&
            left.prefix_.reload[index] != right.prefix_.reload[index]) return false;
    for (unsigned index = 0; index != 5; ++index)
        if (index != 1 && (mask & (ScrVmScriptCommandState::StateByteAt24Mask << index)) &&
            left.tail_.bytes[index] != right.tail_.bytes[index]) return false;
    return true;
}

void ScrVmScriptCommandState::RequirePublication(unsigned mask, const char* message) const
{
    if (!Published(mask)) throw std::logic_error(message);
}

std::uint32_t ScrVmScriptCommandState::ReadFlagsAt04() const
{
    RequirePublication(FlagsAt04Mask, "CScript+04 read before publication");
    return prefix_.flags;
}

void ScrVmScriptCommandState::WriteFlagsAt04(std::uint32_t value) noexcept
{
    prefix_.flags = value;
    publication_.commandMask |= FlagsAt04Mask;
}

std::uint8_t ScrVmScriptCommandState::ReadReloadByteAt08() const
{
    RequirePublication(ReloadByteAt08Mask, "CScript+08 byte read before publication");
    return prefix_.reload[0];
}

void ScrVmScriptCommandState::WriteReloadByteAt08(std::uint8_t value) noexcept
{
    prefix_.reload[0] = value;
    publication_.commandMask |= ReloadByteAt08Mask;
}

std::int32_t ScrVmScriptCommandState::ReadReloadDwordAt08() const
{
    RequirePublication(ReloadDwordAt08Mask, "CScript+08 DWORD read before all four bytes are published");
    std::uint32_t bits = 0;
    for (unsigned index = 0; index != 4; ++index)
        bits |= static_cast<std::uint32_t>(prefix_.reload[index]) << (index * 8u);
    return std::bit_cast<std::int32_t>(bits);
}

void ScrVmScriptCommandState::WriteReloadDwordAt08(std::int32_t value) noexcept
{
    const auto bits = std::bit_cast<std::uint32_t>(value);
    for (unsigned index = 0; index != 4; ++index)
        prefix_.reload[index] = static_cast<std::uint8_t>(bits >> (index * 8u));
    publication_.commandMask |= ReloadDwordAt08Mask;
}

std::uint8_t ScrVmScriptCommandState::ReadTail(unsigned index, unsigned mask, const char* message) const
{
    RequirePublication(mask, message);
    return tail_.bytes[index];
}

void ScrVmScriptCommandState::WriteTail(unsigned index, unsigned mask, std::uint8_t value) noexcept
{
    tail_.bytes[index] = value;
    publication_.commandMask |= mask;
}
}
