#include <Runtime/EnvironmentArchiveNative18.h>
#include <Runtime/FileManagerRootRuntime.h>
#include "NativeSerialize18.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace NativeSerialize18Detail;
thread_local FileManagerRootRuntime* activeKeys = nullptr;
using Count = U(__thiscall*)(const void*);
using Get = std::uint8_t(__thiscall*)(const void*, U);
using Set = void(__thiscall*)(void*, U, std::uint8_t);
FileManagerRootRuntime& Keys()
{
    if (!activeKeys) throw std::logic_error("Native environment archive key source is not bound");
    return *activeKeys;
}
U KeyCount(const ByteArrayReferenceRuntime& bytes) noexcept
{
    const auto* control = ReadAt<const void*>(&bytes.RawSlot(), 8);
    return control ? ReadAt<U>(control, 0x10) : 0;
}
std::uint8_t NormalizedKey(FileManagerRootRuntime& keys, U index)
{
    for (;;)
    {
        auto* counted = keys.CaptureManagerAt413BE0();
        const U count = KeyCount(counted->KeyReferenceAt1C());
        auto* current = keys.CaptureManagerAt413BE0(); // 47CB4E在signed比较之前独立取源。
        const auto& bytes = current->KeyReferenceAt1C();
        if (std::bit_cast<int>(index) >= std::bit_cast<int>(count))
        {
            index -= KeyCount(bytes); // 原减第二个manager的当前长度，不取模或减首次count。
            continue;
        }
        const auto& ref = bytes.RawSlot();
        const U base = ReadAt<U>(&ref);
        U address = 0;
        if (base)
        {
            const auto* control = ReadAt<const void*>(&ref, 8);
            const U stride = control ? ReadAt<U>(control, 0x0C) : 0;
            address = base + (ReadAt<U>(&ref, 4) + index) * stride;
        }
        const auto value = *reinterpret_cast<const volatile std::uint8_t*>(address);
        return static_cast<std::uint8_t>(value ? value : 0xFFu);
    }
}
std::uint8_t ReadKey(void* archive, U baseOffset, U index, FileManagerRootRuntime& keys)
{
    static_cast<void>(keys.CaptureManagerAt413BE0()); // 48BA74/498154/48B055/77先Ensure。
    const U keyIndex = ReadAt<U>(archive, baseOffset) + index;
    return NormalizedKey(keys, keyIndex);
}
U RuntimeCount(const void* archive) { return Slot<Count>(archive, 8)(archive); }
}
EnvironmentArchiveNative18Scope::EnvironmentArchiveNative18Scope(FileManagerRootRuntime& keys) noexcept
    : previous_(activeKeys) { activeKeys = &keys; }
EnvironmentArchiveNative18Scope::~EnvironmentArchiveNative18Scope() noexcept { activeKeys = previous_; }
void SerializeSecretFlagsNative18(void* archive, void* serializer, FileManagerRootRuntime& keys)
{
    U remaining;
    if (Mode(serializer))
    {
        remaining = RuntimeCount(archive);
        Argument(serializer, 0x58, &remaining);
        for (U index = 0; std::bit_cast<int>(index) < std::bit_cast<int>(RuntimeCount(archive)); ++index)
        {
            std::uint8_t encoded = 0;
            if (Slot<Get>(archive, 0x0C)(archive, index)) encoded = ReadKey(archive, 8, index, keys);
            Argument(serializer, 0x50, &encoded);
        }
        return;
    }
    Argument(serializer, 0x58, &remaining);
    for (U index = 0; std::bit_cast<int>(index) < std::bit_cast<int>(RuntimeCount(archive)); ++index)
    {
        if (!remaining) Slot<Set>(archive, 0x10)(archive, index, 0);
        else
        {
            std::uint8_t encoded;
            Argument(serializer, 0x50, &encoded);
            static_cast<void>(keys.CaptureManagerAt413BE0());
            const U keyIndex = ReadAt<U>(archive, 8) + index;
            const auto* cachedTable = ReadAt<const void*>(archive); // 48BB03/4981E3先缓存表。
            const auto key = NormalizedKey(keys, keyIndex);
            ReadAt<Set>(cachedTable, 0x10)(archive, index, static_cast<std::uint8_t>(encoded == key));
            --remaining;
        }
    }
}
void SerializeGlobalParamsNative18(void* archive, void* serializer, FileManagerRootRuntime& keys)
{
    const bool writing = Mode(serializer) != 0;
    auto* words = reinterpret_cast<std::uint16_t*>(Plus(archive, 0x0C));
    for (U index = 0; index < 0x20; ++index)
    {
        const auto low = ReadKey(archive, 4, index, keys);
        const auto high = ReadKey(archive, 8, index, keys);
        const auto key = static_cast<std::uint16_t>(low | (U(high) << 8));
        std::uint16_t encoded;
        if (writing) encoded = static_cast<std::uint16_t>(words[index] ^ key);
        Argument(serializer, 0x40, &encoded);
        if (!writing) words[index] = static_cast<std::uint16_t>(encoded ^ key);
    }
}
void SerializeSecretFlagsNative18(void* archive, void* serializer)
{ SerializeSecretFlagsNative18(archive, serializer, Keys()); }
void SerializeGlobalParamsNative18(void* archive, void* serializer)
{ SerializeGlobalParamsNative18(archive, serializer, Keys()); }
}
