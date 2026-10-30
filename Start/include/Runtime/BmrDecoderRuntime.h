#pragma once

#include <Runtime/ArchiveCodecInputView.h>
#include <Runtime/YtlReferenceArrayRuntime.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace KaGuYa::Start::Runtime
{
class IArchiveCodecManagerSource;
class IArchiveHuffmanErrorSource;
// 只验证本次即将读取的原地址，失效时抛出；不得重取来源、增持或复制正文。
// guard自身必须活到调用结束；它不证明decoder及destination的存活。
// 不传guard时，调用方必须保证输入在全部回调期间保持可读，span本身不是存活证明。
class IBmrInputReadGuard
{
public:
    virtual ~IBmrInputReadGuard() = default;
    virtual void ValidateRead(std::span<const std::uint8_t> bytes) = 0;
};

// 验证固定目标地址的本次读写范围，不重绑manager/key、不增持、不复制目标。
// 不传guard时，调用方保证跨算法分配回调仍可访问；本接口不证明decoder自身存活。
class IBmrDestinationAccessGuard
{
public:
    virtual ~IBmrDestinationAccessGuard() = default;
    virtual void ValidateAccess(std::span<const std::uint8_t> bytes) = 0;
};

struct BmrHeaderRuntime final
{
    std::array<std::uint8_t, 3> magicAt00;
    std::uint8_t stepAt03;
    std::uint32_t finalSizeAt04;
    std::uint32_t bwtKeyAt08;
    std::uint32_t intermediateSizeAt0C;
};
static_assert(sizeof(BmrHeaderRuntime) == 0x10);
static_assert(offsetof(BmrHeaderRuntime, finalSizeAt04) == 4);
static_assert(offsetof(BmrHeaderRuntime, bwtKeyAt08) == 8);
static_assert(offsetof(BmrHeaderRuntime, intermediateSizeAt0C) == 12);

// 445110 的14h CBMH正文：只拥有Header Ref，不拥有或增持压缩输入。
// 生产重载借用同一共享codec来源；不把Huffman指针或临时算法状态存进正文。
class BmrDecoderRuntime final
{
public:
    BmrDecoderRuntime() noexcept;
    ~BmrDecoderRuntime() noexcept;
    BmrDecoderRuntime(const BmrDecoderRuntime&) = delete;
    BmrDecoderRuntime& operator=(const BmrDecoderRuntime&) = delete;

    // hit返回原source+16；miss不写outSize、不清旧Header。span仅是显式安全范围。
    [[nodiscard]] const std::uint8_t* IdentifyAt445A00(
        std::span<const std::uint8_t> source, std::uint32_t& outSize,
        IBmrInputReadGuard* inputGuard = nullptr);
    [[nodiscard]] const std::uint8_t* IdentifyRawAt445A00(
        const std::uint8_t* source, std::uint32_t& outSize, IBmrInputReadGuard* inputGuard = nullptr);
    [[nodiscard]] const std::uint8_t* IdentifyAt445A00(
        ArchiveCodecInputView source, std::uint32_t& outSize, IBmrInputReadGuard* inputGuard = nullptr);
    // 显式工具/历史夹具入口：局部Huffman和vector临时，不供生产调用。
    void DecodeAt43DCF0(std::span<const std::uint8_t> payload,
        std::span<std::uint8_t> destination, IBmrInputReadGuard* inputGuard = nullptr,
        IBmrDestinationAccessGuard* destinationGuard = nullptr);
    // payload从Huffman长度DWORD开始；先缓存Header.N，再取共享owner，最后读取payload。
    void DecodeAt43DCF0(std::span<const std::uint8_t> payload,
        std::span<std::uint8_t> destination, IArchiveCodecManagerSource& codecs,
        IArchiveHuffmanErrorSource& errors, IBmrInputReadGuard* inputGuard = nullptr,
        IBmrDestinationAccessGuard* destinationGuard = nullptr);
    // Raw一路传到共享Huffman核心，不经现代工具解码器或伪造输入span。
    void DecodeRawAt43DCF0(const std::uint8_t* payload,
        std::span<std::uint8_t> destination, IArchiveCodecManagerSource& codecs,
        IArchiveHuffmanErrorSource& errors, IBmrInputReadGuard* inputGuard = nullptr,
        IBmrDestinationAccessGuard* destinationGuard = nullptr);
    void DecodeAt43DCF0(ArchiveCodecInputView payload,
        std::span<std::uint8_t> destination, IArchiveCodecManagerSource& codecs,
        IArchiveHuffmanErrorSource& errors, IBmrInputReadGuard* inputGuard = nullptr,
        IBmrDestinationAccessGuard* destinationGuard = nullptr);
    // 正常43E292释放可抛；所属析构及识别局部临时清理另有终止边界。
    void ResetHeader();
    [[nodiscard]] YtlReferenceSlotRuntime& HeaderReferenceAt04() noexcept { return headerAt04_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& HeaderReferenceAt04() const noexcept { return headerAt04_; }

private:
    std::array<std::uint8_t, 3> magicAt00_;
    // 原生03h保留未写字节，不删除字段或补初始化。
    [[maybe_unused]] std::uint8_t unwrittenAt03_;
    YtlReferenceSlotRuntime headerAt04_;
    // 原生10h..13h同样保持未写，维持14h正文布局。
    [[maybe_unused]] std::array<std::uint8_t, 4> unwrittenAt10_;
};
static_assert(sizeof(BmrDecoderRuntime) == 0x14);

// 445110直接操作30h manager的+0C槽；原空解析保留为空返回，不另造独立owner。
[[nodiscard]] BmrDecoderRuntime* GetBmrAt445110(YtlReferenceSlotRuntime& slot);

// 明确工具/夹具owner；生产使用30h manager，不能把此独立Ref当作共享压缩根。
class BmrDecoderRuntimeOwner final
{
public:
    BmrDecoderRuntimeOwner() noexcept = default;
    ~BmrDecoderRuntimeOwner() noexcept;
    BmrDecoderRuntimeOwner(const BmrDecoderRuntimeOwner&) = delete;
    BmrDecoderRuntimeOwner& operator=(const BmrDecoderRuntimeOwner&) = delete;

    [[nodiscard]] BmrDecoderRuntime& GetOrCreateAt445110();
    [[nodiscard]] YtlReferenceSlotRuntime& DecoderReference() noexcept { return decoder_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& DecoderReference() const noexcept { return decoder_; }

private:
    YtlReferenceSlotRuntime decoder_{};
};
static_assert(sizeof(BmrDecoderRuntimeOwner) == 12);
}
