#include <Runtime/GuiControlResourceNative.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/RuntimeCStringVector.h>

#include <algorithm>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <new>
#include <mbstring.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = GuiControlResourceNativeBody38h;
using Slot = YtlReferenceSlotRuntime;
constexpr std::uint32_t MaximumReferences = 0x15555555u;

std::int32_t Distance(const Slot* begin, const Slot* end) noexcept
{
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end) -
        reinterpret_cast<std::uintptr_t>(begin));
    return std::bit_cast<std::int32_t>(bytes) / 12;
}
Slot* AllocateVector(std::uint32_t capacity)
{
    const auto bytes = capacity <= MaximumReferences ? capacity * 12u : 0xFFFFFFFFu;
    if (bytes < 0x1000u) return bytes ? static_cast<Slot*>(::operator new(bytes)) : nullptr;
    const auto request = bytes > 0xFFFFFFFFu - 0x23u ? 0xFFFFFFFFu : bytes + 0x23u;
    void* raw = ::operator new(request);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (reinterpret_cast<std::uintptr_t>(raw) + 0x23u) & ~std::uintptr_t{31};
    std::memcpy(reinterpret_cast<void*>(aligned - 4u), &raw, sizeof(raw));
    return reinterpret_cast<Slot*>(aligned);
}
void DeleteVector(Slot* begin, std::uint32_t capacity)
{
    auto bytes = capacity * 12u;
    void* raw = begin;
    if (bytes >= 0x1000u)
    {
        const auto address = reinterpret_cast<std::uintptr_t>(begin);
        std::memcpy(&raw, reinterpret_cast<const void*>(address - 4u), sizeof(raw));
        bytes += 0x23u;
        if (address - reinterpret_cast<std::uintptr_t>(raw) - 4u > 0x1Fu)
            _invalid_parameter_noinfo_noreturn();
    }
    ::operator delete(raw, bytes);
}
void CopySlot(Slot& to, const Slot& from) noexcept
{
    to.baseAt00 = from.baseAt00;
    to.biasAt04 = from.biasAt04;
    to.controlAt08 = from.controlAt08;
    if (to.controlAt08) ++to.controlAt08->referenceCountAt04;
}
void CopyRange(const Slot* begin, const Slot* end, Slot* output) noexcept
{
    for (; begin != end; ++begin, ++output) CopySlot(*output, *begin);
}
class ResourceControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ResourceControl(Body* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* body = static_cast<Body*>(payloadAt14);
        if (!body) return;
        DestroyGuiControlSurfaceReferencesAt43EAA0(body->surfacesAt1C);
        ::operator delete(body, std::size_t{0x38});
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
        { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(ResourceControl) == 0x18);

struct ReferenceUnwind final
{
    Slot& slot;
    bool active = true;
    ~ReferenceUnwind() noexcept { if (active) ReleaseYtlReference(slot); }
    void FinishNormally() noexcept
    {
        active = false;
        ReleaseYtlReference(slot);
    }
};
void ReleaseCached(YtlReferenceControlRuntime* control) noexcept
{
    if (!control || --control->referenceCountAt04) return;
    if (control->owningAt08) control->DestroyPayloadAtVslot00();
    control->DeleteControlAtVslot04(1);
}
void ConsumeArgumentNormally(Slot& argument) noexcept
{
    auto* control = argument.controlAt08;
    if (!control || --control->referenceCountAt04) return;
    if (control->owningAt08) control->DestroyPayloadAtVslot00();
    if (argument.controlAt08) argument.controlAt08->DeleteControlAtVslot04(1);
}
struct NamesUnwind final
{
    EnvironmentVectorStorage<RuntimeCString> names{};
    bool active = true;
    ~NamesUnwind() noexcept { if (active) DestroyRuntimeCStringVectorAt40DD20(names); }
    void FinishNormally() noexcept
    {
        active = false;
        DestroyRuntimeCStringVectorAt43A700(names);
    }
};
struct StringUnwind final
{
    union { RuntimeCString value; };
    bool active = false;
    StringUnwind() noexcept {}
    ~StringUnwind() noexcept { if (active) DestroyRuntimeCStringAt40CD40(&value); }
};
std::int32_t AddDword(std::int32_t first, std::int32_t second) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(first) +
        static_cast<std::uint32_t>(second));
}
GraphicsSurfaceNativeBody& RequireSurface(const Slot& reference)
{
    auto* body = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(reference);
    if (!body) throw std::logic_error("502630 dereferenced a null Surface body");
    return *body;
}
}

void ConstructGuiControlResourceBodyAt4DABB0(Body& body) noexcept
{
    body.surfacesAt1C = {};
    body.combinedRectAt28 = {};
    for (auto& index : body.slotIndicesAt00) index = -1;
    ::SetRectEmpty(&body.combinedRectAt28);
}
Body* NewGuiControlResourceAt5047DD()
{
    void* raw = ::operator new(0x38);
    if (!raw) return nullptr;
    auto* body = ::new(raw) Body;
    ConstructGuiControlResourceBodyAt4DABB0(*body);
    return body;
}
Slot* ConstructGuiControlResourceReferenceAt4D76A0(Slot& output, Body* body, std::uint8_t owning)
{
    output = {};
    if (body)
    {
        output.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
        void* raw = ::operator new(0x18);
        auto* control = raw ? ::new(raw) ResourceControl(body) : nullptr;
        output.controlAt08 = control;
        // 原4D76E2写空control故障；明确终止，不回滚已发布的正文。
        if (!control) _invalid_parameter_noinfo_noreturn();
        control->referenceCountAt04 = 1;
        output.controlAt08->owningAt08 = owning;
        static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->strideAt0C = 0x38;
        static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->countAt10 = 1;
    }
    return &output;
}
Body* ResolveGuiControlResource(const Slot& reference) noexcept
    { return ResolveEnvironmentObjectReference<Body>(reference); }
Body& RequireGuiControlResource(const Slot& reference)
{
    auto* body = ResolveGuiControlResource(reference);
    if (!body) throw std::logic_error("CElement dereferenced a null body");
    return *body;
}
void CopyGuiControlResourceReference(Slot& output, const Slot& source) noexcept
    { CopySlot(output, source); }
void ResetGuiControlResourceReferenceAt506CA0(Slot& reference)
    { ReleaseYtlReference(reference); }
void ReleaseGuiControlResourceReferenceAt4F6380(Slot& reference) noexcept
    { ReleaseYtlReference(reference); }
std::int32_t GuiControlSurfaceReferenceCount(const GuiControlSurfaceReferences& references) noexcept
    { return Distance(references.begin, references.end); }
std::int32_t GuiControlSurfaceReferenceCapacity(const GuiControlSurfaceReferences& references) noexcept
    { return Distance(references.begin, references.capacity); }

Slot* GrowGuiControlSurfaceReferencesAt4540A0(GuiControlSurfaceReferences& refs,
    Slot* position, const Slot& source)
{
    const auto index = static_cast<std::uint32_t>(Distance(refs.begin, position));
    const auto size = static_cast<std::uint32_t>(Distance(refs.begin, refs.end));
    if (size == MaximumReferences) throw std::length_error("vector<T> too long");
    const auto required = size + 1;
    const auto capacity = static_cast<std::uint32_t>(Distance(refs.begin, refs.capacity));
    const auto grown = capacity > MaximumReferences - capacity / 2 ? required : capacity + capacity / 2;
    const auto nextCapacity = (std::max)(required, grown);
    auto* next = AllocateVector(nextCapacity);
    auto* inserted = next + index;
    auto* cleanupBegin = inserted;
    try
    {
        CopySlot(*inserted, source);
        // 4541D1之后才读旧头；先建插入值，不能用move改写别名源。
        if (position == refs.end) CopyRange(refs.begin, refs.end, next);
        else
        {
            CopyRange(refs.begin, position, next);
            cleanupBegin = next;
            CopyRange(position, refs.end, inserted + 1);
        }
    }
    catch (...)
    {
        ReleaseGuiControlSurfaceRangeAt43EA10(cleanupBegin, inserted + 1);
        DeleteVector(next, nextCapacity);
        throw;
    }
    // 45424B关闭构造保护，旧区清理期间三指针仍旧。
    if (refs.begin)
    {
        ReleaseGuiControlSurfaceRangeAt43EA10(refs.begin, refs.end);
        DeleteVector(refs.begin, static_cast<std::uint32_t>(Distance(refs.begin, refs.capacity)));
    }
    refs.begin = next;
    refs.end = next + required;
    refs.capacity = next + nextCapacity;
    return refs.begin + index;
}
void AppendGuiControlSurfaceReference(GuiControlSurfaceReferences& refs, const Slot& source)
{
    auto* position = refs.end;
    if (position != refs.capacity)
    {
        CopySlot(*position, source);
        ++refs.end;
        return;
    }
    static_cast<void>(GrowGuiControlSurfaceReferencesAt4540A0(refs, position, source));
}
void ReleaseGuiControlSurfaceRangeAt43EA10(Slot* begin, Slot* end) noexcept
{
    for (; begin != end; ++begin) ReleaseYtlReference(*begin);
}
void DestroyGuiControlSurfaceReferencesAt43EAA0(GuiControlSurfaceReferences& refs) noexcept
{
    if (!refs.begin) return;
    ReleaseGuiControlSurfaceRangeAt43EA10(refs.begin, refs.end);
    DeleteVector(refs.begin, static_cast<std::uint32_t>(Distance(refs.begin, refs.capacity)));
    refs = {};
}

Slot* GrowGuiControlResourceReferencesAt4D8ED0(GuiControlResourceReferences& refs,
    Slot* position, const Slot& source)
{
    // 4D8ED0与4540A0逐指令同序；两种范围release均重读槽且清理异常终止。
    return GrowGuiControlSurfaceReferencesAt4540A0(refs, position, source);
}
void AppendGuiControlResourceReference(GuiControlResourceReferences& refs, const Slot& source)
    { AppendGuiControlSurfaceReference(refs, source); }
void AppendGuiControlResourceArgumentAt4FF1F0(GuiControlResourceReferences& refs, Slot& argument)
{
    ReferenceUnwind unwind{argument};
    AppendGuiControlResourceReference(refs, argument);
    unwind.active = false;
    ConsumeArgumentNormally(argument);
}
void ReleaseGuiControlResourceRangeAt4D7DE0(Slot* begin, Slot* end) noexcept
    { ReleaseGuiControlSurfaceRangeAt43EA10(begin, end); }
void DestroyGuiControlResourceReferencesAt4F6B00(GuiControlResourceReferences& refs) noexcept
    { DestroyGuiControlSurfaceReferencesAt43EAA0(refs); }

Slot* LoadGuiControlResourceAt5047A0(Slot& output, const char* const* sevenNames,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings)
{
    GraphicsSurfaceNativeScope scope(surfaces);
    Slot group;
    ConstructGuiControlResourceReferenceAt4D76A0(group, NewGuiControlResourceAt5047DD(), 1);
    ReferenceUnwind groupUnwind{group};
    std::uintptr_t returnedBase = 0;
    try
    {
        RECT combined{};
        NamesUnwind names;
        for (std::size_t slot = 0; slot < 7; ++slot)
        {
            if (!sevenNames[slot]) continue;
            std::int32_t index = 0;
            auto* name = names.names.begin;
            for (; name != names.names.end; ++name, ++index)
            {
                if (!sevenNames[slot]) throw ATL::CAtlException(E_FAIL);
                if (_mbscmp(reinterpret_cast<const unsigned char*>(name->GetString()),
                    reinterpret_cast<const unsigned char*>(sevenNames[slot])) == 0) break;
            }
            if (name != names.names.end)
            {
                auto& destination = RequireGuiControlResource(group);
                if (index >= GuiControlSurfaceReferenceCount(destination.surfacesAt1C)) throw 1;
                destination.slotIndicesAt00[slot] = index;
                continue;
            }
            Slot surface;
            ReferenceUnwind surfaceUnwind{surface};
            void* receiver = loader.CaptureLoadReceiverAt413BE0();
            const char* loadName = sevenNames[slot];
            AdoptGraphicsSurfaceAt407A80(surface, NewGraphicsSurfaceAt40C2E0(surfaces, strings));
            auto* surfaceControl = surface.controlAt08;
            if (!loader.LoadIntoAt4800A0(receiver, RequireSurface(surface), loadName)) throw 1;
            // 50499E重读base/bias，但解析仍使用Load之前缓存的EDI control。
            Slot cached{surface.baseAt00, surface.biasAt04, surfaceControl};
            auto& loaded = RequireSurface(cached);
            const RECT rect{loaded.originXAt10, loaded.originYAt14,
                AddDword(loaded.originXAt10, loaded.widthAt08),
                AddDword(loaded.originYAt14, loaded.heightAt0C)};
            ::UnionRect(&combined, &combined, &rect);
            Slot appended;
            CopySlot(appended, cached);
            ReferenceUnwind appendedUnwind{appended};
            auto& destination = RequireGuiControlResource(group);
            const auto resourceIndex = GuiControlSurfaceReferenceCount(destination.surfacesAt1C);
            AppendGuiControlSurfaceReference(destination.surfacesAt1C, appended);
            destination.slotIndicesAt00[slot] = resourceIndex;
            appendedUnwind.active = false;
            auto* appendedControl = appended.controlAt08;
            ReleaseCached(appendedControl);
            if (appendedControl) { appended.controlAt08 = nullptr; appended.baseAt00 = 0; }

            const char* recordedName = sevenNames[slot];
            StringUnwind text;
            InitializeEmptyRuntimeCString(text.value, strings);
            text.active = true;
            if (recordedName && (reinterpret_cast<std::uintptr_t>(recordedName) >> 16u) == 0)
                static_cast<void>(LoadRuntimeCStringResourceAt420510(text.value,
                    static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(recordedName))));
            else SetRuntimeCStringAt437680(text.value, recordedName,
                recordedName ? static_cast<int>(std::strlen(recordedName)) : 0);
            ATL::CStringData* normalHeader;
            if (names.names.end != names.names.capacity)
            {
                // 504B66缓存ESI跨Clone，正常临时释放不能重读可能被回调替换的TEXT槽。
                auto* position = names.names.end;
                normalHeader = reinterpret_cast<ATL::CStringData*>(
                    const_cast<char*>(text.value.GetString()) - 16);
                auto* cloned = CloneRuntimeCStringDataAt414540(normalHeader);
                char* data = reinterpret_cast<char*>(cloned) + 16;
                // Clone引用只写入尚未发布的尾槽，end及临时释放时序保持不变。
                std::memcpy(static_cast<void*>(position), &data, sizeof(data));
                ++names.names.end;
            }
            else
            {
                // 456A10与4085C0同序；原慢路504B90在增长返回后才重读ESI。
                AppendRuntimeCStringAt41D5E9(names.names, text.value);
                normalHeader = reinterpret_cast<ATL::CStringData*>(
                    const_cast<char*>(text.value.GetString()) - 16);
            }
            text.active = false;
            ReleaseRuntimeCStringHeader(normalHeader);
            surfaceUnwind.active = false;
            ReleaseCached(surfaceControl);
            if (surfaceControl) { surface.controlAt08 = nullptr; surface.baseAt00 = 0; }
        }
        returnedBase = group.baseAt00;
        ::CopyRect(&RequireGuiControlResource(group).combinedRectAt28, &combined);
        names.FinishNormally();
    }
    catch (...)
    {
        // 504BF0在局部Surface/CString/vector展开之后执行；本次release可抛。
        ResetGuiControlResourceReferenceAt506CA0(group);
        returnedBase = group.baseAt00;
    }
    output.baseAt00 = returnedBase;
    output.biasAt04 = group.biasAt04;
    auto* control = group.controlAt08;
    output.controlAt08 = control;
    if (control) ++control->referenceCountAt04;
    groupUnwind.active = false;
    ReleaseCached(control);
    return &output;
}

Slot* CopyGuiControlResourceAt502630(Slot& output, const Body& source,
    IGraphicsSurfaceApi& surfaces, IGraphicsSurfaceCopyApi& copy, IRuntimeStringManagerSource& strings)
{
    GraphicsSurfaceNativeScope scope(surfaces);
    auto* body = NewGuiControlResourceAt5047DD();
    ConstructGuiControlResourceReferenceAt4D76A0(output, body, 1);
    ReferenceUnwind result{output};
    for (auto* position = source.surfacesAt1C.begin; position != source.surfacesAt1C.end; ++position)
    {
        // 源正文在new24之前固定，尺寸/alpha则在目标Ref构造完成之后读取。
        auto* selected = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(*position);
        auto* created = NewGraphicsSurfaceAt40C2E0(surfaces, strings);
        Slot temporary;
        ConstructGraphicsSurfaceReferenceAt451500(temporary, created, 1);
        ReferenceUnwind element{temporary};
        if (!selected) throw std::logic_error("502630 source Surface body is null");
        // 原栈槽只写alpha低字节；不读取未初始化的高24位，外部API明确零扩展。
        const auto alpha = static_cast<std::int32_t>(selected->alphaAt18);
        if (!RequireSurface(temporary).CreateAt415010(surfaces,
            selected->widthAt08, selected->heightAt0C, alpha, 1))
        {
            ReleaseYtlReference(temporary);
        }
        else
        {
            static_cast<void>(copy.CopyBlitSurface(RequireSurface(temporary).handleAt04,
                selected->handleAt04, 0, 0, nullptr, nullptr, nullptr, 0));
            auto& destination = RequireSurface(temporary);
            destination.originXAt10 = selected->originXAt10;
            destination.originYAt14 = selected->originYAt14;
        }
        AppendGuiControlSurfaceReference(RequireGuiControlResource(output).surfacesAt1C, temporary);
        element.FinishNormally();
    }
    for (std::size_t slot = 0; slot < 7; ++slot)
    {
        const auto index = source.slotIndicesAt00[slot];
        auto& destination = RequireGuiControlResource(output);
        if (index < GuiControlSurfaceReferenceCount(destination.surfacesAt1C))
            destination.slotIndicesAt00[slot] = index;
    }
    auto& destination = RequireGuiControlResource(output);
    const RECT combined = source.combinedRectAt28;
    ::CopyRect(&destination.combinedRectAt28, &combined);
    result.active = false;
    return &output;
}
}
