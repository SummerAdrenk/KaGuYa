#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/EnvironmentObjectReference.h>

#include <algorithm>
#include <limits>
#include <mbstring.h>
#include <new>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
EnvironmentKeyByteTable::EnvironmentKeyByteTable(InstalledParamsRawBlob installedRawBlob)
    : storageKind_(installedRawBlob.storageKind)
{
    // 工具来源拒绝空表；生产FileManager直接持有Ref，允许原部分发布/空槽状态。
    if (installedRawBlob.bytes.empty())
        throw std::invalid_argument("installed Environment key-byte table must not be empty");
    bytes_.AllocateAt412700(static_cast<std::uint32_t>(installedRawBlob.bytes.size()));
    if (bytes_.Size() != installedRawBlob.bytes.size())
        throw std::length_error("installed key exceeds the signed allocation range");
    std::copy(installedRawBlob.bytes.begin(), installedRawBlob.bytes.end(), bytes_.Data());
}

std::uint8_t ReadNormalizedKeyByteAt47CB00(
    const ByteArrayReferenceRuntime& bytes, const std::uint32_t index)
{
    const auto count = bytes.Size();
    // 原零长度循环不退出、负index会越界；明确拒绝这两类故障，不伪造可用key。
    if (!bytes || count == 0 || count > 0x7FFFFFFFu)
        throw std::logic_error("47CB00: no positive installed key length");
    if (index > 0x7FFFFFFFu)
        throw std::out_of_range("47CB00: negative signed key index");
    // 当前已选定表的合法非负索引，反复减同一count与取模相等；保留bias/stride寻址。
    const auto& slot = bytes.RawSlot();
    const auto stride = static_cast<const EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C;
    const auto address = slot.baseAt00 + (slot.biasAt04 + index % count) * stride;
    const auto value = *reinterpret_cast<const std::uint8_t*>(address);
    return value == 0 ? 0xFF : value;
}

std::uint8_t EnvironmentKeyByteTable::NormalizedByteAt(const std::uint32_t index) const
{
    return ReadNormalizedKeyByteAt47CB00(bytes_, index);
}

std::span<const std::uint8_t> EnvironmentKeyByteTable::Bytes() const noexcept
{
    return bytes_.Bytes();
}

ByteArrayReferenceRuntime EnvironmentKeyByteTable::AcquireRawBytes() const noexcept
{
    return bytes_;
}

ParamsRawBlobStorageKind EnvironmentKeyByteTable::StorageKind() const noexcept
{
    return storageKind_;
}


ClickInfoArchive::ClickInfoArchive() noexcept
    : bytesAt04{2, 0, 0, 0, 1, 0, 0, 0},
      bytesAt0C{1, 0, 0, 0, 2, 0, 0, 0},
      bytesAt14{3, 0, 0, 0, 1, 0, 0, 0}
{
    // 47E3CE..47E3F1写六个DWORD；47E3F8..47E429将尾部32字节全部置零。
}

void ClickInfoArchive::Serialize(SerializeBuffer& serializer)
{
    // Start.exe:48AFB0 通过 452020 连续处理三个 8 字节区块。
    // 当前值缓冲仅保留稳定模式下的字节结果；尚未表达452020每块取mode、
    // 每字节取vector，以及读时先递增cursor再取data的虚调用/故障时序。
    for (std::uint8_t& value : bytesAt04)
        serializer.SerializeSlot50Byte(value);
    for (std::uint8_t& value : bytesAt0C)
        serializer.SerializeSlot50Byte(value);
    for (std::uint8_t& value : bytesAt14)
        serializer.SerializeSlot50Byte(value);
}

void* ClickInfoArchive::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    // 468740退化为IArchive并保留全部56字节数据；仅flags位0置位时释放0x3C。
    this->~ClickInfoArchive();
    if (flags & 1u) ::operator delete(this, OriginalObjectSize);
    return this;
}

GlobalParamsArchive::GlobalParamsArchive() noexcept
    : wordsAt0C{}
{
    // Start.exe:47E4C0..47E4F1。其余 29 个 word 由前置 memset 保持为零。
    wordsAt0C[2] = 0xFFFF;
    wordsAt0C[3] = 0xFFFF;
    wordsAt0C[6] = 1;
    highKeyOffsetAt08 = 0;
    lowKeyOffsetAt04 = 0;
}

void GlobalParamsArchive::Serialize(SerializeBuffer& serializer)
{
    // 48B03B只取一次分支；key回调可以改字段，low/high不能合成无序表达式或预取。
    const bool writing = serializer.IsWriting();
    for (std::uint32_t index = 0; index < wordsAt0C.size(); ++index)
    {
        const auto low = serializer.EnvironmentKeys().NormalizedByteAt(lowKeyOffsetAt04 + index);
        const auto high = serializer.EnvironmentKeys().NormalizedByteAt(highKeyOffsetAt08 + index);
        const std::uint16_t key = static_cast<std::uint16_t>(
            low | (static_cast<std::uint16_t>(high) << 8));
        // 48B09C在两次key调用后取原word；读分支不提前读取目标的未写内容。
        std::uint16_t encoded = writing ? static_cast<std::uint16_t>(wordsAt0C[index] ^ key) : 0;
        serializer.SerializeSlot40Word(encoded);
        if (!writing)
        {
            wordsAt0C[index] = static_cast<std::uint16_t>(encoded ^ key);
        }
    }
}

void* GlobalParamsArchive::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    // 468BC0先退化IArchive；flags0不释放正文，后续基类删除只有4byte大小。
    this->~GlobalParamsArchive();
    if (flags & 1u) ::operator delete(this, OriginalObjectSize);
    return this;
}
}
