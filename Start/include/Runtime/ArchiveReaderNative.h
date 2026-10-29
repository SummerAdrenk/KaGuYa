#pragma once

#include <Runtime/ByteArrayReferenceRuntime.h>
#include <Runtime/RuntimeCString.h>

#include <cstddef>
#include <cstdint>
#include <span>

namespace KaGuYa::Start::Runtime
{
class ArchiveReaderReferenceControl;
class ArchiveReaderReference;

// 5ADE08：只声明原五槽，不添加 C++ 虚析构槽。
class ArchiveStreamReaderRuntime
{
public:
    virtual void* DeleteAtVslot00(std::uint32_t flags);
    virtual void ClearAt4327B0();
    [[nodiscard]] virtual bool ReadyAt419360() const;
    [[nodiscard]] virtual ByteArrayReferenceRuntime* BytesAt4172C0();
    [[nodiscard]] virtual const char* NameAt4179F0() const;

    // 两段手工生命周期：GetNil 失败不得自动展开已经清零的 byte Ref。
    union { ByteArrayReferenceRuntime bytesAt04; };
    union { RuntimeCString nameAt10; };

    [[nodiscard]] ByteArrayReferenceRuntime& RawBytesAt04() noexcept { return bytesAt04; }
    [[nodiscard]] const ByteArrayReferenceRuntime& RawBytesAt04() const noexcept { return bytesAt04; }
    [[nodiscard]] RuntimeCString& RawNameAt10() noexcept { return nameAt10; }
    [[nodiscard]] const RuntimeCString& RawNameAt10() const noexcept { return nameAt10; }

    ArchiveStreamReaderRuntime(const ArchiveStreamReaderRuntime&) = delete;
    ArchiveStreamReaderRuntime& operator=(const ArchiveStreamReaderRuntime&) = delete;

protected:
    explicit ArchiveStreamReaderRuntime(IRuntimeStringManagerSource& strings);
    ~ArchiveStreamReaderRuntime() noexcept;
};
static_assert(sizeof(ArchiveStreamReaderRuntime) == 0x14);

// 5ADE20：正文没有派生字段。flags0 执行完整析构并留下 IStreamReader 虚表。
class ArchiveFileReaderRuntime final : public ArchiveStreamReaderRuntime
{
public:
    explicit ArchiveFileReaderRuntime(
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    // 511DCF内联构造完成后才从caller调用Clear；失败无C++基类展开资格。
    struct InlineBeforeClearTag final {};
    ArchiveFileReaderRuntime(InlineBeforeClearTag, IRuntimeStringManagerSource& strings);
    void* DeleteAtVslot00(std::uint32_t flags) override;
    ~ArchiveFileReaderRuntime() noexcept = default;
};
static_assert(sizeof(ArchiveFileReaderRuntime) == 0x14);

inline constexpr std::size_t ArchiveReaderNativeControlSize = 0x18;

// 仅在调用者已分配的 14h 外块原地构造；不分配、不发布、不回收外块。
// state2 的外块删除及构造完成后解除保护由 Factory 掌握。
[[nodiscard]] ArchiveFileReaderRuntime* ConstructArchiveFileReaderAt483E99(
    void* storage, IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

// 483F82：仅写 vptr/payload；发布后 refs/owning/stride/extent 的写入仍由 Factory 完成。
// 18h 控制块不保存 allocator；slot04 首次删除为 18h，flags0 退役后为 14h。
[[nodiscard]] ArchiveReaderReferenceControl* ConstructArchiveReaderControl(
    void* storage, ArchiveStreamReaderRuntime* body) noexcept;

// 现代连续视图适配：仅在实际消费点调用，不保活、不跨回调缓存。
// 非空输入必须是 bias=0/stride=1；越出该现代适配域抛 E_INVALIDARG，不改原 getter。
// 缺正文或无control却有base同样拒绝，不能以空span掩盖未知长度/缺失对象。
[[nodiscard]] std::span<const std::byte> ArchiveReaderPayloadView(const ArchiveReaderReference& reader);
}
