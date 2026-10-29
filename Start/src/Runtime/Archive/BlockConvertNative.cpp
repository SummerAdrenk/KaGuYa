#include <Runtime/BlockConvertNative.h>
#include <Runtime/ArchiveCodecManagerRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>

#include <array>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
class RetiredBlockControl final : public EnvironmentObjectReferenceControl
{
public:
    RetiredBlockControl() noexcept : EnvironmentObjectReferenceControl(nullptr, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
};
class BlockControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit BlockControl(void* body) noexcept : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 448A10直接delete10h，正文没有析构或可调虚表。
        ::operator delete(payloadAt14, std::size_t{0x10});
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        std::array<std::byte, 0x14> fields;
        std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(this) + 4, fields.size());
        auto* retired = ::new (static_cast<void*>(this)) RetiredBlockControl;
        std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
        if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
    }
};
static_assert(sizeof(BlockControl) == 0x18 && sizeof(RetiredBlockControl) == 0x18);

std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}
std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b));
}
std::int32_t Mul(std::int32_t a, std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) * static_cast<std::uint32_t>(b));
}
template<class T> T* Offset(T* pointer, std::int32_t bytes) noexcept
{
    return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(pointer) + static_cast<std::uint32_t>(bytes));
}
std::uint32_t ReadDword(const std::uint8_t* p) noexcept
{
    return p[0] | (std::uint32_t{p[1]} << 8) | (std::uint32_t{p[2]} << 16) | (std::uint32_t{p[3]} << 24);
}
// PE 5B2090，throw-info为PAD；保持char*而不是const char*。
char IndexError[] = "\x83\x43\x83\x93\x83\x66\x83\x62\x83\x4E\x83\x58\x8F\xEE\x95\xF1"
    "\x82\xAA\x83\x47\x83\x89\x81\x5B\x81\x49\x81\x49";
}

BlockConvertNativeBody10h* GetBlockConvertAt445220(ArchiveCodecManagerRuntime& manager)
{
    auto& slot = manager.blockConvertAt24;
    if (!slot.baseAt00)
    {
        ReleaseYtlReference(slot);
        void* const body = ::operator new(std::size_t{0x10});
        if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
        slot.biasAt04 = 0;
        slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
        void* const storage = ::operator new(sizeof(BlockControl));
        auto* const control = storage ? ::new (storage) BlockControl(body) : nullptr;
        slot.controlAt08 = control;
        control->referenceCountAt04 = 1;
        slot.controlAt08->owningAt08 = 1;
        static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0x10;
        static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
    }
    return ResolveEnvironmentObjectReference<BlockConvertNativeBody10h>(slot);
}

void DecodeBlockConvertNativeAt445300(BlockConvertNativeBody10h& body,
    const std::uint8_t* previous, std::uint8_t* destination, const std::uint8_t* payload)
{
    // 块高度、行跨度和mask步长在原入口缓存；宽/通道/块步进则按下文原点重读。
    auto blockWidth = body.blockSizeAt0C;
    const auto maskBitRow = blockWidth < 8 ? blockWidth : 0;
    const auto rowBytes = Mul(body.channelsAt08, body.widthAt00);
    const auto maskBytes = Mul(blockWidth, blockWidth) >> 3;
    const auto commandBytes = static_cast<std::int32_t>(payload[4] | (std::uint32_t{payload[5]} << 8));
    auto auxiliary = Add(commandBytes, 10);
    auto raw = Add(auxiliary, std::bit_cast<std::int32_t>(ReadDword(Offset(payload, Add(commandBytes, 6)))));
    std::int32_t commandIndex = 0;
    unsigned commandBit = 0;
    std::int32_t y = 0;
    auto width = body.widthAt00;
    auto height = body.heightAt04;
    if (height <= 0) return;
    do
    {
        const auto blockHeight = Add(y, blockWidth) >= height ? Sub(height, y) : blockWidth;
        std::int32_t x = 0;
        if (width > 0)
        {
            do
            {
                if (Add(x, blockWidth) >= width) blockWidth = Sub(width, x);
                const auto bytesPerBlockRow = Mul(body.channelsAt08, blockWidth);
                unsigned mapIndex = 0;
                for (;;)
                {
                    if (commandIndex >= commandBytes) throw static_cast<char*>(IndexError);
                    const bool bit = (Offset(payload, Add(commandIndex, 6))[0] & (1u << (7u - commandBit))) != 0;
                    if (++commandBit == 8) { commandBit = 0; commandIndex = Add(commandIndex, 1); }
                    if (!bit || ++mapIndex == 3) break;
                }
                const auto opcode = payload[mapIndex];
                if (opcode == 0 || opcode == 1)
                {
                    auto rowOffset = Mul(y, rowBytes);
                    for (std::int32_t row = 0; row < blockHeight; ++row)
                    {
                        const auto at = Add(rowOffset, Mul(body.channelsAt08, x));
                        const auto* source = opcode == 0 ? Offset(previous, at) : Offset(payload, raw);
                        std::memmove(Offset(destination, at), source, static_cast<std::uint32_t>(bytesPerBlockRow));
                        if (opcode == 1) raw = Add(raw, bytesPerBlockRow);
                        rowOffset = Add(rowOffset, rowBytes);
                    }
                }
                else if (opcode == 2 || opcode == 3)
                {
                    std::int32_t replacements = 0;
                    std::int32_t replacementIndex = 0;
                    if (opcode == 3)
                    {
                        replacements = *Offset(payload, auxiliary);
                        auxiliary = Add(auxiliary, 1);
                    }
                    auto rowOffset = Mul(y, rowBytes);
                    auto channels = body.channelsAt08;
                    for (std::int32_t row = 0; row < blockHeight; ++row)
                    {
                        auto at = Add(rowOffset, Mul(channels, x));
                        for (std::int32_t column = 0; column < blockWidth; ++column)
                        {
                            bool changed;
                            if (opcode == 2)
                            {
                                // 445545按缓存小block计算bit，445569按当前block计算byte；不能合为一个索引。
                                const auto bit = Add(Mul(maskBitRow, row), column) % 8;
                                const auto maskAt = Add(auxiliary, Add(Mul(body.blockSizeAt0C, row), column) / 8);
                                changed = (*Offset(payload, maskAt) & (1u << (static_cast<unsigned>(7 - bit) & 31u))) != 0;
                            }
                            else
                            {
                                changed = replacementIndex < replacements &&
                                    Add(Mul(body.blockSizeAt0C, row), column) == *Offset(payload, Add(auxiliary, replacementIndex));
                            }
                            const auto* source = changed ? Offset(payload, raw) : Offset(previous, at);
                            std::memmove(Offset(destination, at), source, static_cast<std::uint32_t>(channels));
                            channels = body.channelsAt08;
                            if (changed)
                            {
                                raw = Add(raw, channels);
                                if (opcode == 3) replacementIndex = Add(replacementIndex, 1);
                            }
                            at = Add(at, channels);
                        }
                        rowOffset = Add(rowOffset, rowBytes);
                    }
                    auxiliary = Add(auxiliary, opcode == 2 ? maskBytes : replacements);
                }
                // >3的映射不消费aux/raw，也不写目标。每块末尾重新读正文步进与宽。
                blockWidth = body.blockSizeAt0C;
                x = Add(x, blockWidth);
                width = body.widthAt00;
            } while (x < width);
        }
        y = Add(y, blockWidth);
        height = body.heightAt04;
    } while (y < height);
}
}
