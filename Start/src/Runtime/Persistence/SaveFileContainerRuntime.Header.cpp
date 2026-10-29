#include "SaveFileContainerRuntime.Internal.h"
#include <Runtime/EnvironmentObjectReference.h>
#include <cstring>

namespace KaGuYa::Start::Runtime::SaveFileContainerDetail
{
namespace
{
using U = std::uint32_t;
template<class T> T Read(const void* p, U offset = 0) noexcept
{ T value; std::memcpy(&value, static_cast<const std::byte*>(p) + offset, sizeof(value)); return value; }
const char* Chars(const RuntimeCString& slot) noexcept
{ return *reinterpret_cast<const char* const volatile*>(&slot); }
U NativeDword(const void* p, U offset = 0) noexcept
{ return *reinterpret_cast<const volatile U*>(static_cast<const std::byte*>(p) + offset); }
U Length(const char* chars) noexcept { return NativeDword(chars - 12); }
U Count(const YtlReferenceControlRuntime* control) noexcept { return control ? NativeDword(control, 0x10) : 0; }
std::byte* Address(U base, U bias, const YtlReferenceControlRuntime* control, U index = 0) noexcept
{
    if (!base) return nullptr;
    return reinterpret_cast<std::byte*>(base + (bias + index) * (control ? NativeDword(control, 0x0C) : 0));
}
void Compare(const std::byte* buffer, const char* magic, U length)
{
    // 原4C761B..4C766F先DWORD，失配/尾部逐byte；不以NUL截断，不合并为strcmp。
    U remaining = length - 4u;
    if (length >= 4u)
    {
        for (;;)
        {
            if (Read<U>(buffer) != Read<U>(magic)) break;
            buffer += 4; magic += 4;
            const U before = remaining;
            remaining -= 4;
            if (before < 4u) break;
        }
    }
    if (remaining == 0xFFFFFFFCu) return;
    if (Read<std::uint8_t>(buffer) != Read<std::uint8_t>(magic)) throw 0;
    if (remaining == 0xFFFFFFFDu) return;
    if (Read<std::uint8_t>(buffer, 1) != Read<std::uint8_t>(magic, 1)) throw 0;
    if (remaining == 0xFFFFFFFEu) return;
    if (Read<std::uint8_t>(buffer, 2) != Read<std::uint8_t>(magic, 2)) throw 0;
    if (remaining == 0xFFFFFFFFu) return;
    if (Read<std::uint8_t>(buffer, 3) != Read<std::uint8_t>(magic, 3)) throw 0;
}
}
void WriteContainerHeader(ContainerFile& file, WorkBuffer& work, const SaveFileFormatProfile& profile, U seed)
{
    if (!profile.nativeMagicSlot)
    {
        work.Allocate(36);
        std::memcpy(work.Data(), profile.magicBytes.data(), 32);
        std::memcpy(work.Data() + 32, &seed, 4);
        file.Write(work.Data(), work.Size());
        return;
    }
    const char* first = Chars(*profile.nativeMagicSlot);
    work.Allocate(Length(first) + 4u); // 4C7C47..5B；分配回调之后不能沿用该chars/长度。
    const auto& live = work.Native().RawSlot();
    auto* control = live.controlAt08;
    const U base = live.baseAt00, bias = live.biasAt04; // 原ESI/EBX/EDI跨三个CRT内存操作固定。
    const U zeroCount = Count(control);
    auto* zeroAt = Address(base, bias, control);
    std::memset(zeroAt, 0, zeroCount);
    const char* copy = Chars(*profile.nativeMagicSlot); // 4C7C9A。
    auto* copyAt = Address(base, bias, control); // 4C7CAA..BA先算目标，再4C7CBC读Lcopy。
    const U copyCount = Length(copy);
    std::memmove(copyAt, copy, copyCount);
    const char* offset = Chars(*profile.nativeMagicSlot); // 4C7CC6：再取Lseed，不沿用Lcopy。
    const U seedOffset = Length(offset); // base为空也必须先读此长度，不由优化器删掉故障。
    auto* seedAt = Address(base, bias, control, seedOffset);
    std::memcpy(seedAt, &seed, 4);
    const U outputCount = Count(control);
    auto* output = Address(base, bias, control);
    file.Write(output, outputCount); // 一次头Write，count来自缓存control的当前字段。
}
U ReadContainerHeader(ContainerFile& file, WorkBuffer& work, const SaveFileFormatProfile& profile, U& firstSize)
{
    if (!profile.nativeMagicSlot)
    {
        work.Allocate(36);
        file.Read(work.Data(), work.Size());
        if (std::memcmp(work.Data(), profile.magicBytes.data(), 32)) throw 0;
        firstSize = 32;
        return Read<U>(work.Data(), 32);
    }
    const char* first = Chars(*profile.nativeMagicSlot);
    work.Allocate(Length(first) + 4u); // 4C758E..5A1，不预清原生读取buffer。
    const auto& live = work.Native().RawSlot();
    auto* control = live.controlAt08;
    const U bias = live.biasAt04;
    const U inputCount = Count(control);
    auto* input = Address(live.baseAt00, bias, control);
    file.Read(input, inputCount);
    const char* compared = Chars(*profile.nativeMagicSlot); // 4C75F2：IO返回后重取Src。
    firstSize = Length(compared);
    // 比较用当前base，但沿用IO前的bias/control；seed的43D230则重新读完整live Ref。
    Compare(Address(live.baseAt00, bias, control), compared, firstSize);
    return Read<U>(work.Native().DataAt(firstSize));
}
}
