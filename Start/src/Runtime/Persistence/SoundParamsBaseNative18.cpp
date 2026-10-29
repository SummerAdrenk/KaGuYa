#include <Runtime/SoundParamsBaseNative18.h>
#include <Runtime/SoundValueNative18.h>
#include <Runtime/SoundKeyValueNative.h>
#include "NativeSerialize18.Internal.h"
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace NativeSerialize18Detail;
U Count(void* sound) noexcept
{
    const U end = ReadAt<U>(sound, 8);
    const U begin = ReadAt<U>(sound, 4);
    return static_cast<U>(std::bit_cast<std::int32_t>(end - begin) / 12);
}
void InvokeReference(const void* reference, void* serializer)
{
    const U base = ReadAt<U>(reference);
    U selected = 0;
    if (base)
    {
        const auto* control = ReadAt<const void*>(reference, 8);
        const U stride = control ? ReadAt<U>(control, 0x0C) : 0;
        selected = base + ReadAt<U>(reference, 4) * stride;
    }
    using Serialize = void(__thiscall*)(void*, void*);
    auto* body = reinterpret_cast<void*>(selected);
    Slot<Serialize>(body, 0)(body, serializer); // 48BBD0/48BC3F：原NULL也继续解引用。
}
union KeyStorage final
{
    alignas(SoundKeyValueNative) std::byte bytes[0x18];
    KeyStorage() noexcept {}
    ~KeyStorage() noexcept {}
};
struct KeyLife final
{
    void* body;
    bool armed = true;
    ~KeyLife() noexcept { if (armed) RetireSoundKeyValueStackNativeAt464A60(body); }
    void Normal() noexcept
    {
        armed = false; // 48BCBC先退state -1，不能在正常析构失败后额外重试。
        RetireSoundKeyValueStackNativeAt464A60(body);
    }
};
}
void ConstructSoundKeyValueStackNativeAt48BC77(void* raw)
{
    auto* body = ::new(raw) SoundKeyValueNative(SoundNativeShellTag{});
    auto& services = RequireNativeSerialize18Services();
    InitializeEmptyRuntimeCString(body->keyAt04, services.strings);
    body->valueAt0C = {};
    // kindAt08原本未写，不能设成0/FFFFFFFF或用自定义异常代替后续48BD00。
}
void RetireSoundKeyValueStackNativeAt464A60(void* raw) noexcept
{
    // 当前具体成员删除叶flags0保留Ref->CString->Base顺序，不删除栈正文。
    static_cast<SoundKeyValueNative*>(raw)->DeleteAtVslot04(0);
}
void SerializeSoundParamsBaseNative18At48BB40(void* sound, void* serializer)
{
    using namespace NativeSerialize18Detail;
    if (Mode(serializer))
    {
        U count = Count(sound);
        Argument(serializer, 0x58, &count);
        if (!count) return;
        auto* position = ReadAt<const B*>(sound, 4);
        for (; position != ReadAt<const B*>(sound, 8); position = Plus(position, 12))
            InvokeReference(position, serializer); // 当前end每项重读，不按已写count固定次数。
        return;
    }
    U count;
    Argument(serializer, 0x58, &count);
    for (U index = 0, offset = 0; std::bit_cast<int>(index) < std::bit_cast<int>(count); ++index, offset += 12)
    {
        if (std::bit_cast<int>(index) < std::bit_cast<int>(Count(sound)))
        {
            auto* begin = ReadAt<const B*>(sound, 4); // 48BC2F在Count后重新读begin。
            InvokeReference(Plus(begin, offset), serializer);
        }
        else
        {
            KeyStorage temporary;
            ConstructSoundKeyValueStackNativeAt48BC77(temporary.bytes);
            KeyLife life{temporary.bytes};
            // 原48BCB4直接调用48BD00，不经临时对象vptr，也不把超额项追加到表里。
            SerializeSoundKeyValueNative18(temporary.bytes, serializer);
            life.Normal();
        }
    }
}
}
