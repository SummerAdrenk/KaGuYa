#pragma once

#include <Runtime/ArchiveEntryReader.h>

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class ArchiveReaderReference;
struct ArchiveCodecServices;

struct ArchiveKeyView final
{
    std::span<const std::uint8_t> storage;
    std::size_t index = 0;
    std::size_t count = 0;
};

// 严格工具接口：转换原引擎选中的图像数据区，容器头保持明文；语义错误向调用者报告。
[[nodiscard]] std::vector<std::byte> ApplyArchiveKeyConvert(
    std::span<const std::byte> payload,
    ArchiveKeyView key,
    std::uint32_t archiveVersion);

[[nodiscard]] std::vector<std::byte> ApplyArchiveKeyConvert(
    std::span<const std::byte> payload,
    std::span<const std::uint8_t> key,
    std::uint32_t archiveVersion);

// 对应 0x483D90 的 if/else-if/else-if，组合 flags 严格保持 1 > 2 > 4 的优先级。
[[nodiscard]] std::vector<std::byte> TransformArchiveEntryPayload(
    const ResolvedResourceEntry& entry,
    std::span<const std::byte> payload,
    std::span<const std::uint8_t> key);

// 现代vector工具原位变换：46E200 内部捕获 AN/PL 的明确语义异常，保留已转换状态。
// 现代范围/溢出检查不能冒充原异常；仍向外报告，不将越界输入变成伪成功。
void TransformArchiveEntryPayloadInPlace(
    const ResolvedResourceEntry& entry,
    std::vector<std::byte>& payload,
    std::span<const std::uint8_t> key);

// 原生工厂借用唯一outer Ref，在各原取值点Get()->BytesAt4172C0()。
// key须由Factory在原480380时点取得并保活；这里不重取或复制key。
// codec仍为显式现代有界适配：LZSS共享窗口、BMR共享445110 owner尚未迁移；
// BMR局部decoder失败时清Header，不能声称具有原跨调用Header保留资格。
// BMR识别/解码的ParamsRawBlobInstallException映射为归档BmrMalformed，保留工具错误分类；
// 不捕获分配、范围验证或外层getter/提交回调异常，不改变合法输入的原调用顺序。
// BMR只验证已捕获输入/目标span的范围，不回读旧reader/control，不因换槽拒绝；
// 缓存内存的存活属于原借用调用前提，不通过增持、复制或猜测liveness补足。
void TransformArchiveEntryPayloadInPlace(
    const ResolvedResourceEntry& entry,
    const ArchiveReaderReference& reader,
    std::span<const std::uint8_t> key);

// 生产入口：同一共享manager、原Huffman报告/throw，不使用上述局部工具codec。
void TransformArchiveEntryPayloadInPlace(
    const ResolvedResourceEntry& entry, const ArchiveReaderReference& reader,
    std::span<const std::uint8_t> key, const ArchiveCodecServices& codecs);
}
