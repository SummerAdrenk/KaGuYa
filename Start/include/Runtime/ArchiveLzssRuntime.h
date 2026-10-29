#pragma once

#include <Runtime/ArchiveCodecInputView.h>

#include <cstddef>
#include <cstdint>
#include <exception>
#include <span>

namespace KaGuYa::Start::Runtime
{
struct ArchiveCodecManagerRuntime;
class IBmrInputReadGuard;
class IBmrDestinationAccessGuard;

// 原445750没有长度/错误返回口；下列错误只属于明确的现代有界访问域。
enum class ArchiveLzssRuntimeError
{
    InputTruncated,
    DestinationOverflow,
};

class ArchiveLzssRuntimeException final : public std::exception
{
public:
    explicit ArchiveLzssRuntimeException(ArchiveLzssRuntimeError error) noexcept : error_(error) {}
    [[nodiscard]] ArchiveLzssRuntimeError Error() const noexcept { return error_; }
    [[nodiscard]] const char* what() const noexcept override;
private:
    ArchiveLzssRuntimeError error_;
};

// 5B208C：只有444E50一个虚槽，不增加C++虚析构或安全状态字段。
class ArchiveLzssRuntime
{
public:
    virtual void* DeleteAtVslot00(std::uint32_t flags);

    // 输入已由caller跳过输出长度DWORD；目标span仅为上界，终止不要求写满。
    // 窗口属于这份正文并跨调用保留，不在Decode内建立窗口字节副本。
    // 合法借用前提：正文跨全部guard回调存活，match只读该正文此前已写的字节
    // （包括本次match重叠写入）。原首次未写窗口没有确定值，本API不证明该资格，
    // 不设地址表/有效位镜像，不填0；违反此前提不属于有界入口承诺的定义域。
    void DecodeAt445750(std::span<const std::uint8_t> bitstreamAlreadySkip4,
        std::span<std::uint8_t> destination, IBmrInputReadGuard* inputGuard = nullptr,
        IBmrDestinationAccessGuard* destinationGuard = nullptr);
    void DecodeRawAt445750(const std::uint8_t* bitstreamAlreadySkip4,
        std::span<std::uint8_t> destination, IBmrInputReadGuard* inputGuard = nullptr,
        IBmrDestinationAccessGuard* destinationGuard = nullptr);
    // 两种输入共用一份逐bit算法；Raw不读Ref/count或探查OS地址范围。
    void DecodeAt445750(ArchiveCodecInputView input,
        std::span<std::uint8_t> destination, IBmrInputReadGuard* inputGuard = nullptr,
        IBmrDestinationAccessGuard* destinationGuard = nullptr);

    const std::uint8_t* inputAt04;
    std::uint8_t maskAt08;
    std::uint8_t unwrittenAt09[3];
    std::uint32_t currentAt0C;
    std::uint8_t windowAt10[0x1000];

    ArchiveLzssRuntime(const ArchiveLzssRuntime&) = delete;
    ArchiveLzssRuntime& operator=(const ArchiveLzssRuntime&) = delete;

protected:
    // 484100只写虚表。其余标量、padding和窗口都保持分配时的未写状态。
    ArchiveLzssRuntime() noexcept = default;
    ~ArchiveLzssRuntime() = default;

    friend ArchiveLzssRuntime* GetLzssAt4840B0(ArchiveCodecManagerRuntime& manager);
};
static_assert(sizeof(ArchiveLzssRuntime) == 0x1010);

// 原4840B0..484184为inline工厂；返回当前Ref解析结果，不缓存新正文或增持。
[[nodiscard]] ArchiveLzssRuntime* GetLzssAt4840B0(ArchiveCodecManagerRuntime& manager);
}
