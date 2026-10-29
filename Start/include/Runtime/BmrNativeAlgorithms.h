#pragma once

#include <cstdint>
#include <span>

namespace KaGuYa::Start::Runtime
{
class BmrDecoderRuntime;
class IBmrDestinationAccessGuard;

namespace BmrNativeAlgorithms
{
// 43E320使用真实ulong[]/byte[] Ref；正常缓存控制块，异常按当前槽逆序释放。
void BurrowsWheelerAt43E320(std::span<std::uint8_t> bytes, std::uint32_t key,
    IBmrDestinationAccessGuard* destinationGuard = nullptr);

// 43E17D..43E253：finalSize只控制下一组读取，游程写完才比较；extent是物理访问边界。
void RunLengthAt43E17D(std::span<const std::uint8_t> input, std::span<std::uint8_t> destination,
    std::uint32_t finalSize, std::uint8_t step,
    IBmrDestinationAccessGuard* destinationGuard = nullptr);

// Huffman完成后的43DFF5..43E292；Header字段在各原访问点读取，不缓存整份Header。
void FinishAt43DFF5(BmrDecoderRuntime& decoder, std::span<std::uint8_t> destination,
    std::uint32_t intermediateCount, std::span<const std::uint8_t, 256> initialDictionary,
    IBmrDestinationAccessGuard* destinationGuard = nullptr);
}
}
