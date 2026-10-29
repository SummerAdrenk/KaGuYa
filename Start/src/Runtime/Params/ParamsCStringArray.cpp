#include <Runtime/ParamsCStringArray.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t SnapshotCount(std::size_t count)
{
    // 快照数量不能截断为负数后让 412870 静默返回空表；此检查不用于线上 count。
    if (count > static_cast<std::size_t>((std::numeric_limits<std::int32_t>::max)()))
        throw std::length_error("Params CString snapshot exceeds signed array count");
    return static_cast<std::int32_t>(count);
}

void SetSnapshotString(RuntimeCString& destination, std::string_view bytes)
{
    if (bytes.size() > static_cast<std::size_t>((std::numeric_limits<int>::max)()))
        throw std::length_error("Params CString snapshot exceeds CString length");
    // 重新写入完整字节，不 Clone 源 header；保留嵌入 NUL，不复制 utf16 或源 manager。
    SetRuntimeCStringAt437680(destination, bytes.data(), static_cast<int>(bytes.size()));
}
}

ParamsCStringArray::ParamsCStringArray(std::initializer_list<ParamsText> values)
{
    AssignSnapshot(std::span<const ParamsText>(values.begin(), values.size()));
}

ParamsCStringArray& ParamsCStringArray::operator=(std::initializer_list<ParamsText> values)
{
    AssignSnapshot(std::span<const ParamsText>(values.begin(), values.size()));
    return *this;
}

void ParamsCStringArray::AssignSnapshot(std::span<const ParamsText> values)
{
    const auto count = SnapshotCount(values.size());
    ParamsCStringArray snapshot;
    snapshot.storage.RebuildAt412870(count);
    for (std::size_t index = 0; index < values.size(); ++index)
        SetSnapshotString(snapshot[index], values[index].cp932);
    // 快照独立完成后替换；空快照也替换旧表，不冒充 reader 的 count==0 保留分支。
    // 此局部拥有者仅用于现代快照，不能移入生产 reader 为部分发布增加回滚。
    *this = std::move(snapshot);
}

ParamsCStringArray::ParamsCStringArray(const ParamsCStringArray& other)
{
    const auto count = other.size();
    ParamsCStringArray snapshot;
    snapshot.storage.RebuildAt412870(SnapshotCount(count));
    for (std::size_t index = 0; index < count; ++index)
        SetSnapshotString(snapshot[index], RuntimeStringView(other[index]));
    // 不能默认复制 storage：底层复制是 Ref 增持，profile 需要独立数组和字符串正文。
    storage = std::move(snapshot.storage);
}

ParamsCStringArray& ParamsCStringArray::operator=(const ParamsCStringArray& other)
{
    if (this != &other)
    {
        ParamsCStringArray snapshot(other);
        *this = std::move(snapshot);
    }
    return *this;
}

ParamsCStringArray::ParamsCStringArray(ParamsCStringArray&& other) noexcept
    : storage(std::move(other.storage)) {}

ParamsCStringArray& ParamsCStringArray::operator=(ParamsCStringArray&& other)
{
    if (this != &other) storage = std::move(other.storage);
    return *this;
}

std::size_t ParamsCStringArray::size() const noexcept
{
    // 数量来自当前 control.count，不扣除 bias，不读取 cookie，也不猜测 base 状态。
    return storage.Size();
}

RuntimeCString& ParamsCStringArray::operator[](std::size_t index)
{
    return storage.At(static_cast<std::uint32_t>(index));
}

const RuntimeCString& ParamsCStringArray::operator[](std::size_t index) const
{
    return storage.At(static_cast<std::uint32_t>(index));
}

RuntimeCString& ParamsCStringArray::at(std::size_t index)
{
    return const_cast<RuntimeCString&>(std::as_const(*this).at(index));
}

const RuntimeCString& ParamsCStringArray::at(std::size_t index) const
{
    if (index >= size()) throw std::out_of_range("Params CString array index is out of range");
    return (*this)[index];
}

bool ParamsCStringArray::operator==(const ParamsCStringArray& other) const
{
    const auto count = size();
    if (count != other.size()) return false;
    for (std::size_t index = 0; index < count; ++index)
        if (RuntimeStringView((*this)[index]) != RuntimeStringView(other[index])) return false;
    return true;
}
}
