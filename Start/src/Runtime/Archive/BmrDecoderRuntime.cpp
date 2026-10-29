#include <Runtime/BmrDecoderRuntime.h>
#include <Runtime/ArchiveCodecManagerRuntime.h>
#include <Runtime/ArchiveHuffmanRuntime.h>
#include <Runtime/BmrNativeAlgorithms.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/ParamsRawBlobInstaller.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <new>
#include <stdexcept>
#include <vector>

namespace KaGuYa::Start::Runtime
{
namespace
{
class RetiredBmrObjectControl final : public EnvironmentObjectReferenceControl
{
public:
    RetiredBmrObjectControl() noexcept : EnvironmentObjectReferenceControl(nullptr, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 5ADD18 -> 412460：退休后按14h删除，不再使用原派生18h尺寸。
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
};

void RetireBmrObjectControl(EnvironmentObjectReferenceControl* control, std::uint32_t flags)
{
    // 444D30/444DC0共享5ADD18退休身份；除vptr之外二十字节全部保留。
    std::array<std::byte, 20> fields;
    std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(control) + 4, fields.size());
    auto* retired = ::new (static_cast<void*>(control)) RetiredBmrObjectControl;
    std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
    if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
}

class HeaderControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit HeaderControl(void* body) noexcept : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 5B210C.slot00=448900：非数组Header正文，不能调用delete[]。
        ::operator delete(payloadAt14, std::size_t{0x10});
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        RetireBmrObjectControl(this, flags);
    }
};

class DecoderControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit DecoderControl(void* body) noexcept : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() noexcept override
    {
        // 448990的state0同时覆盖内部Header释放和14h正文删除。
        auto* body = static_cast<BmrDecoderRuntime*>(payloadAt14);
        if (body)
        {
            body->~BmrDecoderRuntime();
            ::operator delete(body, std::size_t{0x14});
        }
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireBmrObjectControl(this, flags); }
};
static_assert(sizeof(HeaderControl) == 0x18 && sizeof(DecoderControl) == 0x18 && sizeof(RetiredBmrObjectControl) == 0x18);

struct HeaderTemporary final
{
    YtlReferenceSlotRuntime slot;
    bool active = true;
    ~HeaderTemporary() noexcept
    {
        // 595480 -> 444950整体flags5，展开按当前slot释放。
        if (active) ReleaseYtlReference(slot);
    }
    void ReleaseNormal(YtlReferenceControlRuntime* cached) noexcept
    {
        // 445B4A state1：先撤局部资格，正常减持/两个虚槽均使用缓存ESI。
        active = false;
        if (cached && --cached->referenceCountAt04 == 0)
        {
            if (cached->owningAt08) cached->DestroyPayloadAtVslot00();
            cached->DeleteControlAtVslot04(1);
        }
    }
};

const BmrHeaderRuntime& Header(const YtlReferenceSlotRuntime& slot)
{
    const auto* body = ResolveEnvironmentObjectReference<BmrHeaderRuntime>(slot);
    if (!body)
        throw ParamsRawBlobInstallException(ParamsRawBlobInstallError::MissingDecoderHeader,
            "BMR decoder has no current header (safe boundary)");
    return *body;
}

std::uint32_t ReadDword(std::span<const std::uint8_t> bytes) noexcept
{
    return static_cast<std::uint32_t>(bytes[0]) |
        (static_cast<std::uint32_t>(bytes[1]) << 8u) |
        (static_cast<std::uint32_t>(bytes[2]) << 16u) |
        (static_cast<std::uint32_t>(bytes[3]) << 24u);
}

void ReleaseBeforeReplacement(YtlReferenceSlotRuntime& slot)
{
    // 445ACF..445AF1只解除旧引用，尚不清字段；回调结束才由调用者发布新Ref。
    auto* control = slot.controlAt08;
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control = slot.controlAt08;
        if (control) control->DeleteControlAtVslot04(1);
    }
}
}

BmrDecoderRuntime::BmrDecoderRuntime() noexcept : magicAt00_{'B', 'M', 'R'}, headerAt04_{} {}

BmrDecoderRuntime::~BmrDecoderRuntime() noexcept
{
    // 448990 state0终止保护Header销毁；不把同样边界施加给正常ResetHeader。
    ResetHeader();
}

void BmrDecoderRuntime::ResetHeader()
{
    // 43E292..43E2BE：回调后重读control，成功后清base/control并保留bias。
    ReleaseYtlReference(headerAt04_);
}

const std::uint8_t* BmrDecoderRuntime::IdentifyAt445A00(
    std::span<const std::uint8_t> source, std::uint32_t& outSize, IBmrInputReadGuard* inputGuard)
{
    return IdentifyAt445A00(ArchiveCodecInputView::Bounded(source), outSize, inputGuard);
}

const std::uint8_t* BmrDecoderRuntime::IdentifyRawAt445A00(
    const std::uint8_t* source, std::uint32_t& outSize, IBmrInputReadGuard* inputGuard)
{
    return IdentifyAt445A00(ArchiveCodecInputView::Raw(source), outSize, inputGuard);
}

const std::uint8_t* BmrDecoderRuntime::IdentifyAt445A00(
    ArchiveCodecInputView source, std::uint32_t& outSize, IBmrInputReadGuard* inputGuard)
{
    void* const body = ::operator new(sizeof(BmrHeaderRuntime));
    HeaderControl* control = nullptr;
    YtlReferenceSlotRuntime local{};
    if (body)
    {
        // 445A50的control分配先于state0；失败不回收已经创建的裸Header。
        local.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
        void* const storage = ::operator new(sizeof(HeaderControl));
        control = storage ? ::new (storage) HeaderControl(body) : nullptr;
        // 原445A77归零后在445A79空写；这里在同一资格边界显式failfast。
        if (!control) _invalid_parameter_noinfo_noreturn();
        control->referenceCountAt04 = 1;
        control->owningAt08 = 1;
        control->strideAt0C = 0x10;
        local.controlAt08 = control;
        control->countAt10 = 1;
    }
    HeaderTemporary temporary{local};
    // 445A9D才建立清理态；有界检查位于原16字节读取处，不依赖new的padding。
    if (!source.Contains(source.data(), sizeof(BmrHeaderRuntime)))
        throw ParamsRawBlobInstallException(ParamsRawBlobInstallError::HeaderTruncated,
            "BMR identification requires sixteen readable bytes (safe boundary)");
    if (inputGuard) inputGuard->ValidateRead({source.data(), sizeof(BmrHeaderRuntime)});
    // 445A4E的null分支跳过control，原随后向null写16字节；不把它改成bad_alloc或miss。
    if (!body) _invalid_parameter_noinfo_noreturn();
    std::memcpy(body, source.data(), sizeof(BmrHeaderRuntime));
    const auto* const copied = static_cast<const BmrHeaderRuntime*>(body);
    const std::uint8_t* result = nullptr;
    if (copied->magicAt00 == magicAt00_)
    {
        ReleaseBeforeReplacement(headerAt04_);
        headerAt04_.baseAt00 = temporary.slot.baseAt00;
        headerAt04_.biasAt04 = 0;
        headerAt04_.controlAt08 = control;
        if (control) ++control->referenceCountAt04;
        outSize = Header(headerAt04_).finalSizeAt04;
        result = source.data() + sizeof(BmrHeaderRuntime);
    }
    temporary.ReleaseNormal(control);
    return result;
}

void BmrDecoderRuntime::DecodeAt43DCF0(std::span<const std::uint8_t> payload,
    std::span<std::uint8_t> destination, IBmrInputReadGuard* inputGuard,
    IBmrDestinationAccessGuard* destinationGuard)
{
    // 43DD50仅捕获一次N；后面的Header字段不能合并成入口快照。
    const auto count = Header(headerAt04_).intermediateSizeAt0C;
    if (payload.size() < 4)
        throw ParamsRawBlobInstallException(ParamsRawBlobInstallError::HeaderTruncated,
            "BMR payload is missing its Huffman byte count");
    if (inputGuard) inputGuard->ValidateRead(payload.first(4));
    const auto encodedCount = ReadDword(payload);
    if (encodedCount > payload.size() - 4)
        throw ParamsRawBlobInstallException(ParamsRawBlobInstallError::HuffmanStreamOutOfRange,
            "BMR Huffman stream exceeds the supplied payload span");
    if (count > destination.size())
        throw ParamsRawBlobInstallException(ParamsRawBlobInstallError::InvalidSizeRelationship,
            "BMR intermediate output exceeds the supplied destination span");
    const auto intermediate = destination.first(count);
    BmrDecoderAlgorithms::DecodeHuffman(payload.subspan(4, encodedCount), intermediate, inputGuard, destinationGuard);
    if (destinationGuard) destinationGuard->ValidateAccess(intermediate);
    BmrDecoderAlgorithms::MoveToFront(intermediate);
    const auto key = Header(headerAt04_).bwtKeyAt08;
    BmrDecoderAlgorithms::BurrowsWheeler(intermediate, key, destinationGuard);
    if (Header(headerAt04_).stepAt03 != 0)
    {
        // 暂用现代vector承载RLE临时输入；不声称复现原ByteRef分配/释放EH。
        // 不能用iterator构造：分配回调可能已使旧target失效，须在读取它之前重新验证。
        std::vector<std::uint8_t> encoded(intermediate.size());
        if (destinationGuard) destinationGuard->ValidateAccess(intermediate);
        std::copy(intermediate.begin(), intermediate.end(), encoded.begin());
        const auto step = Header(headerAt04_).stepAt03;
        const auto finalSize = Header(headerAt04_).finalSizeAt04;
        if (step != 0)
        {
            if (finalSize > destination.size())
                throw ParamsRawBlobInstallException(ParamsRawBlobInstallError::DestinationSizeMismatch,
                    "BMR final output exceeds the supplied destination span");
            const auto finalOutput = destination.first(finalSize);
            if (destinationGuard) destinationGuard->ValidateAccess(finalOutput);
            BmrDecoderAlgorithms::RunLength(encoded, finalOutput, step);
        }
    }
    // 不放在RAII guard中：中途失败保留Header，正常尾声释放本身也允许抛出。
    ResetHeader();
}

void BmrDecoderRuntime::DecodeAt43DCF0(std::span<const std::uint8_t> payload,
    std::span<std::uint8_t> destination, IArchiveCodecManagerSource& codecs,
    IArchiveHuffmanErrorSource& errors, IBmrInputReadGuard* inputGuard,
    IBmrDestinationAccessGuard* destinationGuard)
{
    DecodeAt43DCF0(ArchiveCodecInputView::Bounded(payload), destination, codecs, errors,
        inputGuard, destinationGuard);
}

void BmrDecoderRuntime::DecodeRawAt43DCF0(const std::uint8_t* payload,
    std::span<std::uint8_t> destination, IArchiveCodecManagerSource& codecs,
    IArchiveHuffmanErrorSource& errors, IBmrInputReadGuard* inputGuard,
    IBmrDestinationAccessGuard* destinationGuard)
{
    DecodeAt43DCF0(ArchiveCodecInputView::Raw(payload), destination, codecs, errors,
        inputGuard, destinationGuard);
}

void BmrDecoderRuntime::DecodeAt43DCF0(ArchiveCodecInputView payload,
    std::span<std::uint8_t> destination, IArchiveCodecManagerSource& codecs,
    IArchiveHuffmanErrorSource& errors, IBmrInputReadGuard* inputGuard,
    IBmrDestinationAccessGuard* destinationGuard)
{
    // 43DD50读N在445910之前；getter回调换Header不改变本次输出N。
    const auto count = Header(headerAt04_).intermediateSizeAt0C;
    auto* const manager = codecs.CaptureAt445910();
    if (!manager) throw std::logic_error("BMR codec manager is null (safe boundary)");
    auto* const huffman = GetHuffmanAt43DE24(*manager);
    if (!huffman) throw std::logic_error("BMR Huffman body is null (safe boundary)");
    // 建树/原错误报告先于任何目标写入；容量检查交给实际输出点，不能提前覆盖int1/int2。
    huffman->DecodeAt43DE24(payload, destination, count, inputGuard, destinationGuard, errors);
    BmrNativeAlgorithms::FinishAt43DFF5(*this, destination, count,
        ProcessArchiveMtfDictionary(), destinationGuard);
}

BmrDecoderRuntimeOwner::~BmrDecoderRuntimeOwner() noexcept
{
    ReleaseYtlReference(decoder_);
}

BmrDecoderRuntime* GetBmrAt445110(YtlReferenceSlotRuntime& slot)
{
    if (!slot.baseAt00)
    {
        ReleaseYtlReference(slot);
        void* const storage = ::operator new(sizeof(BmrDecoderRuntime));
        auto* const created = storage ? ::new (storage) BmrDecoderRuntime : nullptr;
        // 44518A重查回调可能放入的control：这里只删control，不再减持或删正文。
        if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
        slot.biasAt04 = 0;
        slot.baseAt00 = reinterpret_cast<std::uintptr_t>(created);
        // 4451A1已发布base；分配失败保留该半发布状态，没有裸正文清理资格。
        void* const controlStorage = ::operator new(sizeof(DecoderControl));
        auto* const control = controlStorage ? ::new (controlStorage) DecoderControl(created) : nullptr;
        slot.controlAt08 = control;
        // 4451BB发布null后原4451C0空写；安全边界不回收已发布正文。
        if (!control) _invalid_parameter_noinfo_noreturn();
        control->referenceCountAt04 = 1;
        // 4451C7/4451CF/4451D9分别重读当前control，不套用别的factory的写入缓存。
        slot.controlAt08->owningAt08 = 1;
        static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0x14;
        static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
    }
    return ResolveEnvironmentObjectReference<BmrDecoderRuntime>(slot);
}

BmrDecoderRuntime& BmrDecoderRuntimeOwner::GetOrCreateAt445110()
{
    auto* const body = GetBmrAt445110(decoder_);
    // 返回引用的现代接口不能表示原null返回；不改绑来源，也不补建已被回调清空的base。
    if (!body) _invalid_parameter_noinfo_noreturn();
    return *body;
}

BmrDecoderRuntime* GetBmrAt445110(ArchiveCodecManagerRuntime& manager)
{
    return GetBmrAt445110(manager.bmrAt0C);
}
}
