#include <Runtime/ScrContainer.h>
#include <Runtime/EnvironmentObjectReference.h>

#include <array>
#include <cstring>
#include <new>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
// parser只在解Ref后连续读一个标记/DWORD；stride不是逐字符步长。
const std::byte* ReadAddress(const ByteArrayReferenceRuntime& bytes, std::uint32_t index,
    std::size_t length, ScrContainerError error)
{
    const auto& ref = bytes.RawSlot();
    auto* address = bytes.DataAt(index);
    if (!address) throw ScrContainerException(error, index, "SCR null read address");
    if (ref.controlAt08)
    {
        const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(ref.controlAt08);
        const std::uint32_t offset = (ref.biasAt04 + index) * control->strideAt0C;
        const std::uint64_t extent = std::uint64_t{control->countAt10} * control->strideAt0C;
        if (offset > extent || length > extent - offset)
            throw ScrContainerException(error, index, "SCR read exceeds reference allocation");
    }
    // 无control却有base的原Ref仍按stride=0解析；长度不能从零control臆造出来。
    return reinterpret_cast<const std::byte*>(address);
}
std::uint32_t ReadWord(const ByteArrayReferenceRuntime& bytes, std::uint32_t index,
    ScrContainerError error)
{
    std::uint32_t result;
    std::memcpy(&result, ReadAddress(bytes, index, 4, error), 4);
    return result;
}
bool MatchesHeader(const ByteArrayReferenceRuntime& bytes, const RuntimeCString& header)
{
    const auto length = static_cast<std::size_t>(header.GetLength());
    if (!length) return true;
    // 原版对短输入可能越界；现代安全入口将不足完整header视为不匹配，不越界探读。
    try
    {
        return std::memcmp(ReadAddress(bytes, 0, length, ScrContainerError::UnsupportedHeader),
            header.GetString(), length) == 0;
    }
    catch (const ScrContainerException&) { return false; }
}
[[noreturn]] void MarkerFailure(ScrContainer& parser, ScrContainerError kind,
    std::uint32_t offset, char* message, ScrParseDiagnostic* diagnostic)
{
    if (diagnostic) *diagnostic = {kind, offset};
    parser.ResetAt4D4C30();
    // 原 __TI2PAD 是char*而非const char*；只读常量的内容不允许调用者修改。
    throw message;
}
}

ScrContainer::ScrContainer(IRuntimeStringManagerSource& strings)
{
    ::new (static_cast<void*>(&bytesAt04)) ByteArrayReferenceRuntime;
    struct Unwind final
    {
        ScrContainer& body;
        bool header = false;
        bool active = true;
        ~Unwind() noexcept
        {
            if (!active) return;
            if (header) DestroyRuntimeCStringAt40CD40(&body.headerAt28);
            body.bytesAt04.~ByteArrayReferenceRuntime();
        }
    } unwind{*this};
    InitializeEmptyRuntimeCString(headerAt28, strings);
    unwind.header = true;
    scriptIdAt00 = -1;
    ResetAt4D4C30();
    unwind.active = false;
}

ScrContainer::~ScrContainer() noexcept
{
    // 4C9EF0：CString先释放，随后SCR Ref；没有隐式成员析构导致的二次减持。
    DestroyRuntimeCStringAt40CD40(&headerAt28);
    bytesAt04.~ByteArrayReferenceRuntime();
}

ScrContainer::ScrContainer(ScrContainer&& source) : ScrContainer()
{
    // 工具返回值转交；用字节复制未发布的数值字段，不能把未初始化值求值。
    std::memcpy(&scriptIdAt00, &source.scriptIdAt00, sizeof(scriptIdAt00));
    std::memcpy(&stagedPcAt10, &source.stagedPcAt10, 0x18);
    std::swap(bytesAt04.RawSlot(), source.bytesAt04.RawSlot());
    std::array<std::byte, 4> nil;
    std::memcpy(nil.data(), &headerAt28, 4);
    std::memcpy(&headerAt28, &source.headerAt28, 4);
    std::memcpy(&source.headerAt28, nil.data(), 4);
    source.scriptIdAt00 = -1;
    source.stagedPcAt10 = 0;
}

void ScrContainer::ResetAt4D4C30()
{
    scriptIdAt00 = -1;
    bytesAt04.Reset();
    stagedPcAt10 = 0;
    SetRuntimeCStringAt437680(headerAt28, "", 0);
    // +14..+24在原reset中完全不写；失败前已发布计数保持。
}

void ScrContainer::LoadAt4D5BA0(std::int32_t scriptId, const YtlReferenceSlotRuntime& input,
    const IScriptHeaderSource& headers, ScrParseDiagnostic* diagnostic)
{
    struct Argument final
    {
        YtlReferenceSlotRuntime slot;
        bool active = true;
        ~Argument() noexcept { if (active) ReleaseYtlReference(slot); }
    } argument{input};
    if (argument.slot.controlAt08) ++argument.slot.controlAt08->referenceCountAt04;
    scriptIdAt00 = scriptId;
    auto& destination = bytesAt04.RawSlot();
    ReleaseYtlReference(destination);
    destination = argument.slot;
    if (destination.controlAt08) ++destination.controlAt08->referenceCountAt04;

    const RuntimeCString* selected = nullptr;
    for (std::size_t index = 0; index < ScriptHeaderCount; ++index)
    {
        const auto& header = headers.Borrow(static_cast<ScriptHeaderId>(index));
        if (MatchesHeader(bytesAt04, header)) { selected = &header; break; }
    }
    if (!selected)
        MarkerFailure(*this, ScrContainerError::UnsupportedHeader, 0,
            const_cast<char*>("[SCRIPT] : error !!"), diagnostic);
    AssignRuntimeCStringAt410220(headerAt28, *selected);

    // 4D5E55捕获新header指针；后续无回调，偏移加法保留DWORD回绕。
    const auto headerLength = static_cast<std::uint32_t>(headerAt28.GetLength());
    const auto codeSize = ReadWord(bytesAt04, headerLength, ScrContainerError::CodeSizeTruncated);
    const auto saveMarker = headerLength + 4u + codeSize;
    if (std::memcmp(ReadAddress(bytesAt04, saveMarker, 6, ScrContainerError::CodeRangeOverflow), "[SAVE]", 6))
        MarkerFailure(*this, ScrContainerError::SaveMarkerMismatch, saveMarker,
            const_cast<char*>("[SAVE] : error !!"), diagnostic);
    saveCountAt20 = ReadWord(bytesAt04, saveMarker + 6u, ScrContainerError::SaveCountTruncated);
    saveOffsetAt24 = saveMarker + 10u;
    const auto layerMarker = saveOffsetAt24 + saveCountAt20 * 4u;
    if (std::memcmp(ReadAddress(bytesAt04, layerMarker, 7, ScrContainerError::SaveTableOverflow), "[LAYER]", 7))
        MarkerFailure(*this, ScrContainerError::LayerMarkerMismatch, layerMarker,
            const_cast<char*>("[LAYER] : error !!"), diagnostic);
    layerCountAt14 = ReadWord(bytesAt04, layerMarker + 7u, ScrContainerError::LayerCountTruncated);
    layerOffsetAt18 = layerMarker + 11u;
    parsedEndAt1C = layerOffsetAt18 + layerCountAt14 * 4u;
    // 4D5FA7之后不读layer表，不在这里添加全表预校验或重置staged PC。
    argument.active = false;
    ReleaseYtlReference(argument.slot);
}

std::int32_t ScrContainer::ScriptId() const noexcept { return scriptIdAt00; }
ScrVersion ScrContainer::Version() const
{
    for (std::size_t index = 0; index < ScriptHeaderCount; ++index)
        if (Header() == ScriptHeaderLiteral(static_cast<ScriptHeaderId>(index)))
            return static_cast<ScrVersion>(index);
    throw std::logic_error("SCR version queried before a recognized header");
}
std::string_view ScrContainer::Header() const noexcept { return RuntimeStringView(headerAt28); }
std::uint32_t ScrContainer::CodeOffset() const noexcept { return static_cast<std::uint32_t>(headerAt28.GetLength()) + 4u; }
std::uint32_t ScrContainer::CodeSize() const { return ReadWord(bytesAt04, CodeOffset() - 4u, ScrContainerError::CodeSizeTruncated); }
std::uint32_t ScrContainer::SaveCount() const noexcept { return saveCountAt20; }
std::uint32_t ScrContainer::SaveTableOffset() const noexcept { return saveOffsetAt24; }
std::uint32_t ScrContainer::LayerCount() const noexcept { return layerCountAt14; }
std::uint32_t ScrContainer::LayerTableOffset() const noexcept { return layerOffsetAt18; }
std::uint32_t ScrContainer::ParsedEndOffset() const noexcept { return parsedEndAt1C; }
std::uint32_t ScrContainer::SaveOffsetAt(std::uint32_t index) const
{
    if (index >= saveCountAt20) throw ScrContainerException(ScrContainerError::OffsetIndexOutOfRange,
        saveOffsetAt24, "SCR save offset index is outside the parsed table");
    return ReadWord(bytesAt04, saveOffsetAt24 + index * 4u, ScrContainerError::SaveTableOverflow);
}
std::uint32_t ScrContainer::LayerOffsetAt(std::uint32_t index) const
{
    if (index >= layerCountAt14) throw ScrContainerException(ScrContainerError::OffsetIndexOutOfRange,
        layerOffsetAt18, "SCR layer offset index is outside the parsed table");
    return ReadWord(bytesAt04, layerOffsetAt18 + index * 4u, ScrContainerError::LayerTableOverflow);
}
std::span<const std::byte> ScrContainer::Bytes() const
{
    const auto& ref = bytesAt04.RawSlot();
    if (!ref.baseAt00) return {};
    const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(ref.controlAt08);
    if (ref.biasAt04 || !control || control->strideAt0C != 1)
        throw std::logic_error("SCR contiguous view requires unbiased byte reference");
    return {reinterpret_cast<const std::byte*>(ref.baseAt00), control->countAt10};
}
std::span<const std::byte> ScrContainer::Code() const
{
    const auto bytes = Bytes();
    const auto offset = CodeOffset(), length = CodeSize();
    if (offset > bytes.size() || length > bytes.size() - offset)
        throw ScrContainerException(ScrContainerError::CodeRangeOverflow, offset, "SCR code view out of range");
    return bytes.subspan(offset, length);
}
std::span<const std::byte> ScrContainer::Tail() const
{
    const auto bytes = Bytes();
    if (parsedEndAt1C > bytes.size())
        throw ScrContainerException(ScrContainerError::LayerTableOverflow, parsedEndAt1C, "SCR tail view out of range");
    return bytes.subspan(parsedEndAt1C);
}
}
