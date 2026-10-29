#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace KaGuYa::Start::Runtime::CategorizedSoundEnvironmentWire
{
// 文件适配层的值结构，不是旧原生对象，也不替换现代设置快照。
enum class Kind : std::uint32_t { Basic = 0, Bgm = 1, Categorized = 2 };

struct Value final
{
    std::uint8_t mute{};
    std::uint32_t volume{};
    bool operator==(const Value&) const = default;
};

struct Child final
{
    Value value;
    // 每个子项均保留原始 byte，包括现代模型没有显示字段的普通效果音。
    std::uint8_t isDisp{};
    bool operator==(const Child&) const = default;
};

struct Entry final
{
    Kind kind{};
    Value value;
    std::vector<Child> children;
    bool operator==(const Entry&) const = default;
};

inline constexpr std::size_t NoIndex = static_cast<std::size_t>(-1);
// 原 45887F/458D69 使用有符号循环；高位 count 在此明确拒绝，不接受为空表。
inline constexpr std::uint32_t MaximumCount = 0x7FFFFFFFu;

enum class Error
{
    None,
    InvalidCursor,
    InvalidKind,
    UnexpectedChildren,
    CountOutOfRange,
    SizeOverflow,
    TruncatedInput,
    OutputTooSmall
};

enum class Field { None, EntryCount, Kind, Mute, Volume, ChildCount, IsDisp };

struct Result final
{
    Error error{};
    std::size_t cursor{}; // 相对 span 起点，已消费/写出前缀的末尾。
    std::size_t offset{}; // 失败字段的起点；成功时等于 cursor。
    std::size_t entry = NoIndex;
    std::size_t child = NoIndex; // NoIndex 表示顶层项自身的字段。
    Field field{};
    explicit operator bool() const noexcept { return error == Error::None; }
};

// 无分配地校验注册形状并计算长度；失败时不改 size。
// 此操作返回的 cursor/offset 均为零，成功长度通过 size 取得。
[[nodiscard]] Result Measure(std::span<const Entry> entries, std::size_t& size) noexcept;

// 调用方按原注册顺序提供已初始化的顶层项和子项。
// 不扩缩容、不按名称查找、不转码、不归一化 bool、不截断数值范围。
// 同位置且同 kind 才按 wire 顺序提交；超额/不匹配项及超额子项消费后丢弃；
// 文件中缺少的项/子项保留当前值。每个完整标量独立提交，后续失败不回滚前缀。
// 截断时失败标量及游标不变：这是有界安全策略，不复刻原版越界行为。
// 非法输入 kind/count 读完其 DWORD 后拒绝，不再消费正文。
// 注册形状非法则在读取或修改状态前拒绝。后续 env 尾段不消费、不拒绝。
// 调用期间 span 必须指向有效且不重叠的存储。
[[nodiscard]] Result Read(std::span<const std::byte> input,
    std::size_t cursor, std::span<Entry> registered) noexcept;

// 写前校验注册形状；容量不足时保留完整标量前缀，失败标量不写任何 byte。
// 失败缓冲区不得落盘。写出当前注册形状，不恢复读入时已丢弃的超额项/子项。
[[nodiscard]] Result Write(std::span<std::byte> output,
    std::size_t cursor, std::span<const Entry> registered) noexcept;
}
