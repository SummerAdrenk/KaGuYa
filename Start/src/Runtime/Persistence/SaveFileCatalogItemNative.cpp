#include <Runtime/SaveFileCatalogItemNative.h>

#include <Runtime/SaveFileNativePages.h>
#include <Runtime/SaveFileSelectionNative.h>
#include <Runtime/SaveFileStateNativeArchive.h>
#include <Runtime/SaveFileCatalog.h>

#include <bit>
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
union TextSlot
{
    TextSlot() noexcept {}
    ~TextSlot() noexcept {}
    RuntimeCString value;
};

ATL::CStringData* Header(const char* chars) noexcept
{
    return reinterpret_cast<ATL::CStringData*>(
        reinterpret_cast<std::uintptr_t>(chars) - sizeof(ATL::CStringData));
}

void PublishText(RuntimeCString& output, ATL::CStringData* header) noexcept
{
    const auto chars = reinterpret_cast<std::uintptr_t>(header) + sizeof(ATL::CStringData);
    std::memcpy(&output, &chars, sizeof(chars));
}

struct TextUnwind final
{
    RuntimeCString& value;
    bool active = true;
    // EH读当前TEXT；正常先撤资格，再按该调用点的缓存或当前header释放。
    ~TextUnwind() noexcept { if (active) DestroyRuntimeCStringAt40CD40(&value); }
    void FinishCurrent()
    {
        active = false;
        DestroyRuntimeCStringAt40CD40(&value);
    }
    void FinishCached(const char* chars)
    {
        active = false;
        ReleaseRuntimeCStringHeader(Header(chars));
    }
};

template<class Function>
Function ManagerSlot(ATL::IAtlStringMgr* manager, std::size_t offset) noexcept
{
    const std::byte* table;
    std::memcpy(&table, manager, sizeof(table));
    Function result;
    std::memcpy(&result, table + offset, sizeof(result));
    return result;
}

RuntimeCString& CaptureDirectory(ISaveFileCatalogItemNativeSource& source)
{
    auto* root = source.CaptureEnvironmentAt4180C0();
    auto* paths = ResolveEnvironmentObjectReference<std::byte>(root->pathsAt48);
    return *reinterpret_cast<RuntimeCString*>(reinterpret_cast<std::uintptr_t>(paths) + 0x10u);
}

SaveFileStateNativeArchive* CaptureCatalog(ISaveFileCatalogItemNativeSource& source)
{
    auto* root = source.CaptureEnvironmentAt4180C0();
    const auto bytes = std::uint32_t(reinterpret_cast<std::uintptr_t>(root->archivesAt30.end) -
        reinterpret_cast<std::uintptr_t>(root->archivesAt30.begin));
    // 417990按signed字节差除12，随后unsigned比较slot6；不增持根/归档Ref。
    const auto count = std::uint32_t(std::bit_cast<std::int32_t>(bytes) / 12);
    return count > 6 ? ResolveEnvironmentObjectReference<SaveFileStateNativeArchive>(
        root->archivesAt30.begin[6]) : nullptr;
}

YtlReferenceSlotRuntime& RefAt(YtlReferenceSlotRuntime* begin, std::int32_t index) noexcept
{
    const auto address = reinterpret_cast<std::uintptr_t>(begin) + std::uint32_t(index) * 12u;
    return *reinterpret_cast<YtlReferenceSlotRuntime*>(address);
}

SaveFileNativePage* CapturePage(ISaveFileCatalogItemNativeSource& source, std::int32_t page)
{
    auto* catalog = CaptureCatalog(source);
    if (page == -1) return ResolveEnvironmentObjectReference<SaveFileNativePage>(catalog->quickPageAt2C);
    if (page == -2) return ResolveEnvironmentObjectReference<SaveFileNativePage>(catalog->autoPageAt38);
    return ResolveEnvironmentObjectReference<SaveFileNativePage>(RefAt(catalog->regularPagesAt20.begin, page));
}

SaveFileNativeItem* CaptureItem(ISaveFileCatalogItemNativeSource& source,
    std::int32_t page, std::int32_t item)
{
    auto* selected = CapturePage(source, page);
    return ResolveEnvironmentObjectReference<SaveFileNativeItem>(RefAt(selected->itemsAt04.begin, item));
}

// 对应三支内联的Clone/GetNil/4145E0；复用CString原核，不增加现代格式串副本。
void ConstructDirectoryFormat(RuntimeCString& output, RuntimeCString& directory,
    const char* suffix, std::uint32_t suffixLength, ISaveFileCatalogItemNativeSource& source)
{
    using Clone = ATL::IAtlStringMgr* (__thiscall*)(ATL::IAtlStringMgr*);
    auto* manager = Header(directory.GetString())->pStringMgr;
    if (manager) manager = ManagerSlot<Clone>(manager, 16)(manager);
    if (!manager)
    {
        auto* global = source.Strings().CaptureAt417360();
        manager = ManagerSlot<Clone>(global, 16)(global);
        if (!manager) throw ATL::CAtlException(E_FAIL);
    }
    using GetNil = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*);
    auto* nil = ManagerSlot<GetNil>(manager, 12)(manager);
    PublishText(output, nil);
    TextUnwind unwind{output};
    // Clone/GetNil回调之后重读原目录槽；目录本身是printf格式串的一部分。
    const char* first = directory.GetString();
    ConcatenateRuntimeCStringAt4145E0(output, first,
        std::uint32_t(Header(first)->nDataLength), suffix, suffixLength);
    unwind.active = false;
}

void CloneParamsText(RuntimeCString& output, ScriptParamsNativeBody& params, bool title)
{
    auto* saved = params.ResolveSaveParamsAt08();
    const auto& input = title ? saved->saveTitleAt08 : saved->saveDescriptionAt04;
    PublishText(output, CloneRuntimeCStringDataAt414540(Header(input.GetString())));
}

SaveFileNativeTime* CreateTimeAfterRead(ScriptParamsNativeBody& params, IEnvironmentRootMemory& memory)
{
    void* raw = memory.AllocateBody(8);
    if (!raw) return nullptr;
    auto* time = ::new (raw) SaveFileNativeTime;
    // 48E737/48E9E8/48EC7D：new8回调返回后才读CParams+00/+04，不能先取值快照。
    time->lowAt00 = params.timestampLowAt00;
    time->highAt04 = params.timestampHighAt04;
    return time;
}

std::int32_t Increment32(std::int32_t value) noexcept
{
    return std::bit_cast<std::int32_t>(std::uint32_t(value) + 1u);
}

void PublishSuccessfulItem(ScriptParamsNativeBody& params, std::int32_t page, std::int32_t item,
    ISaveFileCatalogItemNativeSource& source, IEnvironmentRootMemory& memory)
{
    auto* time = CreateTimeAfterRead(params, memory);
    // 原new8无失败回收资格；捕获或control分配异常可留下孤立/部分发布正文。
    auto* target = CaptureItem(source, page, item);
    ReplaceSaveFileTimeAt470A6E(target->timeAt00, time, memory); // 4518A0同核。

    TextSlot label;
    CloneParamsText(label.value, params, false);
    TextUnwind labelUnwind{label.value};
    const char* cachedLabel = label.value.GetString();
    if (Header(cachedLabel)->nDataLength == 0)
    {
        TextSlot title;
        CloneParamsText(title.value, params, true);
        TextUnwind titleUnwind{title.value};
        AssignRuntimeCStringAt410220(label.value, title.value);
        titleUnwind.FinishCurrent();
        cachedLabel = label.value.GetString();
    }
    // 时间、label、count各自重新捕获，可来自三个不同Env，不能缓存一个catalog。
    auto* labelTarget = CaptureItem(source, page, item);
    AssignRuntimeCStringAt410220(labelTarget->labelAt0C, label.value);
    UpdateSaveFilePageCountAt48EFE0(*CapturePage(source, page));
    if (page >= 0) static_cast<void>(RefreshSaveFileSelectionNativeAt48D290(source));
    // quick/regular正常尾用缓存ESI；auto在48EB56重读局部TEXT。EH始终读当前槽。
    if (page == -2) labelUnwind.FinishCurrent();
    else labelUnwind.FinishCached(cachedLabel);
}
}

void RefreshSaveFileCatalogItemNativeAt48E5F0(std::int32_t page, std::int32_t item,
    ISaveFileCatalogItemNativeSource& source, IEnvironmentRootMemory& memory)
{
    TextSlot path;
    InitializeEmptyRuntimeCString(path.value, source.Strings());
    TextUnwind pathUnwind{path.value};
    const char* cachedPath = path.value.GetString(); // 48E636的EDI，包括其它负page。
    {
        ScriptParamsNativeBody params(memory, source.Strings());
        if (page == -1 || page == -2 || page >= 0)
        {
            TextSlot format;
            auto& directory = CaptureDirectory(source);
            ConstructDirectoryFormat(format.value, directory,
                page == -1 ? "qsave%02d" : page == -2 ? "asave%02d" : "save%04d_%02d",
                page < 0 ? 9u : 13u, source);
            TextUnwind formatUnwind{format.value};
            AppendRuntimeCStringAt413550(format.value, ".dat", 4);
            const char* cachedFormat = format.value.GetString();
            if (page >= 0)
            {
                FormatRuntimeCStringAt417130(path.value, cachedFormat,
                    SaveFilePhysicalPageNumber(*CaptureCatalog(source), page), Increment32(item));
                // 48EC36先撤格式串资格；其Free失败时尚未进入Read。
                formatUnwind.FinishCached(cachedFormat);
            }
            else FormatRuntimeCStringAt417130(path.value, cachedFormat, Increment32(item));

            cachedPath = path.value.GetString();
            const auto result = source.ReadContainerAt4C7420(params, cachedPath, 1u);
            if (static_cast<std::uint8_t>(result) != 0)
                PublishSuccessfulItem(params, page, item, source, memory);
            // AL=0不清旧time/label、不计数、不写latest；只清局部对象。
            if (page == -1) formatUnwind.FinishCurrent();
            else if (page == -2) formatUnwind.FinishCached(cachedFormat);
        }
    } // 48ED97先463490销毁70h CParams；EH亦在path之前。
    pathUnwind.FinishCached(cachedPath); // 48EDA3缓存EDI，不重试已撤资格的Free。
}

void RefreshSaveFileCatalogItemNativeAt48EE00(std::int32_t page, std::int32_t item,
    ISaveFileCatalogItemNativeSource& source, IEnvironmentRootMemory& memory)
{
    RefreshSaveFileCatalogItemNativeAt48E5F0(page, item, source, memory);
}
}
