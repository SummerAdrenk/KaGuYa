#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>

namespace KaGuYa::Start::Runtime
{
enum class ParamsRawBlobStorageKind
{
    Plain,
    Bmr
};

enum class ParamsRawBlobInstallError
{
    HeaderTruncated,
    HuffmanStreamOutOfRange,
    HuffmanTreeExhausted,
    HuffmanLeafExhausted,
    HuffmanNodeLimit,
    HuffmanPayloadExhausted,
    InvalidSizeRelationship,
    InvalidBwtKey,
    RleStreamExhausted,
    DestinationSizeMismatch,
    RleDestinationOutOfRange,
    MissingDecoderHeader,
    BorrowedInputInvalidated,
    BorrowedDestinationInvalidated
};

class ParamsRawBlobInstallException final : public std::runtime_error
{
public:
    ParamsRawBlobInstallException(ParamsRawBlobInstallError error, const char* message);

    [[nodiscard]] ParamsRawBlobInstallError Error() const noexcept;

private:
    ParamsRawBlobInstallError error_;
};

struct InstalledParamsRawBlob
{
    ParamsRawBlobStorageKind storageKind = ParamsRawBlobStorageKind::Plain;
    std::vector<std::uint8_t> bytes;
};

struct ParamsRawBlobRuntimeLayout final
{
    ParamsRawBlobStorageKind storageKind;
    std::size_t byteCount;
};

class ParamsRawBlobInstaller final
{
public:
    // 工具用事务式入口；成功时返回完整结果，不模拟原根对象的部分发布。
    [[nodiscard]] static InstalledParamsRawBlob Install(
        std::span<const std::uint8_t> source);

    // 445A00只读取16字节头；Huffman长度及正文检查必须留在目标发布之后。
    [[nodiscard]] static ParamsRawBlobRuntimeLayout InspectRuntimeSource(
        std::span<const std::uint8_t> source);
    // 43DCF0直接修改已发布数组，异常时保留已写前缀；source与destination不得重叠。
    // 畸形数据导致的原版越界访问显式报错，不模拟越界，也不静默裁剪。
    static void InstallIntoRuntime(std::span<const std::uint8_t> source,
        std::span<std::uint8_t> destination);
};

// 新原生Header owner分阶段调用；旧工具入口保留原结果与校验次序。
// 这些算法仍使用现代Huffman状态及BWT临时vector，不冒充原singleton/Ref分配链。
class IBmrInputReadGuard;
class IBmrDestinationAccessGuard;
namespace BmrDecoderAlgorithms
{
void DecodeHuffman(std::span<const std::uint8_t> encoded, std::span<std::uint8_t> output,
    IBmrInputReadGuard* inputGuard = nullptr, IBmrDestinationAccessGuard* destinationGuard = nullptr);
void MoveToFront(std::span<std::uint8_t> bytes) noexcept;
void BurrowsWheeler(std::span<std::uint8_t> bytes, std::uint32_t key,
    IBmrDestinationAccessGuard* destinationGuard = nullptr);
void RunLength(std::span<const std::uint8_t> input, std::span<std::uint8_t> output, std::uint8_t step);
}
}
