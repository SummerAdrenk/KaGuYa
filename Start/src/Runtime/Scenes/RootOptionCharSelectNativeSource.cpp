#include "RootOptionCharSelectNativeSource.Internal.h"
#include <Runtime/EnvironmentStartupRuntime.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
using namespace RootOptionCharSelectNativeDetail;

RootOptionCharSelectNativeSource::RootOptionCharSelectNativeSource(
    EnvironmentStartupRuntime& environment, IEnvironmentRootMemory& memory) noexcept
    : environment_(environment), memory_(memory) {}

GuiClickedEventReferences* RootOptionCharSelectNativeSource::NewClickedEventManagerAt4E4AAD()
{ return NewManager(memory_); }

void RootOptionCharSelectNativeSource::AdoptClickedEventManagerAt4D7B90(
    YtlReferenceSlotRuntime& destination, GuiClickedEventReferences* owned)
{ AdoptManager(destination, owned, memory_); }

std::int32_t RootOptionCharSelectNativeSource::PollClickedEventManagerAt50FDB0(
    GuiClickedEventReferences* manager, std::int32_t clearMatched)
{ return PollManager(manager, clearMatched); }

const std::uint8_t* RootOptionCharSelectNativeSource::AvailabilityAt4E4C74(std::int32_t index)
{
    // 4E4B00..4E4BF7 与4180C0共用601160/6198C8及相同锁EH。
    // 必须用服务所借的同一owner；外层Startup::Capture的null->logic_error不等价。
    const auto* root = environment_.RootSource().CaptureAt4180C0();
    const auto* begin = ReadAt<YtlReferenceSlotRuntime*>(root, 0x30);
    const auto* end = ReadAt<YtlReferenceSlotRuntime*>(root, 0x34);
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end) -
        reinterpret_cast<std::uintptr_t>(begin));
    // 4E4C0E..4E4C1C：signed除12后用unsigned JBE 比较1，不检查control.count。
    const auto count = static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
    std::uintptr_t saved = 0;
    if (count > 1u)
    {
        const auto* slot = reinterpret_cast<const YtlReferenceSlotRuntime*>(
            reinterpret_cast<std::uintptr_t>(begin) + 0x0Cu);
        saved = ResolveWithCapturedBase(slot, ReadAt<std::uintptr_t>(slot, 0));
    }

    // slot1 是CSaveData90h；+1C是CSoundParams44h Ref，不是slot0 CSysData68h。
    // 4E4C47首先读+1C；空Save仍访问1Ch，不先规范化成null或false。
    const auto* soundSlot = reinterpret_cast<const YtlReferenceSlotRuntime*>(saved + 0x1Cu);
    const auto soundBase = ReadAt<std::uintptr_t>(soundSlot, 0);
    const auto sound = ResolveWithCapturedBase(soundSlot, soundBase);
    const auto* receiver = reinterpret_cast<const void*>(sound);
    const auto* table = ReadAt<const void*>(receiver, 0);
    using Display = std::uint8_t* (__thiscall*)(void*, std::int32_t);
    const auto display = ReadAt<Display>(table, 0x84);
    // 4E4C74只调用虚槽并返回借用byte；caller的4E4C76才cmp byte[eax],0。
    return display(reinterpret_cast<void*>(sound), index);
}
}
