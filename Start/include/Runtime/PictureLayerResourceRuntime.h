#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>

namespace KaGuYa::Start::Runtime
{
struct ArchiveCodecServices;
enum class PictureLayerVersion : std::uint8_t
{
    Pl00,
    Pl01,
    Pl10,
    Pl11,
    Pl20,
    Pl30,
};

enum class PictureLayerResourceError : std::uint8_t
{
    HeaderTruncated,
    UnsupportedMagic,
    UnsupportedVersion,
    EmptyFrameSet,
    PayloadTruncated,
    ArithmeticOverflow,
    InvalidGeometry,
    InvalidRleStep,
    InvalidHuffmanTree,
    HuffmanBitstreamExhausted,
    InvalidBmrHeader,
    InvalidBwtKey,
    UnsupportedCompressionMode,
    InvalidLzssStream,
    InvalidBlockSize,
    BlockControlExhausted,
    DecodedSizeMismatch,
};

class PictureLayerResourceException final : public std::runtime_error
{
public:
    PictureLayerResourceException(
        PictureLayerResourceError error,
        std::size_t offset,
        const char* message);

    [[nodiscard]] PictureLayerResourceError Error() const noexcept;
    [[nodiscard]] std::size_t Offset() const noexcept;

private:
    PictureLayerResourceError error_;
    std::size_t offset_;
};

struct PictureLayerRect final
{
    std::int32_t x = 0;
    std::int32_t y = 0;
    std::int32_t width = 0;
    std::int32_t height = 0;
};

struct PictureLayerFrame final
{
    // fileOrigin 是 PL 文件中的原始局部坐标；surfaceOrigin 对应原 Start.exe
    // 写入 CSurface+16/+20 并最终交给 SpriteRuntime 的坐标。
    std::int32_t fileOriginX = 0;
    std::int32_t fileOriginY = 0;
    std::int32_t surfaceOriginX = 0;
    std::int32_t surfaceOriginY = 0;
    std::int32_t width = 0;
    std::int32_t height = 0;
    std::int32_t channels = 0;
    bool createWithAlpha = false;
    std::vector<std::byte> pixels;
};

struct ParsedPictureLayerResource final
{
    PictureLayerVersion version = PictureLayerVersion::Pl00;
    PictureLayerRect globalRect{};
    std::vector<PictureLayerFrame> frames;

    // 仅对应版本有意义。PL11 的 word 在 446FB0 中只被跳过，未解释业务含义。
    std::uint16_t pl11UnknownWord = 0;
    std::uint16_t pl20CompressionMode = 0;
    std::uint8_t pl30BlockSize = 0;
};

// Start.exe:4482A0 -> 447F60 -> 446BD0/4465C0/446FB0/447500/4479D0。
// 入口只接受精确的 PL00/01/10/11/20/30，不执行试读或版本回退。
// 当前返回现代帧快照，未声明完整PL/Surface Ref布局与分配顺序。
// 输入与codec正文须跨回调存活；LZSS首次未写窗口/临时输出未写尾部不得被读取。
// span只提供物理上界，不暗建有效位表、不填0，也不强制LZSS写满长度DWORD。
[[nodiscard]] ParsedPictureLayerResource ParsePictureLayerResource(
    std::span<const std::byte> bytes, const ArchiveCodecServices& codecs);

// 显式现代工具/旧夹具算法，不能由生产Sprite调用，不共享原codec窗口或owner。
[[nodiscard]] ParsedPictureLayerResource ParsePictureLayerResourceForTools(
    std::span<const std::byte> bytes);
}
