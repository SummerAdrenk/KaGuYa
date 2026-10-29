#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>

namespace KaGuYa::Start::Runtime
{
struct ArchiveCodecServices;
enum class AnimationResourceVersion : std::uint8_t
{
    An00,
    An10,
    An20,
    An30,
    An21,
};

enum class AnimationPictureVersion : std::uint8_t
{
    None,
    Pic00,
    Pic01,
    Pic10,
    Pic11,
    Pic20,
    Pic30,
};

enum class AnimationCommandOpcode : std::uint8_t
{
    NoOperation = 0,
    SetFrameMapping = 1,
    WaitTicks = 2,
    LoadBranchRepeatCounter = 3,
    DecrementBranchAndJumpIfPositive = 4,
    SetAnimationResultCode = 5,
};

struct AnimationCommand final
{
    AnimationCommandOpcode opcode = AnimationCommandOpcode::NoOperation;
    std::int32_t first = 0;
    std::int32_t second = 0;

    bool operator==(const AnimationCommand&) const = default;
};

struct AnimationBranch final
{
    std::int32_t jumpCommandIndex = -1;
    std::int32_t repeatCount = -1;

    bool operator==(const AnimationBranch&) const = default;
};

enum class AnimationFramePixelStorage : std::uint8_t
{
    SourceResource,
    DecodedBytes,
};

struct AnimationFrameDescriptor final
{
    std::int32_t originX = 0;
    std::int32_t originY = 0;
    std::int32_t width = 0;
    std::int32_t height = 0;
    std::int32_t channels = 0;
    AnimationFramePixelStorage pixelStorage =
        AnimationFramePixelStorage::SourceResource;
    std::size_t pixelOffset = 0;
    std::size_t pixelLength = 0;
    std::vector<std::byte> decodedPixels;

    [[nodiscard]] std::span<const std::byte> Pixels(
        std::span<const std::byte> sourceResource) const;

    bool operator==(const AnimationFrameDescriptor&) const = default;
};

struct ParsedAnimationResource final
{
    AnimationResourceVersion version = AnimationResourceVersion::An00;
    AnimationPictureVersion pictureVersion = AnimationPictureVersion::None;
    std::int32_t canvasX = 0;
    std::int32_t canvasY = 0;
    std::int32_t canvasWidth = 0;
    std::int32_t canvasHeight = 0;
    bool controlFlag = false;
    std::uint8_t controlReservedByte = 0;
    std::uint16_t pictureReservedWord = 0;
    std::uint8_t pictureReservedByte = 0;
    std::uint16_t pictureCompressionMode = 0;
    std::int32_t pictureChannels = 0;
    std::uint8_t pictureBlockSize = 0;
    std::int32_t frameMappingSlotCount = 0;
    std::vector<AnimationCommand> commands;
    std::vector<AnimationBranch> branches;
    std::vector<AnimationFrameDescriptor> frames;
    std::size_t consumedBytes = 0;
};

enum class AnimationParseState : std::uint8_t
{
    Success,
    NotAnimation,
};

enum class AnimationResourceError : std::uint8_t
{
    HeaderTruncated,
    UnsupportedVersion,
    EmptyCommandTable,
    InvalidCommandOpcode,
    MissingFrameMappingCommand,
    InvalidCommandReference,
    MissingPictureMarker,
    UnsupportedPictureVersion,
    EmptyFrameTable,
    FramePayloadSizeOverflow,
    FramePayloadTruncated,
    InvalidFramePayloadShape,
    UnsupportedCompressionMode,
    CompressedPayloadMalformed,
    DecompressedSizeMismatch,
    BlockPayloadMalformed,
};

class AnimationResourceException final : public std::runtime_error
{
public:
    AnimationResourceException(
        AnimationResourceError error,
        const char* message);

    [[nodiscard]] AnimationResourceError Error() const noexcept;

private:
    AnimationResourceError error_;
};

// 448010/445B90：非 "AN" 返回 NotAnimation；已识别但损坏的 AN 必须明确报错。
// PIC11/20借用同一共享codec来源；输入和codec正文须跨回调存活。
// 返回值仍是现代帧快照，不代表完整AN/Surface Ref布局及上传回调已复刻。
[[nodiscard]] AnimationParseState ParseAnimationResource(
    std::span<const std::byte> bytes,
    ParsedAnimationResource& output,
    const ArchiveCodecServices& codecs);

// 显式工具/旧夹具入口；局部压缩算法不允许作为生产入口的缺省路径。
[[nodiscard]] AnimationParseState ParseAnimationResourceForTools(
    std::span<const std::byte> bytes,
    ParsedAnimationResource& output);
}
