#include "SaveFileContainerNative.Blocks.h"
#include "NativeSerialize18.Internal.h"
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime::SaveFileContainerDetail
{
namespace
{
using U = std::uint32_t;
U Word(const void* p, U offset = 0) noexcept
{ return *reinterpret_cast<const volatile U*>(static_cast<const std::byte*>(p) + offset); }
U Count(const void* control) noexcept { return control ? Word(control, 0x10) : 0; }
std::byte* CachedAddress(U base, U bias, const void* control) noexcept
{ return base ? reinterpret_cast<std::byte*>(base + bias * (control ? Word(control, 0x0C) : 0)) : nullptr; }
U Size(NativeSerialize18Body& body) noexcept
{
    const U end = Word(&body, 8);
    const U begin = Word(&body, 4);
    return end - begin;
}
void* Begin(NativeSerialize18Body& body) noexcept { return reinterpret_cast<void*>(Word(&body, 4)); }
void ReleaseSecondControl(WorkBuffer& work, YtlReferenceControlRuntime* control)
{
    // 4C8037..8064消费第二块缓存的ESI，不以当前Ref替代它；回调之后才清control/base。
    // 4C804B/8053仍在state7及5F11C4 catch范围，普通虚槽异常不能额外terminate。
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        using Destroy = void(__thiscall*)(void*);
        using Delete = void(__thiscall*)(void*, U);
        if (control->owningAt08)
            (*reinterpret_cast<Destroy const*>(Word(control)))(control);
        (*reinterpret_cast<Delete const*>(Word(control) + 4))(control, 1);
    }
    auto& slot = work.Native().RawSlot();
    slot.controlAt08 = nullptr;
    slot.baseAt00 = 0; // 原bias不清零。
}
}
static_assert(std::is_trivially_default_constructible_v<NativeSerialize18Body>);
SaveSerializerLife::SaveSerializerLife(SaveSerializerStorage& storage) noexcept
    : body_(*::new (&storage.body) NativeSerialize18Body)
{
    // 原容器内联构造清mode整个DWORD，而通用40BB30仅写byte，不能抹平padding规则。
    body_.vptrAt00 = &NativeSerialize18Methods();
    body_.bytesAt04.begin = nullptr;
    body_.bytesAt04.capacity = nullptr;
    body_.bytesAt04.end = nullptr;
    body_.cursorAt14 = 0;
    const U writing = 1;
    std::memcpy(reinterpret_cast<std::byte*>(&body_) + 0x10, &writing, 4);
}
SaveSerializerLife::~SaveSerializerLife() noexcept
{ if (armed_) DestroyNativeSerialize18At463E60(&body_); }
void SaveSerializerLife::NormalWithUnwind()
{
    // 三次写及第三次读4C78A9保留当前state，正常Free抛出后仍有463E60展开资格。
    DestroyNativeSerialize18At463E60(&body_);
    armed_ = false;
}
void SaveSerializerLife::NormalAfterStateDrop()
{
    // 4C772C/4C77EB先退到state4；正常Free失败不能再补一次serializer清理。
    armed_ = false;
    DestroyNativeSerialize18At463E60(&body_);
}
YtlReferenceControlRuntime* WriteNativeSaveBlock(ContainerFile& file, WorkBuffer& work,
    NativeSerialize18Body& body, U& size, U seed, bool first)
{
    size = Size(body);
    work.Allocate(size);
    auto& slot = work.Native().RawSlot();
    auto* control = reinterpret_cast<YtlReferenceControlRuntime*>(Word(&slot, 8));
    const U zeroCount = Count(control);
    U base = 0, bias = 0;
    std::byte* zeroAt;
    if (first)
    {
        // 第一块缓存control/base/bias；有base时先读stride再读bias。
        base = Word(&slot);
        if (base)
        {
            const U stride = control ? Word(control, 0x0C) : 0;
            bias = Word(&slot, 4);
            zeroAt = reinterpret_cast<std::byte*>(base + bias * stride);
        }
        else { bias = Word(&slot, 4); zeroAt = nullptr; }
    }
    else zeroAt = reinterpret_cast<std::byte*>(work.Native().DataAt(0));
    std::memset(zeroAt, 0, zeroCount);
    if (first)
    {
        auto* copyAt = CachedAddress(base, bias, control);
        auto* source = Begin(body);
        std::memmove(copyAt, source, size);
    }
    else
    {
        auto* source = Begin(body); // 原push Src先于43D230调用。
        auto* copyAt = work.Native().DataAt(0);
        std::memmove(copyAt, source, size);
    }
    work.XorAt4C7AA0(seed);
    file.Write(&size, 4);
    auto* output = first ? CachedAddress(base, bias, control) : reinterpret_cast<std::byte*>(work.Native().DataAt(0));
    file.Write(output, size); // 首两块即使零长度也写正文；length回调后重读同一个size。
    return control;
}
void WriteNativeHistoryBlock(ContainerFile& file, WorkBuffer& work, NativeSerialize18Body& body,
    U& size, U seed, YtlReferenceControlRuntime* secondControl)
{
    size = Size(body);
    if (!size) ReleaseSecondControl(work, secondControl);
    else
    {
        work.Allocate(size);
        const auto& slot = work.Native().RawSlot();
        auto* control = reinterpret_cast<void*>(Word(&slot, 8));
        const U zeroCount = Count(control);
        auto* zeroAt = work.Native().DataAt(0);
        std::memset(zeroAt, 0, zeroCount);
        auto* source = Begin(body);
        auto* copyAt = work.Native().DataAt(0);
        std::memmove(copyAt, source, size);
        work.XorAt4C7AA0(seed);
    }
    file.Write(&size, 4);
    // 4C80CC判当前base，不判size；写length回调后可有新发布的非空Ref。
    if (Word(&work.Native().RawSlot())) file.Write(work.Native().DataAt(0), size);
}
void ReadNativeSaveBlock(ContainerFile& file, WorkBuffer& work, U size, U seed)
{
    work.Allocate(size);
    const auto& slot = work.Native().RawSlot();
    const U count = Count(reinterpret_cast<void*>(Word(&slot, 8)));
    auto* destination = work.Native().DataAt(0);
    file.Read(destination, count);
    work.XorAt4C7AA0(seed);
}
void ImportNativeSaveBlock(NativeSerialize18Body& body, WorkBuffer& work)
{
    const auto result = NativeSerialize18Detail::ImportReference(&body, nullptr, &work.Native().RawSlot());
    if (result) throw 4;
    body.writingAt10 = static_cast<std::uint8_t>(result);
    body.cursorAt14 = static_cast<U>(result);
}
}
