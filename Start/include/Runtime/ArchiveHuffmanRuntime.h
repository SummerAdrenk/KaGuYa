#pragma once

#include <Runtime/ArchiveCodecInputView.h>

#include <Windows.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace KaGuYa::Start::Runtime
{
struct ArchiveCodecManagerRuntime;
class IBmrInputReadGuard;
class IBmrDestinationAccessGuard;
class ILoadSpriteErrorEnvironmentSource;

class IArchiveHuffmanErrorSource
{
public:
    virtual ~IArchiveHuffmanErrorSource() = default;
    // 原4180C0取环境后执行420C50；报告可抛，成功返回后解码叶才抛原int。
    virtual void ReportAt420C50(const char* textCp932) = 0;
};

// 借用真实环境来源，每次错误才Capture；不缓存标题/窗口，不提供空报告兜底。
class ArchiveHuffmanErrorSource final : public IArchiveHuffmanErrorSource
{
public:
    explicit ArchiveHuffmanErrorSource(ILoadSpriteErrorEnvironmentSource& environment,
        decltype(&::MessageBoxA) messageBox = &::MessageBoxA) noexcept;
    void ReportAt420C50(const char* textCp932) override;
private:
    ILoadSpriteErrorEnvironmentSource& environment_;
    decltype(&::MessageBoxA) messageBox_;
};

// 仅表示已证三指针和释放字节范围；scratch元素类型、编码侧写入者尚未确定。
struct ArchiveHuffmanScratchRuntime final
{
    void* begin;
    void* end;
    void* capacity;
};

struct ArchiveHuffmanRuntime final
{
    // 正文由专属control执行43E910释放；不得以普通delete代替scratch退役。
    struct RawAt43DE72 final {};
    explicit ArchiveHuffmanRuntime(RawAt43DE72) noexcept;
    ArchiveHuffmanRuntime(const ArchiveHuffmanRuntime&) = delete;
    ArchiveHuffmanRuntime& operator=(const ArchiveHuffmanRuntime&) = delete;

    // caller先在原点Capture manager/GetHuffman；本方法此时才读取长度DWORD。
    // 旧入口以output.size()委托输出N；即使N=0仍建树，不复制或增持输入。
    // 逻辑耗尽/节点超限按原报告后抛int；物理extent不足另抛out_of_range，不混为原错误。
    // 43DE24是原inline owner片段地址；本方法抽取其后43DF08/44851C的共同解码。
    void DecodeAt43DE24(std::span<const std::uint8_t> payloadInclLengthDword,
        std::span<std::uint8_t> output, IBmrInputReadGuard* inputGuard,
        IBmrDestinationAccessGuard* destinationGuard, IArchiveHuffmanErrorSource& errors);

    // 原缓存N与物理范围独立；先解码当前字符，到43DFD3写入点才拒绝目标越界。
    void DecodeAt43DE24(std::span<const std::uint8_t> payloadInclLengthDword,
        std::span<std::uint8_t> outputExtent, std::uint32_t outputCount,
        IBmrInputReadGuard* inputGuard, IBmrDestinationAccessGuard* destinationGuard,
        IArchiveHuffmanErrorSource& errors);
    // 正式raw入口仍在原点读长度DWORD，保留逻辑耗尽/节点错误；不猜物理extent。
    void DecodeRawAt43DE24(const std::uint8_t* payloadInclLengthDword,
        std::span<std::uint8_t> outputExtent, std::uint32_t outputCount,
        IBmrInputReadGuard* inputGuard, IBmrDestinationAccessGuard* destinationGuard,
        IArchiveHuffmanErrorSource& errors);
    void DecodeAt43DE24(ArchiveCodecInputView input,
        std::span<std::uint8_t> output, IBmrInputReadGuard* inputGuard,
        IBmrDestinationAccessGuard* destinationGuard, IArchiveHuffmanErrorSource& errors);
    void DecodeAt43DE24(ArchiveCodecInputView input,
        std::span<std::uint8_t> outputExtent, std::uint32_t outputCount,
        IBmrInputReadGuard* inputGuard, IBmrDestinationAccessGuard* destinationGuard,
        IArchiveHuffmanErrorSource& errors);

    std::int32_t remainingBitsAt00;
    std::uint32_t bitsPerByteAt04;
    std::int32_t currentByteAt08;
    std::uint32_t readBytesAt0C;
    std::uint32_t inputSizeAt10;
    // 匿名union不选择初始成员，避免/sdl给原未写的指针成员补nullptr。
    union { const std::uint8_t* inputAt14; };
    ArchiveHuffmanScratchRuntime scratchAt18;
    std::array<std::int32_t, 0x1FF> leftAt24;
    std::array<std::int32_t, 0x1FF> rightAt820;
    std::int32_t nextNodeAt101C;
};
static_assert(sizeof(ArchiveHuffmanScratchRuntime) == 12);
static_assert(sizeof(ArchiveHuffmanRuntime) == 0x1020);
static_assert(offsetof(ArchiveHuffmanRuntime, remainingBitsAt00) == 0x00);
static_assert(offsetof(ArchiveHuffmanRuntime, bitsPerByteAt04) == 0x04);
static_assert(offsetof(ArchiveHuffmanRuntime, currentByteAt08) == 0x08);
static_assert(offsetof(ArchiveHuffmanRuntime, readBytesAt0C) == 0x0C);
static_assert(offsetof(ArchiveHuffmanRuntime, inputSizeAt10) == 0x10);
static_assert(offsetof(ArchiveHuffmanRuntime, inputAt14) == 0x14);
static_assert(offsetof(ArchiveHuffmanRuntime, scratchAt18) == 0x18);
static_assert(offsetof(ArchiveHuffmanRuntime, leftAt24) == 0x24);
static_assert(offsetof(ArchiveHuffmanRuntime, rightAt820) == 0x820);
static_assert(offsetof(ArchiveHuffmanRuntime, nextNodeAt101C) == 0x101C);

// 原inline工厂：只访问manager.huffmanAt00；不增加正文失败回收或隐藏持有。
[[nodiscard]] ArchiveHuffmanRuntime* GetHuffmanAt43DE24(ArchiveCodecManagerRuntime& manager);
}
