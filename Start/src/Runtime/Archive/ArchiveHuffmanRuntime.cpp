#include <Runtime/ArchiveHuffmanRuntime.h>
#include <Runtime/ArchiveCodecManagerRuntime.h>
#include <Runtime/BmrDecoderRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/LoadSpriteErrorEnvironment.h>

#include <bit>
#include <cstdlib>
#include <cstring>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原PE 5AEC20/5AEC40/5AEC64逐字节CP932；不依赖编译机代码页。
constexpr char BitError[] = "\x93\xFC\x97\xCD\x83\x47\x83\x89\x81\x5B : CBitIO::Getbit()";
constexpr char LeafError[] = "\x93\xFC\x97\xCD\x83\x47\x83\x89\x81\x5B : CBitIO::Getbit(int n)";
constexpr char TreeError[] = "\x95\x5C\x82\xAA\x8A\xD4\x88\xE1\x82\xC1\x82\xC4\x82\xA2\x82\xDC\x82\xB7";

[[noreturn]] void ReportAndThrow(IArchiveHuffmanErrorSource& errors, const char* text, int value)
{
    errors.ReportAt420C50(text);
    throw value;
}

class RetiredHuffmanControl final : public EnvironmentObjectReferenceControl
{
public:
    RetiredHuffmanControl() noexcept
        : EnvironmentObjectReferenceControl(nullptr, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 5ADD18.slot04=412460，flags0退休后的后续删除是14h。
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
};

class HuffmanControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit HuffmanControl(ArchiveHuffmanRuntime* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 43E910无SEH：缓存正文，scratch仅按capacity-begin字节退役，不推断元素析构。
        auto* const body = static_cast<ArchiveHuffmanRuntime*>(payloadAt14);
        if (!body) return;
        auto& scratch = body->scratchAt18;
        if (scratch.begin)
        {
            void* raw = scratch.begin;
            auto bytes = reinterpret_cast<std::uintptr_t>(scratch.capacity) -
                reinterpret_cast<std::uintptr_t>(scratch.begin);
            if (bytes >= 0x1000)
            {
                std::memcpy(&raw, reinterpret_cast<const std::byte*>(scratch.begin) - 4, sizeof(raw));
                bytes += 0x23;
                const auto distance = reinterpret_cast<std::uintptr_t>(scratch.begin) -
                    reinterpret_cast<std::uintptr_t>(raw) - 4;
                if (distance > 0x1F) _invalid_parameter_noinfo_noreturn();
            }
            ::operator delete(raw, static_cast<std::size_t>(bytes));
            scratch.begin = nullptr;
            scratch.end = nullptr;
            scratch.capacity = nullptr;
        }
        ::operator delete(body, std::size_t{0x1020});
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 43DCC0先退虚表，首次删除18h；不使用增加noexcept的通用退休函数。
        std::array<std::byte, 0x14> fields;
        std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(this) + 4, fields.size());
        auto* retired = ::new (static_cast<void*>(this)) RetiredHuffmanControl;
        std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
        if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
    }
};
static_assert(sizeof(HuffmanControl) == 0x18 && sizeof(RetiredHuffmanControl) == 0x18);

void ValidateInput(ArchiveCodecInputView extent, const std::uint8_t* address,
    std::size_t count, IBmrInputReadGuard* guard)
{
    if (!extent.Contains(address, count))
        throw std::out_of_range("Huffman input exceeds captured extent (safe boundary)");
    if (guard) guard->ValidateRead({address, count});
}

std::uint8_t ReadInputByte(ArchiveHuffmanRuntime& body, std::uint32_t index,
    ArchiveCodecInputView extent, IBmrInputReadGuard* guard)
{
    const auto* address = reinterpret_cast<const std::uint8_t*>(
        reinterpret_cast<std::uintptr_t>(body.inputAt14) + index);
    ValidateInput(extent, address, 1, guard);
    return *address;
}

std::int32_t ReadBitAt43E640(ArchiveHuffmanRuntime& body,
    ArchiveCodecInputView extent, IArchiveHuffmanErrorSource& errors,
    IBmrInputReadGuard* guard)
{
    const auto remaining = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(body.remainingBitsAt00) - 1u);
    body.remainingBitsAt00 = remaining;
    if (remaining >= 0) return (body.currentByteAt08 >> (remaining & 31)) & 1;
    const auto index = body.readBytesAt0C;
    if (index >= body.inputSizeAt10) ReportAndThrow(errors, BitError, 1);
    // 43E666先写7，实际输入读取失败不能回滚此字段。
    body.remainingBitsAt00 = 7;
    const auto byte = ReadInputByte(body, index, extent, guard);
    body.currentByteAt08 = byte;
    body.readBytesAt0C = index + 1u;
    return byte >> 7;
}

std::int32_t ReadLeafAt43E7AF(ArchiveHuffmanRuntime& body,
    ArchiveCodecInputView extent, IArchiveHuffmanErrorSource& errors,
    IBmrInputReadGuard* guard)
{
    auto remaining = body.remainingBitsAt00;
    std::uint32_t needed = 8;
    std::uint32_t value = 0;
    if (remaining < 8)
    {
        auto current = static_cast<std::uint32_t>(body.currentByteAt08);
        auto index = body.readBytesAt0C;
        do
        {
            needed -= static_cast<std::uint32_t>(remaining);
            value |= (current & ((1u << (remaining & 31)) - 1u)) << (needed & 31);
            // 43E7E1失败不写remaining；不能用八次ReadBit替代这条叶值读取路径。
            if (index >= body.inputSizeAt10) ReportAndThrow(errors, LeafError, 2);
            current = ReadInputByte(body, index, extent, guard);
            remaining = 8;
            ++index;
            body.currentByteAt08 = static_cast<std::int32_t>(current);
            body.readBytesAt0C = index;
            body.remainingBitsAt00 = 8;
        } while (needed > 8);
    }
    remaining -= static_cast<std::int32_t>(needed);
    const auto current = body.currentByteAt08 >> (remaining & 31);
    body.remainingBitsAt00 = remaining;
    value |= static_cast<std::uint32_t>(current) & ((1u << (needed & 31)) - 1u);
    return static_cast<std::int32_t>(value);
}

std::int32_t ReadTreeAt43E760(ArchiveHuffmanRuntime& body,
    ArchiveCodecInputView extent, IArchiveHuffmanErrorSource& errors,
    IBmrInputReadGuard* guard)
{
    if (ReadBitAt43E640(body, extent, errors, guard) == 0)
        return ReadLeafAt43E7AF(body, extent, errors, guard);
    const auto node = body.nextNodeAt101C;
    body.nextNodeAt101C = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(node) + 1u);
    if (node >= 0x1FF) ReportAndThrow(errors, TreeError, 1);
    if (node < 0)
        throw std::out_of_range("Huffman negative tree index (safe boundary)");
    const auto left = ReadTreeAt43E760(body, extent, errors, guard);
    body.leftAt24[static_cast<std::size_t>(node)] = left;
    const auto right = ReadTreeAt43E760(body, extent, errors, guard);
    body.rightAt820[static_cast<std::size_t>(node)] = right;
    return node;
}
}

ArchiveHuffmanErrorSource::ArchiveHuffmanErrorSource(ILoadSpriteErrorEnvironmentSource& environment,
    decltype(&::MessageBoxA) messageBox) noexcept : environment_(environment), messageBox_(messageBox) {}

void ArchiveHuffmanErrorSource::ReportAt420C50(const char* textCp932)
{
    // 43E67E/43E822/43E84C都传caption=null、uType=10h；420C5A读本次根+10，420C64读同根+2C。
    // 复用的仅是环境字段借用接口，不借用LoadSprite的文本、flags、catch或错误分类。
    const auto environment = environment_.CaptureAt4180C0();
    const char* const caption = environment.CaptionAt10();
    const HWND window = environment.windowAt2C.Read();
    if (!messageBox_) _invalid_parameter_noinfo_noreturn();
    static_cast<void>(messageBox_(window, textCp932, caption, MB_ICONHAND));
}

ArchiveHuffmanRuntime::ArchiveHuffmanRuntime(RawAt43DE72) noexcept : scratchAt18{} {}

ArchiveHuffmanRuntime* GetHuffmanAt43DE24(ArchiveCodecManagerRuntime& manager)
{
    auto& slot = manager.huffmanAt00;
    if (!slot.baseAt00)
    {
        ReleaseYtlReference(slot);
        void* const storage = ::operator new(sizeof(ArchiveHuffmanRuntime));
        auto* const body = storage ? ::new (storage) ArchiveHuffmanRuntime(ArchiveHuffmanRuntime::RawAt43DE72{}) : nullptr;
        // 43DE8B第二次检查只删当前control；新正文没有失败回收资格。
        if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
        slot.biasAt04 = 0;
        slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
        void* const controlStorage = ::operator new(sizeof(HuffmanControl));
        auto* const control = controlStorage ? ::new (controlStorage) HuffmanControl(body) : nullptr;
        slot.controlAt08 = control;
        if (!control) _invalid_parameter_noinfo_noreturn();
        control->referenceCountAt04 = 1;
        // 43DECD/43DED4/43DEDE逐次重读发布槽；不套缓存control字段赋值。
        slot.controlAt08->owningAt08 = 1;
        static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0x1020;
        static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
    }
    return ResolveEnvironmentObjectReference<ArchiveHuffmanRuntime>(slot);
}

void ArchiveHuffmanRuntime::DecodeAt43DE24(std::span<const std::uint8_t> payloadInclLengthDword,
    std::span<std::uint8_t> output, IBmrInputReadGuard* inputGuard,
    IBmrDestinationAccessGuard* destinationGuard, IArchiveHuffmanErrorSource& errors)
{
    DecodeAt43DE24(ArchiveCodecInputView::Bounded(payloadInclLengthDword), output, static_cast<std::uint32_t>(output.size()),
        inputGuard, destinationGuard, errors);
}

void ArchiveHuffmanRuntime::DecodeAt43DE24(std::span<const std::uint8_t> payloadInclLengthDword,
    std::span<std::uint8_t> outputExtent, std::uint32_t outputCount,
    IBmrInputReadGuard* inputGuard, IBmrDestinationAccessGuard* destinationGuard,
    IArchiveHuffmanErrorSource& errors)
{
    DecodeAt43DE24(ArchiveCodecInputView::Bounded(payloadInclLengthDword), outputExtent, outputCount,
        inputGuard, destinationGuard, errors);
}

void ArchiveHuffmanRuntime::DecodeRawAt43DE24(const std::uint8_t* payloadInclLengthDword,
    std::span<std::uint8_t> outputExtent, std::uint32_t outputCount,
    IBmrInputReadGuard* inputGuard, IBmrDestinationAccessGuard* destinationGuard,
    IArchiveHuffmanErrorSource& errors)
{
    DecodeAt43DE24(ArchiveCodecInputView::Raw(payloadInclLengthDword), outputExtent, outputCount,
        inputGuard, destinationGuard, errors);
}

void ArchiveHuffmanRuntime::DecodeAt43DE24(ArchiveCodecInputView input,
    std::span<std::uint8_t> output, IBmrInputReadGuard* inputGuard,
    IBmrDestinationAccessGuard* destinationGuard, IArchiveHuffmanErrorSource& errors)
{
    DecodeAt43DE24(input, output, static_cast<std::uint32_t>(output.size()),
        inputGuard, destinationGuard, errors);
}

void ArchiveHuffmanRuntime::DecodeAt43DE24(ArchiveCodecInputView payloadInclLengthDword,
    std::span<std::uint8_t> outputExtent, std::uint32_t outputCount,
    IBmrInputReadGuard* inputGuard, IBmrDestinationAccessGuard* destinationGuard,
    IArchiveHuffmanErrorSource& errors)
{
    // 43DF08/44851C：owner已经取得，此处才读DWORD；物理不足是额外的有界拒绝。
    ValidateInput(payloadInclLengthDword, payloadInclLengthDword.data(), 4, inputGuard);
    const auto* const payload = payloadInclLengthDword.data();
    inputSizeAt10 = static_cast<std::uint32_t>(payload[0]) |
        (static_cast<std::uint32_t>(payload[1]) << 8) |
        (static_cast<std::uint32_t>(payload[2]) << 16) |
        (static_cast<std::uint32_t>(payload[3]) << 24);
    remainingBitsAt00 = 0;
    bitsPerByteAt04 = 8;
    currentByteAt08 = 0;
    readBytesAt0C = 0;
    inputAt14 = payload + 4;
    nextNodeAt101C = 0x100;
    const auto extent = payloadInclLengthDword.After(4);
    const auto root = ReadTreeAt43E760(*this, extent, errors, inputGuard);
    for (std::uint32_t index = 0; index != outputCount; ++index)
    {
        auto node = root;
        while (node >= 0x100)
        {
            const auto bit = ReadBitAt43E640(*this, extent, errors, inputGuard);
            if (node >= 0x1FF)
                throw std::out_of_range("Huffman output tree index (safe boundary)");
            node = bit ? rightAt820[static_cast<std::size_t>(node)] : leftAt24[static_cast<std::size_t>(node)];
        }
        // 43DFD3之前的建树/逐字符取bit及报告副作用不能被容量检查抢先覆盖。
        if (index >= outputExtent.size())
            throw std::out_of_range("Huffman output exceeds captured extent (safe boundary)");
        if (destinationGuard) destinationGuard->ValidateAccess(outputExtent.subspan(index, 1));
        outputExtent[index] = static_cast<std::uint8_t>(node);
    }
}
}
