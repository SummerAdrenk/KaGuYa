#include <Runtime/ArchiveLzssRuntime.h>
#include <Runtime/ArchiveCodecManagerRuntime.h>
#include <Runtime/BmrDecoderRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>

#include <array>
#include <cstdlib>
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
class RetiredLzssControl final : public EnvironmentObjectReferenceControl
{
public:
    RetiredLzssControl() noexcept
        : EnvironmentObjectReferenceControl(nullptr, UninitializedFieldsTag{}) {}

    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 5ADD18.slot04=412460：flags0退休后的再次删除使用14h。
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
};

class LzssControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit LzssControl(ArchiveLzssRuntime* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}

    void DestroyPayloadAtVslot00() override
    {
        // 448A20缓存+14并调用正文唯一虚槽(1)，本函数没有额外terminate保护。
        auto* const body = static_cast<ArchiveLzssRuntime*>(payloadAt14);
        if (body) body->DeleteAtVslot00(1);
    }

    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 444E20只退虚表；+04..+17包括未写字节全部保留。
        std::array<std::byte, 0x14> fields;
        std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(this) + 4, fields.size());
        auto* retired = ::new (static_cast<void*>(this)) RetiredLzssControl;
        std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
        if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
    }
};
static_assert(sizeof(LzssControl) == 0x18 && sizeof(RetiredLzssControl) == 0x18);
}

const char* ArchiveLzssRuntimeException::what() const noexcept
{
    switch (error_)
    {
    case ArchiveLzssRuntimeError::InputTruncated: return "LZSS input exceeds the supplied span";
    case ArchiveLzssRuntimeError::DestinationOverflow: return "LZSS output exceeds the supplied span";
    }
    return "LZSS bounded access failure";
}

void* ArchiveLzssRuntime::DeleteAtVslot00(std::uint32_t flags)
{
    // 444E50的flags0也写回CLzss自身虚表，不是control的purecall退休表。
    void* const address = this;
    ::new (address) ArchiveLzssRuntime;
    if (flags & 1u) ::operator delete(address, std::size_t{0x1010});
    return address;
}

void ArchiveLzssRuntime::DecodeAt445750(std::span<const std::uint8_t> input,
    std::span<std::uint8_t> destination, IBmrInputReadGuard* inputGuard,
    IBmrDestinationAccessGuard* destinationGuard)
{
    DecodeAt445750(ArchiveCodecInputView::Bounded(input), destination, inputGuard, destinationGuard);
}

void ArchiveLzssRuntime::DecodeRawAt445750(const std::uint8_t* input,
    std::span<std::uint8_t> destination, IBmrInputReadGuard* inputGuard,
    IBmrDestinationAccessGuard* destinationGuard)
{
    DecodeAt445750(ArchiveCodecInputView::Raw(input), destination, inputGuard, destinationGuard);
}

void ArchiveLzssRuntime::DecodeAt445750(ArchiveCodecInputView input,
    std::span<std::uint8_t> destination, IBmrInputReadGuard* inputGuard,
    IBmrDestinationAccessGuard* destinationGuard)
{
    // 445763/445766/44576D：每次只重置这三个字段；窗口写入位置是从1开始的局部量。
    inputAt04 = input.data();
    currentAt0C = 0;
    maskAt08 = 0x80;
    std::size_t output = 0;
    std::size_t position = 1;

    const auto readBit = [&]() -> bool
    {
        const auto mask = maskAt08;
        if (mask == 0x80)
        {
            if (!input.Contains(inputAt04, 1))
                throw ArchiveLzssRuntimeException(ArchiveLzssRuntimeError::InputTruncated);
            const auto* const source = inputAt04;
            if (inputGuard) inputGuard->ValidateRead({source, 1});
            currentAt0C = *source;
            inputAt04 = source + 1;
        }
        const bool result = (currentAt0C & mask) != 0;
        maskAt08 = static_cast<std::uint8_t>(mask >> 1u);
        if (!maskAt08) maskAt08 = 0x80;
        return result;
    };
    const auto readBits = [&](unsigned count) -> std::uint32_t
    {
        std::uint32_t result = 0;
        for (unsigned index = 0; index < count; ++index)
            result = (result << 1u) | (readBit() ? 1u : 0u);
        return result;
    };
    const auto writeByte = [&](std::uint8_t value)
    {
        if (output == destination.size())
            throw ArchiveLzssRuntimeException(ArchiveLzssRuntimeError::DestinationOverflow);
        auto* const target = destination.data() + output;
        if (destinationGuard) destinationGuard->ValidateAccess({target, 1});
        // 4457F8/4457FB、4458E5/4458E8：先写固定输出，再写真实环形窗口。
        *target = value;
        ++output;
        windowAt10[position] = value;
        position = (position + 1) & 0xFFFu;
    };

    for (;;)
    {
        if (readBit())
        {
            writeByte(static_cast<std::uint8_t>(readBits(8)));
            continue;
        }
        const auto offset = readBits(12);
        // 44586B..44586D：零位置直接返回，不读取长度，不检查输出是否填满。
        if (!offset) return;
        const auto count = readBits(4) + 2u;
        for (std::uint32_t index = 0; index < count; ++index)
        {
            const auto source = (offset + index) & 0xFFFu;
            // 4458E1逐字节晚读，允许match读到本次刚写入的重叠窗口。
            writeByte(windowAt10[source]);
        }
    }
}

ArchiveLzssRuntime* GetLzssAt4840B0(ArchiveCodecManagerRuntime& manager)
{
    auto& slot = manager.lzssAt18;
    if (!slot.baseAt00)
    {
        // 4840BA..4840E6：payload之后重取当前control，成功才清base/control；bias保留。
        ReleaseYtlReference(slot);
        void* const storage = ::operator new(sizeof(ArchiveLzssRuntime));
        auto* const body = storage ? ::new (storage) ArchiveLzssRuntime : nullptr;

        // 分配回调可能改了control。原第二次检查只删control，没有减持或正文回滚。
        if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
        slot.biasAt04 = 0;
        slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
        void* const controlStorage = ::operator new(sizeof(LzssControl));
        auto* const control = controlStorage ? ::new (controlStorage) LzssControl(body) : nullptr;
        slot.controlAt08 = control;
        // 484143在发布后向分配结果写refs；原空写在现代域显式终止，不变成成功空Ref。
        if (!control) _invalid_parameter_noinfo_noreturn();
        control->referenceCountAt04 = 1;
        static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->owningAt08 = 1;
        static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0x1010;
        static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
    }
    return ResolveEnvironmentObjectReference<ArchiveLzssRuntime>(slot);
}
}
