#pragma once

#include <cstddef>
#include <cstdint>
#include <Runtime/ByteArrayReferenceRuntime.h>
#include <Runtime/ScriptHeaderGlobals.h>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
enum class ScrVersion : std::uint8_t
{
    Version5,
    Version5_1,
    Version5_2,
    Version5_3,
};

enum class ScrContainerError : std::uint8_t
{
    UnsupportedHeader,
    CodeSizeTruncated,
    CodeRangeOverflow,
    SaveMarkerMismatch,
    SaveCountTruncated,
    SaveTableOverflow,
    LayerMarkerMismatch,
    LayerCountTruncated,
    LayerTableOverflow,
    OffsetIndexOutOfRange,
};

class ScrContainerException final : public std::runtime_error
{
public:
    ScrContainerException(
        ScrContainerError error,
        std::size_t offset,
        const char* message);

    [[nodiscard]] ScrContainerError Error() const noexcept;
    [[nodiscard]] std::size_t Offset() const noexcept;

private:
    ScrContainerError error_;
    std::size_t offset_;
};

struct ScrParseDiagnostic final
{
    ScrContainerError error = ScrContainerError::UnsupportedHeader;
    std::uint32_t offset = 0;
};

// 原CScript+A0的唯一2Ch正文；偏移均相对文件开头，不追加vector/字符串副本。
class ScrContainer final
{
public:
    explicit ScrContainer(IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    ~ScrContainer() noexcept;
    ScrContainer(const ScrContainer&) = delete;
    ScrContainer& operator=(const ScrContainer&) = delete;
    // 仅供工具返回值转交，非原引擎业务入口。
    ScrContainer(ScrContainer&& source);
    ScrContainer& operator=(ScrContainer&&) = delete;

    void ResetAt4D4C30();
    void LoadAt4D5BA0(std::int32_t scriptId, const YtlReferenceSlotRuntime& input,
        const IScriptHeaderSource& headers = ProcessScriptHeaderSource(),
        ScrParseDiagnostic* diagnostic = nullptr);
    [[nodiscard]] std::int32_t ScriptId() const noexcept;
    [[nodiscard]] ScrVersion Version() const;
    [[nodiscard]] std::string_view Header() const noexcept;
    [[nodiscard]] std::uint32_t CodeOffset() const noexcept;
    [[nodiscard]] std::uint32_t CodeSize() const;
    [[nodiscard]] std::uint32_t SaveCount() const noexcept;
    [[nodiscard]] std::uint32_t SaveTableOffset() const noexcept;
    [[nodiscard]] std::uint32_t LayerCount() const noexcept;
    [[nodiscard]] std::uint32_t LayerTableOffset() const noexcept;
    [[nodiscard]] std::uint32_t ParsedEndOffset() const noexcept;
    [[nodiscard]] std::uint32_t SaveOffsetAt(std::uint32_t index) const;
    [[nodiscard]] std::uint32_t LayerOffsetAt(std::uint32_t index) const;
    [[nodiscard]] std::span<const std::byte> Bytes() const;
    [[nodiscard]] std::span<const std::byte> Code() const;
    [[nodiscard]] std::span<const std::byte> Tail() const;

    std::int32_t scriptIdAt00;
    union { ByteArrayReferenceRuntime bytesAt04; };
    std::uint32_t stagedPcAt10;
    std::uint32_t layerCountAt14;
    std::uint32_t layerOffsetAt18;
    std::uint32_t parsedEndAt1C;
    std::uint32_t saveCountAt20;
    std::uint32_t saveOffsetAt24;
    union { RuntimeCString headerAt28; };
};
static_assert(sizeof(ScrContainer) == 0x2C);
static_assert(offsetof(ScrContainer, bytesAt04) == 4 && offsetof(ScrContainer, stagedPcAt10) == 0x10);
static_assert(offsetof(ScrContainer, layerCountAt14) == 0x14 && offsetof(ScrContainer, headerAt28) == 0x28);

// 显式工具入口：先校验整个范围再构造原生正文；生产Load直接接reader的原Ref。
[[nodiscard]] ScrContainer ParseScrContainer(
    std::int32_t scriptId,
    std::vector<std::byte> bytes);
}
