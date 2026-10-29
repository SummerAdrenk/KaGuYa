#include <Runtime/SaveFileNativePages.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>

#include <array>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
class RetiredControl final : public YtlReferenceControlRuntime
{
public:
    RetiredControl() noexcept : YtlReferenceControlRuntime(UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
    std::uint32_t strideAt0C;
    std::uint32_t countAt10;
};
static_assert(sizeof(RetiredControl) == 0x14);

void RetireControl(EnvironmentObjectReferenceControl* control, std::uint32_t flags)
{
    // 4683D0/468310/467DA0：先退化为5ADD18，字段不改；首次删18h，退役后删14h。
    std::array<std::byte, 20> fields;
    auto* raw = reinterpret_cast<std::byte*>(control);
    std::memcpy(fields.data(), raw + 4, fields.size());
    ::new (raw) RetiredControl;
    std::memcpy(raw + 4, fields.data(), fields.size());
    if (flags & 1u) ::operator delete(raw, std::size_t{0x18});
}

void DestroyPayload(SaveFileNativeTime* time)
{
    ::operator delete(time, std::size_t{8}); // 492D80：null也交给delete。
}
void DestroyPayload(SaveFileNativeItem* item)
{
    if (!item) return;
    DestroySaveFileItemAt4653A0(*item);
    ::operator delete(item, std::size_t{0x10});
}
void DestroyPayload(SaveFileNativePage* page)
{
    if (!page) return;
    DestroySaveFileItemsAt462090(page->itemsAt04);
    ::operator delete(page, sizeof(SaveFileNativePage));
}

template<class T>
class Control final : public EnvironmentObjectReferenceControl
{
public:
    explicit Control(T* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { DestroyPayload(static_cast<T*>(payloadAt14)); }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireControl(this, flags); }
};
static_assert(sizeof(Control<SaveFileNativePage>) == 0x18);
static_assert(sizeof(Control<SaveFileNativeItem>) == 0x18);
static_assert(sizeof(Control<SaveFileNativeTime>) == 0x18);

template<class T>
Control<T>* AllocateControl(T* body, IEnvironmentRootMemory& memory)
{
    void* raw = memory.AllocateBody(0x18);
    return raw ? ::new (raw) Control<T>(body) : nullptr;
}

template<class T>
void AdoptFresh(YtlReferenceSlotRuntime& fresh, T* body, IEnvironmentRootMemory& memory)
{
    fresh = {};
    if (!body) return;
    fresh.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    auto* control = AllocateControl(body, memory);
    // 470934/4709EE：本段尚无局部Ref清理资格；null control仍是原来的错误路径。
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = sizeof(T);
    fresh.controlAt08 = control;
    control->countAt10 = 1;
}

template<class T>
void Replace(YtlReferenceSlotRuntime& destination, T* body, IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(destination);
    if (!body) return;
    if (destination.controlAt08) destination.controlAt08->DeleteControlAtVslot04(1);
    destination.biasAt04 = 0;
    destination.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    auto* control = AllocateControl(body, memory);
    // 451CD1/470AF2：替换分支先发布control，后逐项从目标槽重读。
    destination.controlAt08 = control;
    control->referenceCountAt04 = 1;
    destination.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(destination.controlAt08)->strideAt0C = sizeof(T);
    static_cast<EnvironmentObjectReferenceControl*>(destination.controlAt08)->countAt10 = 1;
}

bool Later(const SaveFileNativeTime& time, const SaveFileNativeTime& latest) noexcept
{
    const auto high = std::bit_cast<std::int32_t>(time.highAt04);
    const auto oldHigh = std::bit_cast<std::int32_t>(latest.highAt04);
    return high > oldHigh || (high == oldHigh && time.lowAt00 > latest.lowAt00);
}
}

SaveFileNativePage* CreateSaveFilePageAt4708C7(IEnvironmentRootMemory& memory)
{
    void* raw = memory.AllocateBody(sizeof(SaveFileNativePage));
    if (!raw) return nullptr;
    auto* page = ::new (raw) SaveFileNativePage;
    page->itemsAt04 = {};
    return page;
}

SaveFileNativeItem* CreateSaveFileItemAt470961(IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory)
{
    void* raw = memory.AllocateBody(0x10);
    if (!raw) return nullptr;
    auto* item = ::new (raw) SaveFileNativeItem;
    // 47098D..4709A5在state7内，无该item正文的raw delete/成员展开资格。
    InitializeEmptyRuntimeCString(item->labelAt0C, strings);
    return item;
}

SaveFileNativeTime* CreateSaveFileTimeAt470A40(const SaveFileNativeTime& source,
    IEnvironmentRootMemory& memory)
{
    void* raw = memory.AllocateBody(8);
    if (!raw) return nullptr;
    auto* time = ::new (raw) SaveFileNativeTime;
    time->lowAt00 = source.lowAt00;
    time->highAt04 = source.highAt04;
    return time;
}

void AdoptSaveFilePageAt4708F0(YtlReferenceSlotRuntime& fresh, SaveFileNativePage* page,
    IEnvironmentRootMemory& memory) { AdoptFresh(fresh, page, memory); }
void AdoptSaveFileItemAt4709B5(YtlReferenceSlotRuntime& fresh, SaveFileNativeItem* item,
    IEnvironmentRootMemory& memory) { AdoptFresh(fresh, item, memory); }
void ReplaceSaveFilePageAt451C60(YtlReferenceSlotRuntime& destination, SaveFileNativePage* page,
    IEnvironmentRootMemory& memory) { Replace(destination, page, memory); }
void ReplaceSaveFileTimeAt470A6E(YtlReferenceSlotRuntime& destination, SaveFileNativeTime* time,
    IEnvironmentRootMemory& memory) { Replace(destination, time, memory); }

void DestroySaveFileItemAt4653A0(SaveFileNativeItem& item) noexcept
{
    // FuncInfo5E8248 flags=5；先释放字符串，再重读时间Ref。Free抛出不能继续释放Ref。
    DestroyRuntimeCStringAt40CD40(&item.labelAt0C);
    ReleaseYtlReference(item.timeAt00);
}
void ClearSaveFileItemAt5024C0(SaveFileNativeItem& item)
{
    ReleaseYtlReference(item.timeAt00);
    SetRuntimeCStringAt437680(item.labelAt0C, "", 0);
}
void ClearSaveFilePageAt502510(SaveFileNativePage& page)
{
    // 502584每次重读end；出错保留前缀且不提前将availability清零。
    for (auto* entry = page.itemsAt04.begin; entry != page.itemsAt04.end; ++entry)
        ClearSaveFileItemAt5024C0(*ResolveEnvironmentObjectReference<SaveFileNativeItem>(*entry));
    page.availabilityAt00 = 0;
}
void UpdateSaveFilePageCountAt48EFE0(SaveFileNativePage& page) noexcept
{
    // 48EFE7缓存end；只检查time Ref的base，不解CTime，不按原page计数短路。
    const auto* end = page.itemsAt04.end;
    std::uint32_t count = 0;
    for (auto* entry = page.itemsAt04.begin; entry != end; ++entry)
        if (ResolveEnvironmentObjectReference<SaveFileNativeItem>(*entry)->timeAt00.baseAt00) ++count;
    page.availabilityAt00 = count;
}
void DestroySaveFileItemsAt462090(EnvironmentVectorStorage<YtlReferenceSlotRuntime>& items,
    IEnvironmentRootMemory& memory) noexcept
{
    // 452950/462090与4523B0/497080逐指令同构：固定范围释放，随后重读begin/capacity。
    DestroyEnvironmentArchiveReferencesAt497080(items, memory);
}
void DestroySaveFilePagesAt4623A0(EnvironmentVectorStorage<YtlReferenceSlotRuntime>& pages,
    IEnvironmentRootMemory& memory) noexcept
{
    DestroyEnvironmentArchiveReferencesAt497080(pages, memory);
}

YtlReferenceControlRuntime* AppendSaveFileReferenceAt470D1A(
    EnvironmentVectorStorage<YtlReferenceSlotRuntime>& target,
    const YtlReferenceSlotRuntime& local, YtlReferenceControlRuntime* cachedControl,
    IEnvironmentRootMemory& memory)
{
    auto* end = target.end;
    if (end != target.capacity)
    {
        end->baseAt00 = local.baseAt00;
        end->biasAt04 = 0;
        end->controlAt08 = cachedControl;
        if (cachedControl) ++cachedControl->referenceCountAt04;
        ++target.end;
        return cachedControl;
    }
    // 4534C0/453730的末尾插入与454310同构；慢路径重新读取完整local Ref。
    AppendEnvironmentArchiveReferenceAt49852D(target, local, memory);
    return local.controlAt08;
}

void ReleaseSaveFileCachedReferenceAt470D50(YtlReferenceSlotRuntime& local,
    YtlReferenceControlRuntime* cachedControl) noexcept
{
    if (!cachedControl) return;
    if (--cachedControl->referenceCountAt04 == 0)
    {
        if (cachedControl->owningAt08) cachedControl->DestroyPayloadAtVslot00();
        cachedControl->DeleteControlAtVslot04(1);
    }
    local.controlAt08 = nullptr;
    local.baseAt00 = 0;
}

std::int32_t FindLatestSaveFileItemAt48D090(YtlReferenceSlotRuntime ownedPage) noexcept
{
    auto* cachedControl = ownedPage.controlAt08;
    auto* page = ResolveEnvironmentObjectReference<SaveFileNativePage>(ownedPage);
    std::int32_t result = -1;
    if (page->availabilityAt00 != 0)
    {
        SaveFileNativeTime latest{0, 0};
        for (auto* slot = page->itemsAt04.begin;
            slot != ResolveEnvironmentObjectReference<SaveFileNativePage>(ownedPage)->itemsAt04.end; ++slot)
        {
            auto* item = ResolveEnvironmentObjectReference<SaveFileNativeItem>(*slot);
            if (!item->timeAt00.baseAt00) continue;
            const auto* time = ResolveEnvironmentObjectReference<SaveFileNativeTime>(item->timeAt00);
            if (!Later(*time, latest)) continue;
            latest = *time;
            const auto begin = ResolveEnvironmentObjectReference<SaveFileNativePage>(ownedPage)->itemsAt04.begin;
            const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(slot) -
                reinterpret_cast<std::uintptr_t>(begin));
            result = std::bit_cast<std::int32_t>(bytes) / 12;
        }
    }
    // 48D24D缓存edi减持；payload返回后48D25F重读栈上参数control，不清参数字段。
    if (cachedControl && --cachedControl->referenceCountAt04 == 0)
    {
        if (cachedControl->owningAt08) cachedControl->DestroyPayloadAtVslot00();
        if (ownedPage.controlAt08) ownedPage.controlAt08->DeleteControlAtVslot04(1);
    }
    return result;
}
}
