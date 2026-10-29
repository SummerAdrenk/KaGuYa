#include <Runtime/ParamsSceneNameTable.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Storage = EnvironmentVectorStorage<RuntimeCString>;

RuntimeCString* Advance(RuntimeCString* pointer, std::uint32_t count) noexcept
{
    return reinterpret_cast<RuntimeCString*>(reinterpret_cast<std::uintptr_t>(pointer) + count * 4u);
}

// 仅供 fixture/profile 深快照，不代表 41D74C reader 的跨循环临时串及 EH 资格。
class SnapshotCString final
{
public:
    SnapshotCString() { InitializeEmptyRuntimeCString(value); }
    ~SnapshotCString() noexcept { DestroyRuntimeCStringAt40CD40(&value); }
    SnapshotCString(const SnapshotCString&) = delete;
    SnapshotCString& operator=(const SnapshotCString&) = delete;
    union { RuntimeCString value; };
};

void AppendSnapshotString(ParamsSceneNameTable& destination, const char* chars, std::size_t length)
{
    if (length > static_cast<std::size_t>((std::numeric_limits<int>::max)()))
        throw std::length_error("Params scene name snapshot exceeds CString length");
    SnapshotCString copy;
    // 新临时串重新写入字节，不直接 Clone 源 header；深快照不共享源的非空正文。
    // 显式长度保留嵌入 NUL，utf16 不成为第二份生产存储。
    SetRuntimeCStringAt437680(copy.value, chars, static_cast<int>(length));
    destination.Append(copy.value);
}
}

ParamsSceneNameTable::~ParamsSceneNameTable() noexcept
{
    // GS 40F3B9 -> 43A700；40DD20 的终止边界与该调用点匹配，不再跑 ATL 析构。
    DestroyRuntimeCStringVectorAt40DD20(storage);
}

ParamsSceneNameTable::ParamsSceneNameTable(std::initializer_list<ParamsText> values)
{
    ParamsSceneNameTable snapshot;
    for (const auto& value : values) snapshot.push_back(value);
    storage = std::exchange(snapshot.storage, Storage{});
}

ParamsSceneNameTable::ParamsSceneNameTable(const ParamsSceneNameTable& other)
{
    ParamsSceneNameTable snapshot;
    for (const auto& source : other)
        AppendSnapshotString(snapshot, source.GetString(), source.GetLength());
    storage = std::exchange(snapshot.storage, Storage{});
}

ParamsSceneNameTable& ParamsSceneNameTable::operator=(const ParamsSceneNameTable& other)
{
    if (this != &other)
    {
        ParamsSceneNameTable snapshot(other);
        *this = std::move(snapshot);
    }
    return *this;
}

ParamsSceneNameTable::ParamsSceneNameTable(ParamsSceneNameTable&& other) noexcept
    : storage(std::exchange(other.storage, Storage{})) {}

ParamsSceneNameTable& ParamsSceneNameTable::operator=(ParamsSceneNameTable&& other) noexcept
{
    if (this != &other)
    {
        DestroyRuntimeCStringVectorAt40DD20(storage);
        storage = std::exchange(other.storage, Storage{});
    }
    return *this;
}

ParamsSceneNameTable& ParamsSceneNameTable::operator=(std::initializer_list<ParamsText> values)
{
    ParamsSceneNameTable snapshot(values);
    *this = std::move(snapshot);
    return *this;
}

std::size_t ParamsSceneNameTable::size() const noexcept
{
    // 原四字节元素距离：DWORD 相减后 SAR 2，不做 C++ 跨分配块指针相减。
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(storage.end) -
        reinterpret_cast<std::uintptr_t>(storage.begin));
    return (bytes >> 2u) | ((bytes & 0x80000000u) ? 0xC0000000u : 0u);
}

RuntimeCString& ParamsSceneNameTable::operator[](std::size_t index) noexcept
{
    return *Advance(storage.begin, static_cast<std::uint32_t>(index));
}

const RuntimeCString& ParamsSceneNameTable::operator[](std::size_t index) const noexcept
{
    return *Advance(storage.begin, static_cast<std::uint32_t>(index));
}

RuntimeCString& ParamsSceneNameTable::at(std::size_t index)
{
    return const_cast<RuntimeCString&>(std::as_const(*this).at(index));
}

const RuntimeCString& ParamsSceneNameTable::at(std::size_t index) const
{
    // 保留现代调用方的显式越界保护，不改变 [] 的原始索引语义。
    if (index >= size()) throw std::out_of_range("Params scene name index is out of range");
    return (*this)[index];
}

void ParamsSceneNameTable::clear() noexcept
{
    // 41D752/41D75E 固定本轮旧范围；43A440 正序释放，回调不扩展或改换该范围。
    auto* current = storage.begin;
    auto* const end = storage.end;
    for (; current != end; current = Advance(current, 1u))
        DestroyRuntimeCStringAt40CD40(current);
    // 41D765/41D76A 成功后才重读 begin；state22 保证释放失败终止而不发布 end。
    storage.end = storage.begin;
}

void ParamsSceneNameTable::Append(const RuntimeCString& source, IEnvironmentRootMemory& memory)
{
    // 41D7F8..41D827：快路径缓存目标槽、Clone 后更新当前 end；慢路径即 4085C0。
    AppendRuntimeCStringAt41D5E9(storage, source, memory);
}

void ParamsSceneNameTable::push_back(const ParamsText& fixture)
{
    AppendSnapshotString(*this, fixture.cp932.data(), fixture.cp932.size());
}

bool ParamsSceneNameTable::operator==(const ParamsSceneNameTable& other) const
{
    const auto count = size();
    if (count != other.size()) return false;
    for (std::size_t index = 0; index < count; ++index)
        if (RuntimeStringView((*this)[index]) != RuntimeStringView(other[index])) return false;
    return true;
}
}
