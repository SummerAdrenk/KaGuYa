#include "CgDisplayResourceNative.Internal.h"
#include <mbstring.h>

extern "C" int __cdecl _purecall();
namespace KaGuYa::Start::Runtime
{
using namespace CgDisplayResourceNativeDetail;
namespace
{
class RetiredNamesControl final : public YtlReferenceControlRuntime
{
public:
    RetiredNamesControl() noexcept : YtlReferenceControlRuntime(UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(unsigned flags) override
    { if (flags & 1u) ::operator delete(this, std::size_t{0x14}); }
    unsigned strideAt0C, countAt10;
};
struct StringPrefix final
{
    RuntimeCString* elements;
    unsigned& completed;
    bool active = true;
    ~StringPrefix() noexcept
    { if (active) while (completed) DestroyRuntimeCStringAt40CD40(elements + --completed); }
};
class NamesControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit NamesControl(RuntimeCString* p) noexcept : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* elements = static_cast<RuntimeCString*>(payloadAt14);
        if (!elements) return;
        auto* cookie = reinterpret_cast<unsigned*>(elements) - 1;
        unsigned remaining = Read(*cookie);
        StringPrefix cleanup{elements, remaining};
        while (remaining) DestroyRuntimeCStringAt40CD40(elements + --remaining);
        cleanup.active = false;
        // 原43BB3B在元素回调后重新取cookie；删除大小DWORD回绕，不再使用分配时的饱和大小。
        ::operator delete[](cookie, std::size_t{4u + Read(*cookie) * 4u});
    }
    void DeleteControlAtVslot04(unsigned flags) override
    {
        // 410BA0只改ref_object表；退役构造不写任何原字段，也不清payload后的存储。
        auto* retired = ::new(static_cast<void*>(this)) RetiredNamesControl;
        if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
    }
};
static_assert(sizeof(RetiredNamesControl) == 0x14 && sizeof(NamesControl) == 0x18);
void RebuildNames(Array& out, int count, IRuntimeStringManagerSource& strings)
{
    auto& slot = out.RawSlot();
    ReleaseYtlReference(slot);
    if (count <= 0) return;
    const auto extent = std::uint32_t(count);
    const auto bytes = extent > (0xFFFFFFFFu - 4u) / 4u ? 0xFFFFFFFFu : 4u + extent * 4u;
    auto* cookie = static_cast<unsigned*>(::operator new[](std::size_t{bytes}));
    RuntimeCString* elements = nullptr;
    if (cookie)
    {
        *cookie = extent;
        elements = reinterpret_cast<RuntimeCString*>(cookie + 1);
        unsigned completed = 0;
        StringPrefix cleanup{elements, completed};
        for (; completed < extent; ++completed) InitializeEmptyRuntimeCString(elements[completed], strings);
        cleanup.active = false;
        // 412870没有raw数组块的外层EH删除资格，构造失败仅退已完成字符串前缀。
    }
    if (auto* current = Read(slot.controlAt08)) current->DeleteControlAtVslot04(1);
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(elements);
    auto* raw = ::operator new(std::size_t{0x18});
    auto* control = raw ? ::new(raw) NamesControl(elements) : nullptr;
    slot.controlAt08 = control;
    // 原412931直接写null+4；不借共享helper的bad_alloc安全转换让外层catch返回false。
    *reinterpret_cast<volatile unsigned*>(reinterpret_cast<std::uintptr_t>(control) + 4u) = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 4;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = extent;
}
}
RuntimeCStringArrayReference* BuildCgDisplayNamesNativeAt4AD900(Array& fresh,
    const ParamsPatternNative& pattern, int index, IRuntimeStringManagerSource& strings)
{
    ::new(&fresh) Array;
    struct ReturnUnwind final
    { Array& value; bool active = true; ~ReturnUnwind() noexcept { if (active) value.~Array(); } } cleanup{fresh};
    // 原先缓存integers.begin，signed index无负数保护；count来自该元素的control而非base。
    auto* begin = Read(pattern.integersAt10.begin);
    auto* end = Read(pattern.integersAt10.end);
    if (index < Distance(begin, end))
    {
        const auto count = Count(*Advance(begin, index));
        RebuildNames(fresh, count, strings);
        for (int i = 0;; i = Add(i, 1))
        {
            auto* outputControl = static_cast<EnvironmentObjectReferenceControl*>(Read(fresh.RawSlot().controlAt08));
            const auto size = outputControl ? std::bit_cast<int>(Read(outputControl->countAt10)) : 0;
            if (i >= size) break;
            // 4AD9B1每轮重取Pattern的integers.begin，不沿用分配前缓存。
            auto& source = *Advance(Read(pattern.integersAt10.begin), index);
            const auto item = Read(*Resolve<int>(source, i));
            const char* name = nullptr;
            if (item >= 0)
            {
                auto* items = Read(pattern.itemsAt04.begin);
                auto* last = Read(pattern.itemsAt04.end);
                if (item < Distance(items, last))
                    name = Chars(Field<RuntimeCString>(Resolve<void>(*Advance(items, item)), 4));
            }
            // 原负ID/越items范围/null名字均赋空串；有效ID的空正文仍保留实际非法访问边界。
            auto& destination = *Resolve<RuntimeCString>(fresh.RawSlot(), i);
            SetRuntimeCStringAt437680(destination, name,
                name ? std::bit_cast<int>(std::uint32_t(std::strlen(name))) : 0);
        }
    }
    cleanup.active = false;
    return &fresh;
}
int FindCgDisplayPatternNameNativeAt47A020(const ParamsPatternNative& pattern, const char* query)
{
    auto* begin = Read(pattern.itemsAt04.begin);
    auto* end = Read(pattern.itemsAt04.end);
    for (int i = 0; i < Distance(begin, end); i = Add(i, 1))
    {
        auto* body = Resolve<void>(*Advance(begin, i));
        if (!query) throw ATL::CAtlException(E_FAIL); // 空表不检查query，原47A077先解Ref后测query。
        if (!::_mbsicmp(reinterpret_cast<const unsigned char*>(Chars(Field<RuntimeCString>(body, 4))),
            reinterpret_cast<const unsigned char*>(query))) return i;
        end = Read(pattern.itemsAt04.end);
        begin = Read(pattern.itemsAt04.begin);
    }
    return -1;
}
}
