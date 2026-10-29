#include <Runtime/SecretCgNativeArchive.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/ParamsThumbnailCatalogRuntime.h>

#include <bit>
#include <cstdlib>
#include <cstring>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
ISecretCgParamsRootSource* processParams = nullptr;
class BoundParamsSource final : public ISecretCgParamsRootSource
{
public:
    const void* CaptureAt418480(std::uint32_t site) override
    {
        if (!processParams) throw std::logic_error("477FD0: Params source is not bound by the host");
        return processParams->CaptureAt418480(site);
    }
};

template<class T> T& Required(T* value) noexcept
{
    // 原空Ref解引用为硬故障，不允许构造catch把损坏对象伪装成空表。
    if (!value) _invalid_parameter_noinfo_noreturn();
    return *value;
}

std::uintptr_t Resolve(const YtlReferenceSlotRuntime& slot, std::uint32_t index = 0) noexcept
{
    if (!slot.baseAt00) return 0;
    const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(slot.controlAt08);
    return slot.baseAt00 + (slot.biasAt04 + index) * (control ? control->strideAt0C : 0u);
}

std::int32_t SpanCount(const void* begin, const void* end, std::uint32_t stride) noexcept
{
    const auto bytes = reinterpret_cast<std::uintptr_t>(end) - reinterpret_cast<std::uintptr_t>(begin);
    return std::bit_cast<std::int32_t>(bytes) / static_cast<std::int32_t>(stride);
}

std::int32_t IndexCount(const YtlReferenceControlRuntime* value) noexcept
{
    const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(value);
    return control ? std::bit_cast<std::int32_t>(control->countAt10) : 0;
}

std::uint32_t Index(const YtlReferenceSlotRuntime& reference, std::uint32_t index) noexcept
{
    return Required(reinterpret_cast<const std::uint32_t*>(Resolve(reference, index)));
}

SecretCgFlagRecord& Record(const SecretCgReferenceVector& records, std::uint32_t index) noexcept
{
    // 原47A0E0仅按DWORD*12寻址，不增加count/ready检查或归一化。
    auto* slot = reinterpret_cast<const YtlReferenceSlotRuntime*>(
        reinterpret_cast<std::uintptr_t>(records.begin) + index * 12u);
    return Required(ResolveSecretCgFlag(Required(slot)));
}

std::int32_t Percentage(std::uint32_t enabled, std::uint32_t total) noexcept
{
    const auto numerator = std::bit_cast<std::int32_t>(enabled * 100u);
    const auto denominator = total ? std::bit_cast<std::int32_t>(total) : 1;
    const auto result = numerator / denominator;
    return result == 0 && enabled ? 1 : result;
}

void ReleaseCached(YtlReferenceControlRuntime* control) noexcept
{
    // 4781B6/4781DD均在terminate态；payload回调后仍释放同一cached control。
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
}

class ItemUnwind final
{
public:
    explicit ItemUnwind(YtlReferenceSlotRuntime& reference) noexcept : reference_(&reference) {}
    ~ItemUnwind() noexcept { if (reference_) ReleaseYtlReference(*reference_); }
    void Dismiss() noexcept { reference_ = nullptr; }
private:
    YtlReferenceSlotRuntime* reference_;
};

template<class Function> Function Slot(const SecretCgNativeArchive* self, unsigned index) noexcept
{
    return reinterpret_cast<Function>((*reinterpret_cast<void* const* const*>(self))[index]);
}
using CountFunction = std::int32_t(__thiscall*)(const SecretCgNativeArchive*);
using GetFunction = std::uint8_t(__thiscall*)(const SecretCgNativeArchive*, std::uint32_t);
using SetFunction = void(__thiscall*)(SecretCgNativeArchive*, std::uint32_t, std::uint8_t);

const ParamsGameSystem& CaptureGameSystem(ISecretCgParamsRootSource& source, std::uint32_t site)
{
    const auto& root = Required(static_cast<const ParamsRuntimeRoot*>(source.CaptureAt418480(site)));
    return Required(root.GameSystem().get());
}
}

const void* SecretCgParamsRootSource::CaptureAt418480(std::uint32_t)
{
    return owner_.CaptureAt418480();
}

const void* CaptureBoundSecretCgParamsAt418480(std::uint32_t site)
{BoundParamsSource source;return source.CaptureAt418480(site);}

SecretCgParamsBinding::SecretCgParamsBinding(ISecretCgParamsRootSource& source) noexcept
    : previous_(processParams) { processParams = &source; }
SecretCgParamsBinding::~SecretCgParamsBinding() noexcept { processParams = previous_; }

SecretCgNativeArchive::SecretCgNativeArchive(ISecretCgParamsRootSource& source,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    ConstructSecretCgTablesAt45BCA0(readyAt04, baseKeyOffsetAt08,
        recordsAt0C, mappingsAt18, source, strings, memory);
}

SecretCgNativeArchive::~SecretCgNativeArchive() noexcept
{
    auto& memory = DefaultEnvironmentRootMemory();
    DestroySecretCgMappingVectorAt461500(mappingsAt18, memory);
    DestroySecretCgReferencesAt461440(recordsAt0C, memory);
}

void* SecretCgNativeArchive::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    this->~SecretCgNativeArchive();
    if (flags & 1u) ::operator delete(this, OriginalObjectSize);
    return this;
}

std::int32_t SecretCgNativeArchive::CountAtVslot08() const noexcept
{ return SpanCount(recordsAt0C.begin, recordsAt0C.end, 12); }
std::uint8_t SecretCgNativeArchive::GetFlagAtVslot0C(std::uint32_t index) const noexcept
{ return Record(recordsAt0C, index).byteFlagAt04; }
void SecretCgNativeArchive::SetFlagAtVslot10(std::uint32_t index, std::uint8_t value) noexcept
{ Record(recordsAt0C, index).byteFlagAt04 = value; }
void SecretCgNativeArchive::SetKeyBaseAtVslot14(std::uint32_t value) noexcept
{ baseKeyOffsetAt08 = value; }
void SecretCgNativeArchive::FillFlagsAtVslot18(std::uint8_t value) noexcept
{
    for (auto* slot = recordsAt0C.begin; slot != recordsAt0C.end; ++slot)
        Required(ResolveSecretCgFlag(*slot)).byteFlagAt04 = value;
}

std::int32_t SecretCgNativeArchive::PercentAllAtVslot20() const noexcept
{
    const auto* cursor = recordsAt0C.begin;
    const auto count = SpanCount(cursor, recordsAt0C.end, 12);
    std::uint32_t enabled = 0;
    for (std::int32_t index = 0; index < count; ++index, ++cursor)
        if (Required(ResolveSecretCgFlag(*cursor)).byteFlagAt04) ++enabled;
    return Percentage(enabled, static_cast<std::uint32_t>(count));
}

std::int32_t SecretCgNativeArchive::PercentCategoryAtVslot24(std::int32_t category) const noexcept
{
    std::uint32_t enabled = 0, total = 0;
    const auto* begin = mappingsAt18.begin;
    if (category < SpanCount(begin, mappingsAt18.end, 16))
    {
        const auto* mapping = reinterpret_cast<const SecretCgMappingRuntime*>(
            reinterpret_cast<std::uintptr_t>(begin) + static_cast<std::uint32_t>(category) * 16u);
        const auto* end = Required(mapping).referencesAt04.end;
        for (auto* item = mapping->referencesAt04.begin; item != end; ++item)
        {
            const auto count = IndexCount(item->controlAt08);
            total += static_cast<std::uint32_t>(count);
            for (std::int32_t index = 0; index < count; ++index)
                if (Record(recordsAt0C, Index(*item, index)).byteFlagAt04) ++enabled;
        }
    }
    return Percentage(enabled, total);
}

const char* const* SecretCgNativeArchive::ThumbnailAtVslot1C(std::int32_t category, std::int32_t item)
{
    // 未命中/未解锁时原函数根本不取params；不得提前要求全局来源。
    BoundParamsSource source;
    return ThumbnailAt477FD0(source, category, item);
}

std::size_t SecretCgIndicesView::size() const noexcept
{ return static_cast<std::uint32_t>(IndexCount(reference_->controlAt08)); }
std::int32_t SecretCgIndicesView::operator[](std::size_t index) const noexcept
{ return std::bit_cast<std::int32_t>(Index(*reference_, static_cast<std::uint32_t>(index))); }
std::size_t SecretCgNativeArchive::CategoryCount() const noexcept
{ return static_cast<std::uint32_t>(SpanCount(mappingsAt18.begin, mappingsAt18.end, 16)); }
std::size_t SecretCgNativeArchive::ItemCount(std::size_t category) const noexcept
{
    const auto& items = mappingsAt18.begin[category].referencesAt04;
    return static_cast<std::uint32_t>(SpanCount(items.begin, items.end, 12));
}
SecretCgIndicesView SecretCgNativeArchive::Indices(std::size_t category, std::size_t item) const noexcept
{ return SecretCgIndicesView(mappingsAt18.begin[category].referencesAt04.begin[item]); }
const SecretCgFlagRecord& SecretCgNativeArchive::RecordAt(std::size_t index) const noexcept
{ return Record(recordsAt0C, static_cast<std::uint32_t>(index)); }

const char* const* SecretCgNativeArchive::ThumbnailAt477FD0(
    ISecretCgParamsRootSource& source, std::int32_t category, std::int32_t item)
{
    if (category >= SpanCount(mappingsAt18.begin, mappingsAt18.end, 16)) return nullptr;
    const auto* mapping = reinterpret_cast<const SecretCgMappingRuntime*>(
        reinterpret_cast<std::uintptr_t>(mappingsAt18.begin) + static_cast<std::uint32_t>(category) * 16u);
    const auto& items = Required(mapping).referencesAt04;
    if (item >= SpanCount(items.begin, items.end, 12)) return nullptr;
    const auto* selected = reinterpret_cast<const YtlReferenceSlotRuntime*>(
        reinterpret_cast<std::uintptr_t>(items.begin) + static_cast<std::uint32_t>(item) * 12u);
    YtlReferenceSlotRuntime reference = Required(selected);
    auto* const control = reference.controlAt08;
    if (control) ++control->referenceCountAt04;
    ItemUnwind unwind(reference);
    const char* const* result = nullptr;
    for (std::int32_t index = 0; index < IndexCount(control); ++index)
    {
        if (!Record(recordsAt0C, Index(reference, index)).byteFlagAt04) continue;
        const auto& first = CaptureGameSystem(source, 0x4780DD);
        // 原此处越界会硬故障；现代UI入口明确拒绝损坏映射，不回退到另一代GS。
        if (static_cast<std::uint32_t>(category) >= first.registCg.size() ||
            static_cast<std::uint32_t>(item) >= first.registCg[category].itemsAt04.size())
            throw std::out_of_range("477FD0: selected CG has no matching RegistCg item");
        const char* name = first.registCg[category].itemsAt04[item].nameAt00.GetString();
        const auto& second = CaptureGameSystem(source, 0x47815B);
        result = BorrowParamsThumbnailAliasAt477800(second, name);
        break;
    }
    unwind.Dismiss();
    ReleaseCached(control);
    return result;
}

void SecretCgNativeArchive::Serialize(SerializeBuffer& serializer)
{
    if (serializer.IsWriting())
    {
        auto count = static_cast<std::uint32_t>(Slot<CountFunction>(this, 2)(this));
        serializer.SerializeSlot58Dword(count);
        for (std::int32_t index = 0; index < Slot<CountFunction>(this, 2)(this); ++index)
        {
            std::uint8_t encoded = 0;
            if (Slot<GetFunction>(this, 3)(this, index))
                encoded = serializer.EnvironmentKeys().NormalizedByteAt(baseKeyOffsetAt08 + index);
            serializer.SerializeSlot50Byte(encoded);
        }
        return;
    }
    std::uint32_t remaining = 0;
    serializer.SerializeSlot58Dword(remaining);
    for (std::int32_t index = 0; index < Slot<CountFunction>(this, 2)(this); ++index)
    {
        if (!remaining) Slot<SetFunction>(this, 4)(this, index, 0);
        else
        {
            std::uint8_t encoded = 0;
            serializer.SerializeSlot50Byte(encoded);
            const auto keyIndex = baseKeyOffsetAt08 + index;
            auto* const table = *reinterpret_cast<void* const* const*>(this);
            const auto key = serializer.EnvironmentKeys().NormalizedByteAt(keyIndex);
            reinterpret_cast<SetFunction>(table[4])(this, index, encoded == key);
            --remaining;
        }
    }
}

void RegisterSecretCgArchiveAt47E349(EnvironmentRootOwner& owner,
    ISecretCgParamsRootSource& params, IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    void* raw = ::operator new(SecretCgNativeArchive::OriginalObjectSize);
    SecretCgNativeArchive* archive = nullptr;
    try { if (raw) archive = ::new (raw) SecretCgNativeArchive(params, strings, memory); }
    catch (...)
    {
        // 599E61仅保护正文构造；后续Capture/control/append不拥有该raw清理资格。
        ::operator delete(raw, SecretCgNativeArchive::OriginalObjectSize);
        throw;
    }
    auto* root = owner.CaptureAt4180C0();
    RegisterEnvironmentArchiveAt46A4A0(*root, archive, memory);
}
}
