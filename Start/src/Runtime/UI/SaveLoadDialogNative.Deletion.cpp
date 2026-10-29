#include <Runtime/SaveLoadDialogNativeDeletion.h>
#include <Runtime/SaveLoadDialogNativeCommands.h>
#include <Runtime/SaveFileStateNativeArchive.h>
#include <Runtime/SaveFileSelectionNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/SaveFileCatalog.h>
#include <Runtime/SaveLoadResourceProfile.h>
#include <algorithm>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
using Common = GuiDialogNativeBody5Ch;
struct Reference final
{
    Slot slot;
    bool armed = true;
    ~Reference() noexcept { if (armed) ReleaseYtlReference(slot); }
    void Assign(const Slot& source)
    {
        if (&slot == &source) return;
        ReleaseYtlReference(slot);
        slot = source;
        if (slot.controlAt08) ++slot.controlAt08->referenceCountAt04;
    }
    // 正常临时释放的Destroy后重读control；state4/8/10中的第二异常直接terminate。
    void FinishTemporary() noexcept
    {
        armed = false;
        auto* control = slot.controlAt08;
        if (control && --control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
        }
    }
    // 507139/50719D使用缓存EDI；不重读或清空三DWORD。
    void FinishCached(YtlReferenceControlRuntime* control) noexcept
    {
        armed = false;
        if (control && --control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            control->DeleteControlAtVslot04(1);
        }
    }
};
struct FactoryUnwind final
{
    SaveFileDeleteFactoryNativeBody0Ch& body;
    bool armed = true;
    ~FactoryUnwind() noexcept { if (armed) DestroySaveFileDeleteFactoryNativeAt463A90(body); }
    void Finish()
    {
        armed = false;
        DestroySaveFileDeleteFactoryNativeAt463A90(body);
    }
};
Slot* Find(SaveLoadDialogNativeBody184h& body, Slot* output, std::int32_t id)
{
    return reinterpret_cast<const SaveLoadDialogNativeVtable37*>(body.common.vptrAt00)
        ->findDialogAt90(&body.common, output, id);
}
void Select(Reference& saved, SaveLoadDialogNativeBody184h& body, std::int32_t id)
{
    Reference temporary;
    temporary.armed = false;
    const auto* returned = Find(body, &temporary.slot, id);
    temporary.armed = true;
    saved.Assign(*returned);
    temporary.FinishTemporary();
}
Common* CachedPage(const Slot& slot, YtlReferenceControlRuntime* control) noexcept
{
    const auto stride = control ? *reinterpret_cast<const std::uint32_t*>(
        reinterpret_cast<std::uintptr_t>(control) + 0x0C) : 0u;
    return reinterpret_cast<Common*>(slot.baseAt00 + stride * slot.biasAt04);
}
std::int32_t& PageIndex(Common* page) noexcept
{
    return *reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uintptr_t>(page) + 0x7C);
}
SaveFileStateNativeArchive* Catalog(ISaveLoadDialogNativeDeletionSource& source)
{
    return static_cast<SaveFileStateNativeArchive*>(
        ResolveEnvironmentArchiveAt417990(*source.CaptureEnvironmentAt4180C0(), 6));
}
SaveFileStateNativeArchive* InlineCatalog(ISaveLoadDialogNativeDeletionSource& source)
{
    auto* env = source.CaptureEnvironmentAt4180C0();
    const auto bytes = std::uint32_t(reinterpret_cast<std::uintptr_t>(env->archivesAt30.end) -
        reinterpret_cast<std::uintptr_t>(env->archivesAt30.begin));
    const auto count = std::uint32_t(std::bit_cast<std::int32_t>(bytes) / 12);
    return count > 6 ? ResolveEnvironmentObjectReference<SaveFileStateNativeArchive>(
        env->archivesAt30.begin[6]) : nullptr;
}
SaveFileNativePage* RegularPage(SaveFileStateNativeArchive* catalog, std::int32_t index) noexcept
{
    const auto address = reinterpret_cast<std::uintptr_t>(catalog->regularPagesAt20.begin) +
        std::uint32_t(index) * 12u;
    return ResolveEnvironmentObjectReference<SaveFileNativePage>(*reinterpret_cast<const Slot*>(address));
}
std::int32_t Increment(std::int32_t value) noexcept
{
    return std::bit_cast<std::int32_t>(std::uint32_t(value) + 1u);
}
}
std::int32_t ExecuteSaveLoadDeleteNativeAt506CE0(SaveLoadDialogNativeBody184h& body,
    ISaveLoadDialogNativeDeletionSource& source)
{
    Reference selected;
    YtlReferenceControlRuntime* cachedControl;
    const auto mode = body.pageModeAtE4;
    if (mode == 2 || mode == 3)
    {
        SaveFileDeleteFactoryNativeRollingBody10h factory;
        ConstructRollingSaveFileDeleteFactoryNativeAt45B800(factory, mode == 3, source);
        FactoryUnwind unwind{factory.base};
        auto* catalog = InlineCatalog(source);
        auto* latest = reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uintptr_t>(catalog) +
            (mode == 2 ? 4u : 8u));
        DeleteSaveFileFactoryAllNativeAt5022E0(factory.base, latest, source);
        catalog = Catalog(source);
        ClearSaveFilePageAt502510(*ResolveEnvironmentObjectReference<SaveFileNativePage>(
            mode == 2 ? catalog->quickPageAt2C : catalog->autoPageAt38));
        catalog = Catalog(source);
        if (mode == 2) catalog->quickLatestItemAt04 = -1;
        else catalog->autoLatestItemAt08 = -1;
        Select(selected, body, mode == 2 ? 4 : 5);
        unwind.Finish();
        cachedControl = selected.slot.controlAt08;
    }
    else
    {
        Select(selected, body, 2 + (mode != 0));
        if (!selected.slot.baseAt00) return 0;
        cachedControl = selected.slot.controlAt08;
        const auto pageIndex = body.slotIndexAt164<=-2 ? -2-body.slotIndexAt164 :
            PageIndex(CachedPage(selected.slot, cachedControl));
        SaveFileDeleteFactoryNativeBody0Ch factory;
        ConstructRegularSaveFileDeleteFactoryNativeAt4DCDB0(factory,
            SaveFilePhysicalPageNumber(*Catalog(source), pageIndex), source);
        FactoryUnwind unwind{factory};
        const auto itemIndex = body.slotIndexAt164;
        if (itemIndex >= 0)
        {
            DeleteSaveFileFactorySlotNativeAt476460(factory, Increment(itemIndex), 1, source);
            auto* page = RegularPage(Catalog(source), pageIndex);
            const auto address = reinterpret_cast<std::uintptr_t>(page->itemsAt04.begin) +
                std::uint32_t(body.slotIndexAt164) * 12u;
            ClearSaveFileItemAt5024C0(*ResolveEnvironmentObjectReference<SaveFileNativeItem>(
                *reinterpret_cast<const Slot*>(address)));
            // 5070A8再次取Env/page，缓存当前end后遍历，不能复用第一次catalog。
            page = RegularPage(Catalog(source), pageIndex);
            UpdateSaveFilePageCountAt48EFE0(*page);
        }
        else
        {
            for (std::int32_t slot = 1; slot <= CurrentSaveLoadResourceProfile().catalog.slotsPerPage; ++slot)
                DeleteSaveFileFactorySlotNativeAt476460(factory, slot, 1, source);
            ClearSaveFilePageAt502510(*RegularPage(Catalog(source), pageIndex));
            if (CurrentSaveLoadResourceProfile().catalog.kind == SaveLoadCatalogKind::ExistingPhysicalPages)
            {
                auto* catalog = Catalog(source);
                RemoveSaveFileCatalogPage(*catalog, pageIndex);
                auto& selectedIndex=PageIndex(CachedPage(selected.slot, cachedControl));
                selectedIndex=std::min(selectedIndex, static_cast<int>(catalog->RegularPages().size()) - 1);
            }
        }
        (void)source.CaptureInformationAfter413BE0();
        source.RefreshSaveSelectionAt48DBD0();
        unwind.Finish();
    }
    if (!selected.slot.baseAt00)
    {
        selected.FinishCached(cachedControl);
        return 0;
    }
    auto* page = CachedPage(selected.slot, cachedControl);
    const auto* table = *reinterpret_cast<const std::byte* const*>(page);
    using Notify = void (__thiscall*)(Common*, std::int32_t, std::int32_t, std::int32_t);
    const auto notify = *reinterpret_cast<const Notify*>(table + 0x8C);
    const auto previous = PageIndex(page);
    PageIndex(page) = -1;
    // 50718C的ECX仍为page；ret 0Ch不意味着没有this的stdcall。
    notify(page, previous, 1, 0);
    selected.FinishCached(cachedControl);
    return 1;
}
std::int32_t ConfirmSaveLoadDeleteNativeAt51B8F0(SaveLoadDialogNativeBody184h& body,
    ISaveLoadDialogNativeDeleteConfirmationSource& confirmation,
    ISaveLoadDialogNativeDeletionSource& deletion)
{
    return ConfirmSaveLoadDeleteNativeAt51B8F0(body, confirmation,
        [](SaveLoadDialogNativeBody184h& target, void* services)
        {
            (void)ExecuteSaveLoadDeleteNativeAt506CE0(target,
                *static_cast<ISaveLoadDialogNativeDeletionSource*>(services));
        }, &deletion);
}
void ISaveLoadDialogNativeDeletionSource::RefreshSaveSelectionAt48DBD0()
{
    (void)RefreshSaveFileSelectionNativeAt48D290(*this);
}
}
