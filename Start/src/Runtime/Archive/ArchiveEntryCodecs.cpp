#include <Runtime/ArchiveEntryCodecs.h>
#include <Runtime/ArchiveEntryReader.h>

#include <array>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
namespace
{
class BoundedLzssBits final
{
public:
    explicit BoundedLzssBits(std::span<const std::byte> bytes) noexcept : bytes_(bytes) {}
    std::uint32_t Read(unsigned count)
    {
        std::uint32_t result = 0;
        for (unsigned bit = 0; bit != count; ++bit)
        {
            if (!mask_)
            {
                if (offset_ == bytes_.size())
                    throw ArchiveEntryDecodeException(ArchiveEntryDecodeError::LzssBitstreamExhausted,
                        "LINK LZSS bit stream ended unexpectedly");
                current_ = std::to_integer<std::uint8_t>(bytes_[offset_++]);
                mask_ = 0x80;
            }
            result = (result << 1) | ((current_ & mask_) != 0 ? 1u : 0u);
            mask_ >>= 1;
        }
        return result;
    }
private:
    std::span<const std::byte> bytes_;
    std::size_t offset_ = 0;
    std::uint8_t current_ = 0, mask_ = 0;
};
}

void DecodeArchiveLzssPayload(std::span<const std::byte> payload,
    std::span<std::byte> destination)
{
    if (payload.size() < 4)
        throw ArchiveEntryDecodeException(ArchiveEntryDecodeError::LzssHeaderTruncated,
            "LINK LZSS payload is shorter than its size field");
    // 445750没有输出长度参数；这里只把调用者给出的物理容量当成安全上界。
    // 现代工具的窗口初态检查保留，不能把它说成原共享窗口每次清零。
    std::array<std::byte, 0x1000> window{};
    std::array<bool, 0x1000> initialized{};
    std::size_t written = 0, cursor = 1;
    BoundedLzssBits bits(payload.subspan(4));
    const auto emit = [&](std::byte byte)
    {
        if (written == destination.size())
            throw ArchiveEntryDecodeException(ArchiveEntryDecodeError::LzssOutputOverflow,
                "LINK LZSS stream writes past its declared output size");
        destination[written++] = byte;
        window[cursor] = byte;
        initialized[cursor] = true;
        cursor = (cursor + 1) & 0xFFFu;
    };
    for (;;)
    {
        if (bits.Read(1))
        {
            emit(static_cast<std::byte>(bits.Read(8)));
            continue;
        }
        const auto position = bits.Read(12);
        if (!position) break;
        const auto count = bits.Read(4) + 2;
        for (std::uint32_t i = 0; i != count; ++i)
        {
            const auto at = (position + i) & 0xFFFu;
            if (!initialized[at])
                throw ArchiveEntryDecodeException(ArchiveEntryDecodeError::LzssUninitializedWindowReference,
                    "LINK LZSS stream references a window byte that has not been written");
            emit(window[at]);
        }
    }
    if (written != destination.size())
        throw ArchiveEntryDecodeException(ArchiveEntryDecodeError::LzssOutputSizeMismatch,
            "LINK LZSS terminator was reached before the declared output size");
}
}
