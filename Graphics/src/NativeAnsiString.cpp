#include "NativeAnsiString.h"
#include <atlstr.h>
#include <algorithm>
#include <bit>
#include <cstdint>
#include <cstring>
#include <limits>
#include <intrin.h>

namespace KaGuYa::Graphics
{
namespace
{
ATL::CStringData* StringHeader(const char* characters) noexcept
{
    static_assert(sizeof(void*) == 4 && sizeof(ATL::CStringData) == 16);
    static_assert(offsetof(ATL::CStringData, pStringMgr) == 0);
    static_assert(offsetof(ATL::CStringData, nDataLength) == 4);
    static_assert(offsetof(ATL::CStringData, nAllocLength) == 8);
    static_assert(offsetof(ATL::CStringData, nRefs) == 12);
    return reinterpret_cast<ATL::CStringData*>(
        reinterpret_cast<std::uintptr_t>(characters) - sizeof(ATL::CStringData));
}

char* Characters(ATL::CStringData* data) noexcept
{
    // 原100080B6直接add 10h；不对原危险空header分支补安全空串。
    return reinterpret_cast<char*>(reinterpret_cast<std::uintptr_t>(data) + sizeof(ATL::CStringData));
}

template<class Function> Function ManagerSlot(ATL::IAtlStringMgr* manager, std::size_t offset) noexcept
{
    // 原08080/07E80/07F10/081C0均为原始虚调用，不经过现代ATL接口的throw()。
    const std::byte* table;
    std::memcpy(&table, manager, sizeof(table));
    Function function;
    static_assert(sizeof(function) == sizeof(void*));
    std::memcpy(&function, table + offset, sizeof(function));
    return function;
}

void ReleaseStringHeader(ATL::CStringData* data)
{
    // 原100081C0/1000F2B9/1000F2DA：减持后才读manager，不清名字指针。
    if (_InterlockedDecrement(&data->nRefs) > 0) return;
    auto* manager = data->pStringMgr;
    using Free = void (__thiscall*)(ATL::IAtlStringMgr*, ATL::CStringData*);
    ManagerSlot<Free>(manager, 4)(manager, data);
}

}
NativeAnsiString::NativeAnsiString(Initialization initialization)
{
    // 原100080A7..B9先GetNil、再发布字符指针；GetNil抛出时尚无本名字的展开资格。
    // GetInstance本身无throw()声明；不使用CStringT::GetDefaultManager/构造/基类析构。
    auto* manager = ATL::CAtlStringMgr::GetInstance();
    using GetNil = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*);
    characters_ = Characters(ManagerSlot<GetNil>(manager, 12)(manager));
    if (initialization == Initialization::NilOnly) return;
    struct ConstructionUnwind
    {
        NativeAnsiString* value;
        ~ConstructionUnwind() noexcept(false)
        {
            if (value) ReleaseStringHeader(StringHeader(value->GetString()));
        }
    } unwind{this}; // 原100080BD激活s0，1004D110只退已经发布的名字数据。
    (void)Empty(); // 原100080FE -> 07FA0(length=0) -> 07E20；正常nil不另分配。
    unwind.value = nullptr;
}

NativeAnsiString::~NativeAnsiString() noexcept(false)
{
    Retire();
}

void NativeAnsiString::Retire()
{
    ReleaseStringHeader(StringHeader(characters_));
}

int NativeAnsiString::GetLength() const noexcept
{
    return StringHeader(characters_)->nDataLength;
}

const char* NativeAnsiString::Empty()
{
    auto* previous = StringHeader(characters_);
    auto* manager = previous->pStringMgr;
    // 原10007E26/7E2C：原长度为0时EAX留header，不改指针或计数。
    if (previous->nDataLength == 0) return reinterpret_cast<const char*>(previous);
    if (previous->nRefs < 0)
    {
        if (previous->nAllocLength < 0) throw ATL::CAtlException(E_INVALIDARG);
        previous->nDataLength = 0;
        char* result = characters_;
        *result = 0;
        return result;
    }
    // 原10007E55..77缓存旧manager；Free/GetNil抛出不发布新指针，也不补回滚旧引用。
    ReleaseStringHeader(previous);
    using GetNil = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*);
    char* result = Characters(ManagerSlot<GetNil>(manager, 12)(manager));
    characters_ = result;
    return result;
}

void NativeAnsiString::Fork(const int capacity)
{
    auto* previous = StringHeader(characters_);
    auto* manager = previous->pStringMgr;
    const int previousLength = previous->nDataLength;
    using Clone = ATL::IAtlStringMgr* (__thiscall*)(ATL::IAtlStringMgr*);
    manager = ManagerSlot<Clone>(manager, 16)(manager);
    using Allocate = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*, int, int);
    auto* next = ManagerSlot<Allocate>(manager, 0)(manager, capacity, 1);
    if (!next) throw ATL::CAtlException(E_OUTOFMEMORY); // 原10007EAF -> 10007F00。
    char* nextCharacters = Characters(next);
    const auto bytes = static_cast<std::uint32_t>((std::min)(previousLength, capacity)) + 1u;
    (void)memcpy_s(nextCharacters, bytes, Characters(previous), bytes);
    next->nDataLength = previousLength;
    // 原10007EDC/7EF7：退缓存的旧header，成功后才发布新体；失败不补删next。
    ReleaseStringHeader(previous);
    characters_ = nextCharacters;
}

void NativeAnsiString::Reallocate(const int capacity)
{
    auto* previous = StringHeader(characters_);
    auto* manager = previous->pStringMgr;
    if (previous->nAllocLength >= capacity || capacity <= 0) throw ATL::CAtlException(E_OUTOFMEMORY);
    using Reallocate = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*, ATL::CStringData*, int, int);
    auto* next = ManagerSlot<Reallocate>(manager, 8)(manager, previous, capacity, 1);
    if (!next) throw ATL::CAtlException(E_OUTOFMEMORY);
    characters_ = Characters(next); // 原10007F37/3A，正常返回后才发布。
}

void NativeAnsiString::PrepareWrite(int length)
{
    auto* previous = StringHeader(characters_);
    length = (std::max)(length, previous->nDataLength);
    if (previous->nRefs > 1) { Fork(length); return; }
    const int capacity = previous->nAllocLength;
    if (capacity >= length) return;
    // 原10007F68..89：容量超过40000000h加100000h，否则加截零的一半；DWORD环绕后有符号max。
    const int increment = capacity > 0x40000000 ? 0x100000 : capacity / 2;
    const int grown = std::bit_cast<int>(static_cast<std::uint32_t>(capacity) +
        static_cast<std::uint32_t>(increment));
    Reallocate((std::max)(length, grown));
}

const char* NativeAnsiString::Assign(const char* source)
{
    // 原10004940..51/100049D0..E1逐字节扫描；nullptr作为length=0，不走资源ID构造分支。
    std::uint32_t size = 0;
    if (source)
    {
        const char* end = source;
        while (*end) ++end;
        size = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end) -
            reinterpret_cast<std::uintptr_t>(source));
    }
    const int length = std::bit_cast<int>(size);
    if (length == 0) return Empty();
    auto* previous = StringHeader(characters_);
    const auto oldLength = static_cast<std::uint32_t>(previous->nDataLength);
    const auto offset = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(source) -
        reinterpret_cast<std::uintptr_t>(characters_));
    if (length < 0) throw ATL::CAtlException(E_INVALIDARG);
    // 原10007FE3..FF8保留32位减法/OR的符号判断，不用有符号溢出的宿主表达式。
    const auto needsWrite = (1u - static_cast<std::uint32_t>(previous->nRefs)) |
        (static_cast<std::uint32_t>(previous->nAllocLength) - size);
    if (std::bit_cast<int>(needsWrite) < 0) PrepareWrite(length);
    char* destination = characters_;
    const auto capacity = static_cast<std::uint32_t>(StringHeader(destination)->nAllocLength);
    if (offset <= oldLength)
        (void)memmove_s(destination, capacity, destination + offset, size);
    else
        (void)memcpy_s(destination, capacity, source, size);
    // 原10008023..33复制后重读槽；先写header长度，再重读字符指针写NUL，不消费CRT返回码。
    char* result = characters_;
    auto* current = StringHeader(result);
    if (length > current->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
    current->nDataLength = length;
    characters_[length] = 0;
    return result;
}


void NativeAnsiString::PrepareBuffer(const int length)
{
    auto* previous = StringHeader(characters_);
    const auto need = (1u - static_cast<std::uint32_t>(previous->nRefs)) |
        (static_cast<std::uint32_t>(previous->nAllocLength) - static_cast<std::uint32_t>(length));
    if (std::bit_cast<int>(need) < 0) PrepareWrite(length);
}

void NativeAnsiString::AppendChar(const char character)
{
    // 原07AC0：先扩容，写字符，再回读槽校验并提交长度/NUL；异常不回滚已有正文。
    const int old = GetLength();
    const int length = std::bit_cast<int>(static_cast<std::uint32_t>(old) + 1u);
    if (length < 0) throw ATL::CAtlException(E_INVALIDARG);
    PrepareBuffer(length);
    characters_[old] = character;
    auto* current = StringHeader(characters_);
    if (length > current->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
    current->nDataLength = length;
    characters_[length] = 0;
}

void NativeAnsiString::Append(const char* source)
{
    // 原07B20先扫描输入，07BC0再捕获旧长度/源偏移并做有界扫描；NULL仍走长度0的追加。
    const auto scan = [](const char* value)
    {
        if (!value) return std::uint32_t(0);
        const char* end = value;
        while (*end) ++end;
        return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end) -
            reinterpret_cast<std::uintptr_t>(value));
    };
    int count = std::bit_cast<int>(scan(source));
    const int old = GetLength();
    const auto offset = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(source) -
        reinterpret_cast<std::uintptr_t>(characters_));
    if (count < 0) throw ATL::CAtlException(E_INVALIDARG);
    count = source ? static_cast<int>(strnlen_s(source, static_cast<std::size_t>(count))) : 0;
    if ((std::numeric_limits<int>::max)() - count < old) throw ATL::CAtlException(E_INVALIDARG);
    const int length = std::bit_cast<int>(static_cast<std::uint32_t>(old) + static_cast<std::uint32_t>(count));
    if (length < 0) throw ATL::CAtlException(E_INVALIDARG);
    PrepareBuffer(length);
    const char* actual = offset <= static_cast<std::uint32_t>(old) ? characters_ + offset : source;
    (void)memcpy_s(characters_ + old, static_cast<std::size_t>(count), actual, static_cast<std::size_t>(count));
    auto* current = StringHeader(characters_);
    if (length > current->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
    current->nDataLength = length;
    characters_[length] = 0;
}

NativeAnsiString::NativeAnsiString(const NativeAnsiString& prefix, const char* suffix)
{
    // 原05180：先Clone源manager；其缺失/Clone空返回才取默认manager，不能提前复制源正文。
    using Clone = ATL::IAtlStringMgr* (__thiscall*)(ATL::IAtlStringMgr*);
    using GetNil = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*);
    auto* sourceManager = StringHeader(prefix.GetString())->pStringMgr;
    auto* manager = sourceManager ? ManagerSlot<Clone>(sourceManager, 16)(sourceManager) : nullptr;
    if (!manager)
    {
        auto* fallback = ATL::CAtlStringMgr::GetInstance();
        manager = ManagerSlot<Clone>(fallback, 16)(fallback);
    }
    if (!manager) throw ATL::CAtlException(E_FAIL);
    characters_ = Characters(ManagerSlot<GetNil>(manager, 12)(manager));
    struct ConstructionUnwind
    {
        NativeAnsiString* value;
        ~ConstructionUnwind() noexcept(false) { if (value) value->Retire(); }
    } unwind{this}; // 原051F5发布后才启用返回对象bit。
    const char* source = prefix.GetString();
    const int first = StringHeader(source)->nDataLength;
    const int second = static_cast<int>(std::strlen(suffix));
    const int length = std::bit_cast<int>(static_cast<std::uint32_t>(first) + static_cast<std::uint32_t>(second));
    if (length < 0) throw ATL::CAtlException(E_INVALIDARG);
    PrepareBuffer(length);
    char* destination = characters_;
    (void)memcpy_s(destination, static_cast<std::size_t>(first), source, static_cast<std::size_t>(first));
    (void)memcpy_s(destination + first, static_cast<std::size_t>(second), suffix, static_cast<std::size_t>(second));
    auto* current = StringHeader(characters_);
    if (length > current->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
    current->nDataLength = length;
    characters_[length] = 0;
    unwind.value = nullptr;
}

}
