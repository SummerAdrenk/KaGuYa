#include <Runtime/CategorizedSoundEnvironmentWire.h>

#include <limits>

namespace KaGuYa::Start::Runtime::CategorizedSoundEnvironmentWire
{
namespace
{
bool ValidKind(Kind kind) noexcept
{
    return kind == Kind::Basic || kind == Kind::Bgm || kind == Kind::Categorized;
}

Result Failure(Error error, std::size_t cursor, std::size_t offset,
    std::size_t entry, std::size_t child, Field field) noexcept
{
    return {error, cursor, offset, entry, child, field};
}

Result Success(std::size_t cursor) noexcept
{
    return {Error::None, cursor, cursor};
}

Result Validate(std::span<const Entry> entries, std::size_t cursor) noexcept
{
    if (entries.size() > MaximumCount)
        return Failure(Error::CountOutOfRange, cursor, cursor, NoIndex, NoIndex, Field::EntryCount);
    for (std::size_t i = 0; i < entries.size(); ++i)
    {
        const auto& entry = entries[i];
        if (!ValidKind(entry.kind))
            return Failure(Error::InvalidKind, cursor, cursor, i, NoIndex, Field::Kind);
        if (entry.kind != Kind::Categorized && !entry.children.empty())
            return Failure(Error::UnexpectedChildren, cursor, cursor, i, NoIndex, Field::ChildCount);
        if (entry.children.size() > MaximumCount)
            return Failure(Error::CountOutOfRange, cursor, cursor, i, NoIndex, Field::ChildCount);
    }
    return Success(cursor);
}

// 01:458B60/458E00 确定字段顺序；容量不足时按标量拒绝，
// 不复刻 455A20/455AF0/478120 逐 byte 读取导致的半标量提交或越界。
class Reader final
{
public:
    std::span<const std::byte> bytes;
    std::size_t cursor;
    std::size_t entry = NoIndex;
    std::size_t child = NoIndex;
    Result failure;

    template<class T> bool Scalar(T& target, Field field) noexcept
    {
        static_assert(sizeof(T) == 1 || sizeof(T) == 4);
        if (bytes.size() - cursor < sizeof(T))
        {
            failure = Failure(Error::TruncatedInput, cursor, cursor, entry, child, field);
            return false;
        }
        std::uint32_t value = 0;
        for (std::size_t i = 0; i < sizeof(T); ++i)
            value |= std::to_integer<std::uint32_t>(bytes[cursor + i]) << (8 * i);
        target = static_cast<T>(value);
        cursor += sizeof(T);
        return true;
    }

    bool Count(std::uint32_t& count, Field field) noexcept
    {
        const auto start = cursor;
        if (!Scalar(count, field)) return false;
        if (count <= MaximumCount) return true;
        // 45887F/458D69 为有符号循环。负数 count 不属于正常域，
        // 此处明确拒绝，不沿用原循环把其接受为空表的行为。
        failure = Failure(Error::CountOutOfRange, cursor, start, entry, child, field);
        return false;
    }

    bool Scalars(Value& value) noexcept
    {
        return Scalar(value.mute, Field::Mute) && Scalar(value.volume, Field::Volume);
    }
};

class Writer final
{
public:
    std::span<std::byte> bytes;
    std::size_t cursor;
    std::size_t entry = NoIndex;
    std::size_t child = NoIndex;
    Result failure;

    template<class T> bool Scalar(T value, Field field) noexcept
    {
        static_assert(sizeof(T) == 1 || sizeof(T) == 4);
        if (bytes.size() - cursor < sizeof(T))
        {
            failure = Failure(Error::OutputTooSmall, cursor, cursor, entry, child, field);
            return false;
        }
        for (std::size_t i = 0; i < sizeof(T); ++i)
            bytes[cursor + i] = static_cast<std::byte>((static_cast<std::uint32_t>(value) >> (8 * i)) & 0xFFu);
        cursor += sizeof(T);
        return true;
    }

    bool Scalars(const Value& value) noexcept
    {
        return Scalar(value.mute, Field::Mute) && Scalar(value.volume, Field::Volume);
    }
};
}

Result Measure(std::span<const Entry> entries, std::size_t& size) noexcept
{
    const auto valid = Validate(entries, 0);
    if (!valid) return valid;
    std::size_t total = 4;
    constexpr auto maximum = std::numeric_limits<std::size_t>::max();
    for (std::size_t i = 0; i < entries.size(); ++i)
    {
        const auto& entry = entries[i];
        const std::size_t base = entry.kind == Kind::Categorized ? 13 : 9;
        if (maximum - total < base || entry.children.size() > (maximum - total - base) / 6)
            return Failure(Error::SizeOverflow, 0, 0, i, NoIndex, Field::ChildCount);
        total += base + entry.children.size() * 6;
    }
    size = total;
    return Success(0);
}

Result Read(std::span<const std::byte> input, std::size_t cursor,
    std::span<Entry> registered) noexcept
{
    if (cursor > input.size())
        return Failure(Error::InvalidCursor, cursor, cursor, NoIndex, NoIndex, Field::None);
    const auto valid = Validate(registered, cursor);
    if (!valid) return valid;
    Reader reader{input, cursor, NoIndex, NoIndex, {}};
    std::uint32_t count = 0;
    if (!reader.Count(count, Field::EntryCount)) return reader.failure;
    for (std::uint32_t i = 0; i < count; ++i)
    {
        reader.entry = i;
        reader.child = NoIndex;
        std::uint32_t rawKind = 0;
        const auto start = reader.cursor;
        if (!reader.Scalar(rawKind, Field::Kind)) return reader.failure;
        const auto kind = static_cast<Kind>(rawKind);
        if (!ValidKind(kind))
            return Failure(Error::InvalidKind, reader.cursor, start, i, NoIndex, Field::Kind);

        // 45898A..4589AB 只应用同位置且同 kind 的项；
        // 4588BC/4589B2 的超额或不匹配项只消费正文，不修改已有注册。
        Entry* target = i < registered.size() && registered[i].kind == kind ? &registered[i] : nullptr;
        Value ignored{};
        if (!reader.Scalars(target ? target->value : ignored)) return reader.failure;
        if (kind != Kind::Categorized) continue;
        std::uint32_t children = 0;
        if (!reader.Count(children, Field::ChildCount)) return reader.failure;
        for (std::uint32_t j = 0; j < children; ++j)
        {
            reader.child = j;
            Child discarded{};
            auto& child = target && j < target->children.size() ? target->children[j] : discarded;
            if (!reader.Scalars(child.value) || !reader.Scalar(child.isDisp, Field::IsDisp))
                return reader.failure;
        }
    }
    return Success(reader.cursor);
}

Result Write(std::span<std::byte> output, std::size_t cursor,
    std::span<const Entry> registered) noexcept
{
    if (cursor > output.size())
        return Failure(Error::InvalidCursor, cursor, cursor, NoIndex, NoIndex, Field::None);
    const auto valid = Validate(registered, cursor);
    if (!valid) return valid;
    Writer writer{output, cursor, NoIndex, NoIndex, {}};
    if (!writer.Scalar(static_cast<std::uint32_t>(registered.size()), Field::EntryCount))
        return writer.failure;
    for (std::size_t i = 0; i < registered.size(); ++i)
    {
        writer.entry = i;
        writer.child = NoIndex;
        const auto& entry = registered[i];
        if (!writer.Scalar(static_cast<std::uint32_t>(entry.kind), Field::Kind) || !writer.Scalars(entry.value))
            return writer.failure;
        if (entry.kind != Kind::Categorized) continue;
        if (!writer.Scalar(static_cast<std::uint32_t>(entry.children.size()), Field::ChildCount))
            return writer.failure;
        for (std::size_t j = 0; j < entry.children.size(); ++j)
        {
            writer.child = j;
            const auto& child = entry.children[j];
            if (!writer.Scalars(child.value) || !writer.Scalar(child.isDisp, Field::IsDisp))
                return writer.failure;
        }
    }
    return Success(writer.cursor);
}
}
