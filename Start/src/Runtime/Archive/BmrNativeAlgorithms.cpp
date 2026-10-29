#include <Runtime/BmrNativeAlgorithms.h>
#include <Runtime/BmrDecoderRuntime.h>
#include <Runtime/ByteArrayReferenceRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/ParamsRawBlobInstaller.h>

#include <array>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>

namespace KaGuYa::Start::Runtime::BmrNativeAlgorithms
{
namespace
{
void ReleaseCached(YtlReferenceControlRuntime* control) noexcept
{
    // 43E5CC/43E5EF及43E25F已转到terminate态，不能异常后再释放一次当前槽。
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
}

class ByteTemporary final
{
public:
    ByteTemporary() noexcept { ::new (&bytes) ByteArrayReferenceRuntime; }
    ~ByteTemporary() noexcept { if (active_) bytes.~ByteArrayReferenceRuntime(); }
    void ReleaseNormal(YtlReferenceControlRuntime* cached) noexcept
    {
        active_ = false;
        ReleaseCached(cached);
    }
    // 原局部Ref没有第二次自动析构；显式union控制正常/展开两条释放路径。
    union { ByteArrayReferenceRuntime bytes; };
private:
    bool active_ = true;
};

class RetiredUlongControl final : public EnvironmentObjectReferenceControl
{
public:
    RetiredUlongControl() noexcept : EnvironmentObjectReferenceControl(nullptr, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
};

class UlongControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit UlongControl(void* data) noexcept
        : EnvironmentObjectReferenceControl(data, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 43E880只有释放数组正文，没有逐元素析构或count乘法。
        ::operator delete[](payloadAt14);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 43DC30退役至5ADD18；首次18h，退役后再次删除14h。
        std::array<std::byte, 20> fields;
        std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(this) + 4, fields.size());
        auto* retired = ::new (static_cast<void*>(this)) RetiredUlongControl;
        std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
        if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
    }
};
static_assert(sizeof(UlongControl) == 0x18 && sizeof(RetiredUlongControl) == 0x18);

struct UlongTemporary final
{
    YtlReferenceSlotRuntime slot{};
    bool active = true;
    ~UlongTemporary() noexcept { if (active) ReleaseYtlReference(slot); }
    void Allocate(std::uint32_t count)
    {
        if (std::bit_cast<std::int32_t>(count) <= 0) return;
        const auto size = count > (std::numeric_limits<std::uint32_t>::max)() / 4u
            ? (std::numeric_limits<std::uint32_t>::max)() : count * 4u;
        void* data = ::operator new[](size);
        slot.biasAt04 = 0;
        slot.baseAt00 = reinterpret_cast<std::uintptr_t>(data);
        void* storage = ::operator new(sizeof(UlongControl));
        auto* control = storage ? ::new (storage) UlongControl(data) : nullptr;
        // 43E457在发布control前空写，不能把null当有效空数组。
        if (!control) _invalid_parameter_noinfo_noreturn();
        control->referenceCountAt04 = 1;
        control->owningAt08 = 1;
        control->strideAt0C = 4;
        slot.controlAt08 = control;
        control->countAt10 = count;
    }
    void ReleaseNormal(YtlReferenceControlRuntime* cached) noexcept
    {
        active = false;
        ReleaseCached(cached);
    }
};

[[noreturn]] void InvalidBwtAccess()
{
    throw ParamsRawBlobInstallException(ParamsRawBlobInstallError::InvalidBwtKey,
        "BMR BWT access exceeds its supplied storage (safe boundary)");
}

const BmrHeaderRuntime& CurrentHeader(BmrDecoderRuntime& decoder)
{
    const auto* header = ResolveEnvironmentObjectReference<BmrHeaderRuntime>(decoder.HeaderReferenceAt04());
    if (!header)
        throw ParamsRawBlobInstallException(ParamsRawBlobInstallError::MissingDecoderHeader,
            "BMR decoder has no current header (safe boundary)");
    return *header;
}
}

void BurrowsWheelerAt43E320(std::span<std::uint8_t> bytes, std::uint32_t key,
    IBmrDestinationAccessGuard* destinationGuard)
{
    const auto count = static_cast<std::uint32_t>(bytes.size());
    std::array<std::uint32_t, 256> frequencies{};
    if (destinationGuard) destinationGuard->ValidateAccess(bytes);
    for (const auto value : bytes) ++frequencies[value];
    for (std::size_t index = 1; index != frequencies.size(); ++index)
        frequencies[index] += frequencies[index - 1];

    // 43E407先建立state0，control分配失败时无control正文不额外回收。
    UlongTemporary distribution;
    distribution.Allocate(count);
    const auto base = distribution.slot.baseAt00;
    auto* const cached = static_cast<EnvironmentObjectReferenceControl*>(distribution.slot.controlAt08);
    const auto at = [&](std::uint32_t index) -> std::uint32_t&
    {
        const auto offset = static_cast<std::uint64_t>(index) * (cached ? cached->strideAt0C : 0u);
        if (!base || index >= count || offset + 4u > static_cast<std::uint64_t>(count) * 4u ||
            (offset & 3u) != 0) InvalidBwtAccess();
        return *reinterpret_cast<std::uint32_t*>(base + static_cast<std::uintptr_t>(offset));
    };
    if (destinationGuard) destinationGuard->ValidateAccess(bytes);
    for (std::uint32_t index = count; index > 0; --index)
        at(--frequencies[bytes[index - 1]]) = index - 1;

    // 43E4DA在412700前建立byte临时资格；坏key检查也只放在原读取位置。
    ByteTemporary temporary;
    temporary.bytes.AllocateAt412700(count);
    std::uint32_t position = at(key);
    auto* const cachedBytes = temporary.bytes.RawSlot().controlAt08;
    if (destinationGuard) destinationGuard->ValidateAccess(bytes);
    for (std::uint32_t index = 0; index != count; ++index)
    {
        if (position >= count) InvalidBwtAccess();
        auto* output = temporary.bytes.DataAt(index);
        if (!output) InvalidBwtAccess();
        *output = bytes[position];
        position = at(position);
    }
    if (destinationGuard) destinationGuard->ValidateAccess(bytes);
    std::memmove(bytes.data(), temporary.bytes.Data(), count);
    temporary.ReleaseNormal(cachedBytes);
    distribution.ReleaseNormal(cached);
}

void RunLengthAt43E17D(std::span<const std::uint8_t> input, std::span<std::uint8_t> destination,
    std::uint32_t finalSize, std::uint8_t step, IBmrDestinationAccessGuard* destinationGuard)
{
    std::size_t source = 0;
    const auto read = [&]()
    {
        if (source >= input.size())
            throw ParamsRawBlobInstallException(ParamsRawBlobInstallError::RleStreamExhausted,
                "BMR RLE read exceeds its supplied storage (safe boundary)");
        return input[source++];
    };
    const auto write = [&](std::uint32_t offset, std::uint8_t value)
    {
        if (offset >= destination.size())
            throw ParamsRawBlobInstallException(ParamsRawBlobInstallError::RleDestinationOutOfRange,
                "BMR RLE write exceeds its supplied storage (safe boundary)");
        auto output = destination.subspan(offset, 1);
        if (destinationGuard) destinationGuard->ValidateAccess(output);
        output[0] = value;
    };
    for (std::uint32_t lane = 0; lane < step; ++lane)
    {
        auto previous = read();
        write(lane, previous); // 43E1AA先写首值；step大于finalSize也不会跳过lane。
        std::uint32_t offset = lane + step;
        while (offset < finalSize)
        {
            const auto current = read();
            write(offset, current);
            offset += step;
            if (current != previous)
            {
                previous = current;
                continue;
            }
            std::uint32_t count = read();
            if (count & 0x80u) count = ((count & 0x7Fu) << 8u) + 128u + read();
            // 43E200..43E208不与finalSize比较；只在真实越界写处拒绝。
            while (count-- != 0)
            {
                write(offset, current);
                offset += step;
            }
            if (offset < finalSize)
            {
                previous = read();
                write(offset, previous);
                offset += step;
            }
        }
    }
}

void FinishAt43DFF5(BmrDecoderRuntime& decoder, std::span<std::uint8_t> destination,
    std::uint32_t intermediateCount, std::span<const std::uint8_t, 256> initialDictionary,
    IBmrDestinationAccessGuard* destinationGuard)
{
    if (intermediateCount > destination.size())
        throw ParamsRawBlobInstallException(ParamsRawBlobInstallError::InvalidSizeRelationship,
            "BMR intermediate output exceeds the supplied destination span");
    const auto intermediate = destination.first(intermediateCount);
    std::array<std::uint8_t, 256> dictionary;
    std::memcpy(dictionary.data(), initialDictionary.data(), dictionary.size());
    if (destinationGuard) destinationGuard->ValidateAccess(intermediate);
    for (auto& value : intermediate)
    {
        const auto index = value;
        value = dictionary[index];
        for (std::size_t at = index; at > 0; --at) dictionary[at] = dictionary[at - 1];
        dictionary[0] = value;
    }
    const auto key = CurrentHeader(decoder).bwtKeyAt08;
    BurrowsWheelerAt43E320(intermediate, key, destinationGuard);
    if (CurrentHeader(decoder).stepAt03 != 0)
    {
        ByteTemporary encoded;
        encoded.bytes.AllocateAt412700(intermediateCount);
        if (destinationGuard) destinationGuard->ValidateAccess(intermediate);
        if (intermediateCount) std::memmove(encoded.bytes.Data(), intermediate.data(), intermediateCount);
        // 43E103..43E166在分配和memmove之后才读取step/final，不能沿用入口Header。
        const auto step = CurrentHeader(decoder).stepAt03;
        const auto finalSize = CurrentHeader(decoder).finalSizeAt04;
        if (step != 0)
            RunLengthAt43E17D(encoded.bytes.Bytes(), destination, finalSize, step, destinationGuard);
        encoded.ReleaseNormal(encoded.bytes.RawSlot().controlAt08);
    }
    // 43E292仅正常尾声清Header；任何算法/分配失败均保留它供后续调用。
    decoder.ResetHeader();
}
}
