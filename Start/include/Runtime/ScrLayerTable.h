#pragma once

#include <Runtime/ScrContainer.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <vector>

namespace KaGuYa::Start::Runtime
{
enum class ScrLayerTableError : std::uint8_t
{
    LayerIndexOutOfRange,
    LayerOffsetOutOfRange,
    LayerOffsetOrderInvalid,
    RecordHeaderTruncated,
    RecordRangeOverflow,
    RecordPayloadOverflow,
};

class ScrLayerTableException final : public std::runtime_error
{
public:
    ScrLayerTableException(
        ScrLayerTableError error,
        std::size_t offset,
        const char* message);

    [[nodiscard]] ScrLayerTableError Error() const noexcept;
    [[nodiscard]] std::size_t Offset() const noexcept;

private:
    ScrLayerTableError error_;
    std::size_t offset_;
};

// 字段名对应 0x4D3670 写入 ResultLayer 的对象 offset；业务名称等待 0x4CB030 消费链确认。
struct ScrLayerRecord final
{
    std::uint16_t recordSize = 0;
    std::uint32_t valueAt00 = 0;
    std::uint8_t valueAt04 = 0;
    std::int16_t valueAt08 = 0;
    std::int16_t valueAt0C = 0;
    std::uint8_t valueAt10 = 0;
    std::int8_t valueAt18 = 0;
    std::vector<std::uint32_t> extrasAt1C;

    bool operator==(const ScrLayerRecord&) const = default;
};

// 对应 0x4D3670。负索引和超出组内 entryCount 返回空；超出 layerCount 的正索引明确拒绝。
[[nodiscard]] std::optional<ScrLayerRecord> FindScrLayerRecord(
    const ScrContainer& container,
    std::int32_t layerIndex,
    std::int32_t entryIndex);
}
