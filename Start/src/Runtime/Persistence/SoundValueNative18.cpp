#include <Runtime/SoundValueNative18.h>
#include <Runtime/SoundKeyValueNative.h>
#include "NativeSerialize18.Internal.h"
#include <new>

#if !defined(_MSC_VER) || !defined(_M_IX86)
#error SoundValueNative18 requires the MSVC x86 raw-kind MOV boundary.
#endif

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace NativeSerialize18Detail;
using ArchiveSlot00 = void (__thiscall*)(void*, void*);

void DispatchArchiveAt48BDB2(void* body, void* native18h)
{
    // 48BDB2/48BF51/48C095/48C125：每次取当前正文表，不走+5C或静态kind分派。
    // 原空正文在取表处硬故障；不新增空记录或legacy适配回退。
    Slot<ArchiveSlot00>(body, 0)(body, native18h);
}

void* ResolveValueAt48BD9C(const void* reference) noexcept
{
    const U base = ReadAt<U>(reference);
    if (!base) return nullptr;
    const auto* control = ReadAt<const void*>(reference, 8);
    const U stride = control ? ReadAt<U>(control, 0x0C) : 0;
    const U bias = ReadAt<U>(reference, 4);
    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(base + bias * stride));
}

std::int32_t ChildCountAt48C040(const void* body) noexcept
{
    // 48C040..48C059 / 48C0D0..48C0E4：先end后begin，32位差按有符号/12截零。
    // Internal::Count仅覆盖1/2/4字节元素，不能拿来计算12字节Ref数量。
    const U end = ReadAt<U>(body, 0x10);
    const U begin = ReadAt<U>(body, 0x0C);
    return std::bit_cast<std::int32_t>(end - begin) / 12;
}

void* ResolveIndexedChildAt48C0EA(void* body, U byteOffset) noexcept
{
    auto* begin = ReadAt<B*>(body, 0x0C);
    auto* reference = Plus(begin, byteOffset);
    const U base = ReadAt<U>(reference);
    if (!base) return nullptr;
    const auto* control = ReadAt<const void*>(reference, 8);
    const U stride = control ? ReadAt<U>(control, 0x0C) : 0;
    // 48C112重新读begin，再从该范围读bias，不把Ref复制成早期快照。
    begin = ReadAt<B*>(body, 0x0C);
    const U bias = ReadAt<U>(Plus(begin, byteOffset), 4);
    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(base + bias * stride));
}

__declspec(noinline) U ReadKeyKindAt48BD75(void* body) noexcept
{
    // x86/MSVC v143边界：48BB40的48BC77..48BCA2没有写栈KeyValue+08。
    // 48BD75仍比较该地址的实际DWORD；禁止以未初始化C++成员求值或编造kind。
    // 仅输出槽先初始化，随后必经MOV覆盖；既不读取其初值，也不回写正文+08。
    U bits = 0;
    __asm
    {
        mov eax, body
        mov eax, dword ptr [eax + 8]
        mov bits, eax
    }
    return bits;
}

class ExcessChildTemporary final
{
public:
    explicit ExcessChildTemporary(IRuntimeStringManagerSource& strings)
    {
        auto* child = ::new (storage_) SoundChildValueNative(SoundNativeShellTag{});
        // 48C133/48C14D独立取源/GetNil；第二次失败不清理第一个串。
        InitializeEmptyRuntimeCString(child->textAt10, strings);
        InitializeEmptyRuntimeCString(child->textAt14, strings);
        child->stringsAt18 = {};
        child->stringsAt28 = {};
        // 48C199才进入state0；+04/+08/+0C/+24及padding均不补初值。
        armed_ = true;
    }
    ~ExcessChildTemporary() noexcept
    {
        if (armed_) Destroy(); // 59AD30 -> 465050；清理自身抛出时终止。
    }
    ExcessChildTemporary(const ExcessChildTemporary&) = delete;
    ExcessChildTemporary& operator=(const ExcessChildTemporary&) = delete;
    SoundChildValueNative* Body() noexcept
    {
        return reinterpret_cast<SoundChildValueNative*>(storage_);
    }
    void Finish() noexcept
    {
        armed_ = false; // 48C1BC先撤state0，再调用465050，不重试清理。
        Destroy();
    }
private:
    void Destroy() noexcept
    {
        // 469B00(flags=0)复用465050：28vector、18vector、14串、10串、Base退役。
        // 不为序列化改表；这里仅执行现public删除口的真实成员退役。
        Body()->SoundChildValueNative::DeleteAtVslot04(0);
    }
    alignas(SoundChildValueNative) B storage_[sizeof(SoundChildValueNative)];
    bool armed_ = false;
};
}

void SerializeSoundBasicValueNative18(void* body, void* native18h)
{
    using namespace NativeSerialize18Detail;
    // 48C9CE/48C9D7：没有mode读取；每槽独立重取native18动态表。
    Argument(native18h, 0x54, Plus(body, 4));
    Argument(native18h, 0x44, Plus(body, 8));
}

void SerializeSoundChildValueNative18(void* body, void* native18h)
{
    using namespace NativeSerialize18Detail;
    // 48BFB0..48BFDE：原来就是内联Basic两槽，再序列化+0C。
    SerializeSoundBasicValueNative18(body, native18h);
    Argument(native18h, 0x54, Plus(body, 0x0C));
}

void SerializeSoundEffectValueNative18(void* body, void* native18h)
{
    using namespace NativeSerialize18Detail;
    SerializeSoundBasicValueNative18(body, native18h);
    if (Mode(native18h)) // 48C033..48C03E仅一次；count回调改mode也不重新选支。
    {
        U count = static_cast<U>(ChildCountAt48C040(body));
        Argument(native18h, 0x58, &count);
        if (!count) return; // 48C064读回调后的count，不用初始数量替代。
        auto* current = ReadAt<B*>(body, 0x0C);
        if (current == ReadAt<B*>(body, 0x10)) return;
        do
        {
            DispatchArchiveAt48BDB2(ResolveValueAt48BD9C(current), native18h);
            current = Plus(current, 12);
        } while (current != ReadAt<B*>(body, 0x10)); // 48C09A逐项晚读end。
        return;
    }

    U count;
    Argument(native18h, 0x58, &count); // 48C0B3：不清表、不resize，负count不循环。
    U byteOffset = 0;
    for (U index = 0; std::bit_cast<std::int32_t>(index) < std::bit_cast<std::int32_t>(count);
        ++index, byteOffset += 12)
    {
        if (std::bit_cast<std::int32_t>(index) < ChildCountAt48C040(body))
        {
            DispatchArchiveAt48BDB2(ResolveIndexedChildAt48C0EA(body, byteOffset), native18h);
        }
        else
        {
            ExcessChildTemporary temporary(RequireNativeSerialize18Services().strings);
            // 48C191..48C1B6原为三个直接native18槽，非临时Child的slot00调用。
            SerializeSoundChildValueNative18(temporary.Body(), native18h);
            temporary.Finish();
        }
    }
}

void SerializeSoundKeyValueNative18(void* body, void* native18h)
{
    using namespace NativeSerialize18Detail;
    if (Mode(native18h)) // 48BD2D..48BD38仅一次；keyAt04不进入文件记录。
    {
        Argument(native18h, 0x58, Plus(body, 8));
        DispatchArchiveAt48BDB2(ResolveValueAt48BD9C(Plus(body, 0x0C)), native18h);
        return;
    }

    U kind;
    Argument(native18h, 0x58, &kind);
    if (kind == ReadKeyKindAt48BD75(body)) // 回调后才读正文+08，包括Base的原栈残值。
    {
        DispatchArchiveAt48BDB2(ResolveValueAt48BD9C(Plus(body, 0x0C)), native18h);
        return;
    }

    YtlReferenceSlotRuntime temporary{};
    SoundNativeTemporary lifetime(temporary); // 48BDEC..48BDFF：factory之前即有state0。
    // 48BE18/48BE86/48BEDE保持现factory边界；未知不匹配kind由原factory抛int 0。
    ConstructSoundValueReferenceAt48BE18(temporary, kind, RequireNativeSerialize18Services().memory);
    auto* cached = temporary.controlAt08;
    // 48BF4C直接调用factory的bias=0正文slot00；不能为legacy测试改为具名静态核。
    DispatchArchiveAt48BDB2(reinterpret_cast<void*>(temporary.baseAt00), native18h);
    lifetime.Finish(cached); // 48BF57进state1终止边界；EH则59AD00重读Ref当前control。
}
}
