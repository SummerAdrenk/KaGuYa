#include <Runtime/SaveFileCatalog.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>
#include <Runtime/RuntimeCStringConcatenation.h>
#include <Runtime/RuntimeCStringResult.h>
#include <Runtime/SaveLoadResourceProfile.h>

#include <algorithm>
#include <bit>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
class LocalReference final
{
public:
    ~LocalReference() noexcept { ReleaseYtlReference(value); }
    void ReleaseCached(YtlReferenceControlRuntime* control) noexcept
        { ReleaseSaveFileCachedReferenceAt470D50(value, control); }
    YtlReferenceSlotRuntime value;
};

std::int32_t Latest(const YtlReferenceSlotRuntime& page) noexcept
{
    if (page.controlAt08) ++page.controlAt08->referenceCountAt04;
    return FindLatestSaveFileItemAt48D090(page);
}

void SetRegularPattern(RuntimeCString& pattern, ISaveFileCatalogScanSource& source)
{
    // 4707E3..4708AC：目录属于格式串，而非格式化后的字符串前缀。
    {
        const RuntimeCStringConcatenation joined(source.CaptureSaveDirectoryAt4180C0(),
            "save%04d_%02d", 13);
        AssignRuntimeCStringAt410220(pattern, joined.Value());
    }
    AppendRuntimeCStringAt413550(pattern, ".dat", 4);
}

void SetLinearPattern(RuntimeCString& pattern, ISaveFileCatalogScanSource& source, const char* suffix)
{
    // 4713FD/4719DD各自重新取路径；466760在GetNil成功之后测suffix长度。
    {
        const RuntimeCStringConcatenation joined(source.CaptureSaveDirectoryAt4180C0(), suffix);
        AssignRuntimeCStringAt410220(pattern, joined.Value());
    }
    AppendRuntimeCStringAt413550(pattern, ".dat", 4);
}

class FixedScanSource final : public ISaveFileCatalogScanSource
{
public:
    FixedScanSource(SaveFileCatalog& catalog, RuntimeStringBorrow directory) noexcept
        : catalog_(catalog), directory_(directory) {}
    SaveFileStateNativeArchive& CaptureSaveFileStateAt4180C0() override { return catalog_; }
    RuntimeStringBorrow CaptureSaveDirectoryAt4180C0() override { return directory_; }
private:
    SaveFileCatalog& catalog_;
    RuntimeStringBorrow directory_;
};

void EmptyPage(LocalReference& page, int number, int slots)
{
    AdoptSaveFilePageAt4708F0(page.value, CreateSaveFilePageAt4708C7());
    auto* body = ResolveEnvironmentObjectReference<SaveFileNativePage>(page.value);
    body->physicalPageNumber = number;
    body->availabilityAt00 = 0;
    for (int i = 0; i < slots; ++i)
    {
        LocalReference item;
        AdoptSaveFileItemAt4709B5(item.value, CreateSaveFileItemAt470961());
        AppendEnvironmentArchiveReferenceAt49852D(body->itemsAt04, item.value);
    }
}
}

bool ISaveFileMetadataLoader::HasMatchingFiles(const std::string& pattern)
{
    WIN32_FIND_DATAA data;
    const auto handle = ::FindFirstFileA(pattern.c_str(), &data);
    if (handle == INVALID_HANDLE_VALUE) return false;
    if (!::FindClose(handle)) throw std::runtime_error("save catalog FindClose failed");
    return true;
}

bool ISaveFileMetadataLoader::TryReadFileWriteTime(const std::string& path, std::uint64_t& seconds)
{
    WIN32_FIND_DATAA data;
    const auto handle = ::FindFirstFileA(path.c_str(), &data);
    if (handle == INVALID_HANDLE_VALUE) return false;
    if (!::FindClose(handle)) throw std::runtime_error("save catalog FindClose failed");
    const auto ticks = (std::uint64_t(data.ftLastWriteTime.dwHighDateTime) << 32u) |
        data.ftLastWriteTime.dwLowDateTime;
    // 原CTime比较以秒为粒度，相同秒保留先找到的槽，不以100ns子秒打破平局。
    constexpr std::uint64_t unixEpoch = 116444736000000000ull;
    if (ticks < unixEpoch) throw std::runtime_error("save file timestamp predates supported CTime range");
    seconds = (ticks - unixEpoch) / 10000000u;
    return true;
}

int SaveFilePhysicalPageNumber(const SaveFileCatalog& catalog, int displayIndex)
{
    const auto& page = catalog.RegularPages().at(static_cast<std::size_t>(displayIndex));
    if (page.physicalPageNumber < 1 ||
        page.physicalPageNumber > CurrentSaveLoadResourceProfile().catalog.physicalPageLimit)
        throw std::logic_error("save catalog has no valid physical page number");
    return page.physicalPageNumber;
}

int AddSaveFileCatalogPage(SaveFileCatalog& catalog)
{
    const auto& policy = CurrentSaveLoadResourceProfile().catalog;
    if (policy.kind != SaveLoadCatalogKind::ExistingPhysicalPages)
        throw std::logic_error("fixed save catalog cannot insert a page");
    const auto count = catalog.RegularPages().size();
    if (count >= static_cast<std::size_t>(policy.physicalPageLimit)) return -1;
    int number = count ? catalog.RegularPages().back().physicalPageNumber : 0;
    do
    {
        if (++number > policy.physicalPageLimit) number = 1;
    } while (std::any_of(catalog.RegularPages().begin(), catalog.RegularPages().end(),
        [number](const auto& page) { return page.physicalPageNumber == number; }));
    int position = 0;
    while (position < static_cast<int>(count) && catalog.RegularPages()[position].physicalPageNumber < number)
        ++position;
    LocalReference page;
    EmptyPage(page, number, policy.slotsPerPage);
    AppendEnvironmentArchiveReferenceAt49852D(catalog.regularPagesAt20, page.value);
    auto& pages = catalog.regularPagesAt20;
    std::rotate(pages.begin + position, pages.end - 1, pages.end);
    if (catalog.regularLatestPageAt18 >= position) ++catalog.regularLatestPageAt18;
    return position;
}

int SaveFileRollingSlot(const SaveFileCatalog& catalog, bool automatic, int displaySlot)
{
    const auto& policy = CurrentSaveLoadResourceProfile().catalog;
    if (displaySlot < 0 || displaySlot >= policy.slotsPerPage)
        throw std::out_of_range("save display slot outside profile capacity");
    const auto latest = automatic ? catalog.autoLatestItemAt08 : catalog.quickLatestItemAt04;
    if (!policy.rollingNewestFirst || latest < 0) return displaySlot;
    if (latest >= policy.slotsPerPage) throw std::logic_error("rolling latest outside profile capacity");
    return (latest + policy.slotsPerPage - displaySlot) % policy.slotsPerPage;
}

void RemoveSaveFileCatalogPage(SaveFileCatalog& catalog, int displayIndex)
{
    const auto& policy = CurrentSaveLoadResourceProfile().catalog;
    if (policy.kind != SaveLoadCatalogKind::ExistingPhysicalPages)
        throw std::logic_error("fixed save catalog cannot erase a page");
    (void)catalog.RegularPages().at(static_cast<std::size_t>(displayIndex));
    // 先准备空目录保底页；分配失败不发布一个无页可用的目录。
    LocalReference replacement;
    if (catalog.RegularPages().size() == 1) EmptyPage(replacement, 1, policy.slotsPerPage);
    auto& pages = catalog.regularPagesAt20;
    ReleaseYtlReference(pages.begin[displayIndex]);
    std::move(pages.begin + displayIndex + 1, pages.end, pages.begin + displayIndex);
    --pages.end;
    *pages.end = {};
    if (replacement.value.baseAt00) AppendEnvironmentArchiveReferenceAt49852D(pages, replacement.value);
    const auto last = static_cast<int>(catalog.RegularPages().size()) - 1;
    catalog.regularSelectedPageAt0C = std::clamp(catalog.regularSelectedPageAt0C, 0, last);
    catalog.regularDisplayedPageAt10 = std::clamp(catalog.regularDisplayedPageAt10, 0, last);
    if (catalog.regularLatestPageAt18 == displayIndex)
    { catalog.regularLatestPageAt18 = -1; catalog.regularLatestItemAt1C = -1; }
    else if (catalog.regularLatestPageAt18 > displayIndex) --catalog.regularLatestPageAt18;
}

SaveFileCatalogScanner::SaveFileCatalogScanner(ISaveFileMetadataLoader& loader) noexcept
    : loader_(loader)
{
}

bool SaveFileCatalogScanner::LoadItem(SaveFileNativeItem& item, const std::string& path)
{
    SaveFileMetadata metadata;
    if (!loader_.TryLoad(path, metadata))
        return false;
    const SaveFileNativeTime source{static_cast<std::uint32_t>(metadata.lastWriteTime),
        static_cast<std::uint32_t>(metadata.lastWriteTime >> 32u)};
    auto* time = CreateSaveFileTimeAt470A40(source);
    ReplaceSaveFileTimeAt470A6E(item.timeAt00, time);
    // 470B3F/470C08：先取 CSaveParams+4，空时才取 +8，不按磁盘字符串顺序。
    const auto& label = metadata.primaryLabel.empty() ? metadata.secondaryLabel : metadata.primaryLabel;
    SetRuntimeCStringAt437680(item.labelAt0C, label.data(), static_cast<int>(label.size()));
    return true;
}

std::unique_ptr<SaveFileCatalog> SaveFileCatalogScanner::Scan(RuntimeStringBorrow directoryPrefix)
{
    void* raw = ::operator new(sizeof(SaveFileCatalog));
    std::unique_ptr<SaveFileCatalog> catalog(::new (raw) SaveFileCatalog);
    SaveFileCatalogScanState state;
    ScanInto(*catalog, state, directoryPrefix);
    return catalog;
}

void SaveFileCatalogScanner::ScanInto(SaveFileCatalog& catalog,
    SaveFileCatalogScanState& state, RuntimeStringBorrow directoryPrefix)
{
    FixedScanSource fixed(catalog, directoryPrefix);
    ScanFrom(fixed, &state);
}

void SaveFileCatalogScanner::ScanFrom(ISaveFileCatalogScanSource& source)
{
    ScanFrom(source, nullptr);
}

void SaveFileCatalogScanner::ScanFrom(ISaveFileCatalogScanSource& source,
    SaveFileCatalogScanState* fixedDiagnostics)
{
    // 诊断只用于显式固定正文接口；生产源可在任意原getter处换根，不能混记资格。
    const auto& policy = CurrentSaveLoadResourceProfile().catalog;
    const auto itemsPerPage = static_cast<std::uint32_t>(policy.slotsPerPage);
    const auto regularPageLimit = static_cast<std::uint32_t>(policy.physicalPageLimit);
    const bool sparse = policy.kind == SaveLoadCatalogKind::ExistingPhysicalPages;
    if (fixedDiagnostics) fixedDiagnostics->complete = false;
    RuntimeCStringResult format([](RuntimeCString* out) { InitializeEmptyRuntimeCString(*out); });
    RuntimeCStringResult path([](RuntimeCString* out) { InitializeEmptyRuntimeCString(*out); });
    // 470724..4707D7：先清普通页，再写 +1C/+18；其他标量尚不写入。
    ClearEnvironmentArchiveReferencesAt498312(source.CaptureSaveFileStateAt4180C0().regularPagesAt20);
    source.CaptureSaveFileStateAt4180C0().regularLatestItemAt1C = -1;
    source.CaptureSaveFileStateAt4180C0().regularLatestPageAt18 = -1;
    if (fixedDiagnostics) fixedDiagnostics->regularLatestWritten = true;
    std::int64_t latestRegularTime = 0;
    SetRegularPattern(format.Value(), source);
    RuntimeCStringResult matchFormat([](RuntimeCString* out) { InitializeEmptyRuntimeCString(*out); });
    if (sparse)
    {
        const RuntimeCStringConcatenation joined(source.CaptureSaveDirectoryAt4180C0(), policy.regularPageMatchPattern);
        AssignRuntimeCStringAt410220(matchFormat.Value(), joined.Value());
    }
    for (std::uint32_t pageNumber = 1; pageNumber <= regularPageLimit; ++pageNumber)
    {
        if (sparse)
        {
            FormatRuntimeCStringAt417130(path.Value(), matchFormat.Value().GetString(), pageNumber);
            if (!loader_.HasMatchingFiles(std::string(RuntimeStringView(path.Value())))) continue;
        }
        // 4708C7：普通页局部Ref，整页成功前不发布到Environment。
        LocalReference page;
        AdoptSaveFilePageAt4708F0(page.value, CreateSaveFilePageAt4708C7());
        auto* pageBody = ResolveEnvironmentObjectReference<SaveFileNativePage>(page.value);
        pageBody->availabilityAt00 = 0;
        pageBody->physicalPageNumber = static_cast<int>(pageNumber);
        for (std::uint32_t itemNumber = 1; itemNumber <= itemsPerPage; ++itemNumber)
        {
            LocalReference item;
            AdoptSaveFileItemAt4709B5(item.value, CreateSaveFileItemAt470961());
            auto* itemBody = ResolveEnvironmentObjectReference<SaveFileNativeItem>(item.value);
            FormatRuntimeCStringAt417130(path.Value(), format.Value().GetString(), pageNumber, itemNumber);
            if (LoadItem(*itemBody, std::string(RuntimeStringView(path.Value()))))
                ++pageBody->availabilityAt00;
            item.ReleaseCached(AppendSaveFileReferenceAt470D1A(pageBody->itemsAt04,
                item.value, item.value.controlAt08));
        }

        // 470EAD..470F26：整页追加后才查询 latest，未完成的普通页仍是局部对象。
        auto* cachedPage = AppendSaveFileReferenceAt470D1A(source.CaptureSaveFileStateAt4180C0().regularPagesAt20,
            page.value, page.value.controlAt08);
        const std::int32_t latestItem = Latest(page.value);
        if (latestItem >= 0)
        {
            // 47107D的判定、471252和4712A9两次写入分别属于三个独立根捕获。
            if (source.CaptureSaveFileStateAt4180C0().regularLatestPageAt18 < 0 ||
                std::bit_cast<std::int64_t>(pageBody->Items()[static_cast<std::size_t>(latestItem)].LastWriteTime()) > latestRegularTime)
            {
                source.CaptureSaveFileStateAt4180C0().regularLatestPageAt18 = sparse ?
                    static_cast<std::int32_t>(source.CaptureSaveFileStateAt4180C0().RegularPages().size()) - 1 :
                    static_cast<std::int32_t>(pageNumber - 1);
                source.CaptureSaveFileStateAt4180C0().regularLatestItemAt1C = latestItem;
                latestRegularTime = std::bit_cast<std::int64_t>(
                    pageBody->Items()[static_cast<std::size_t>(latestItem)].LastWriteTime());
            }
        }
        page.ReleaseCached(cachedPage);
    }
    if (sparse && source.CaptureSaveFileStateAt4180C0().RegularPages().empty())
    {
        LocalReference page;
        EmptyPage(page, 1, policy.slotsPerPage);
        AppendEnvironmentArchiveReferenceAt49852D(source.CaptureSaveFileStateAt4180C0().regularPagesAt20, page.value);
    }

    // 471398/4713A9/4713BA：先 +10，再 +0C，最后 +14。
    const auto displayed = source.CaptureSaveFileStateAt4180C0().regularLatestPageAt18 >= 0
        ? source.CaptureSaveFileStateAt4180C0().regularLatestPageAt18 : 0;
    source.CaptureSaveFileStateAt4180C0().regularDisplayedPageAt10 = displayed;
    source.CaptureSaveFileStateAt4180C0().regularSelectedPageAt0C = displayed;
    source.CaptureSaveFileStateAt4180C0().regularPageStepAt14 = 1;
    if (fixedDiagnostics) fixedDiagnostics->regularSelectionWritten = true;

    // 4713F5/4719D5：quick/auto 页分别替换，不在扫描入口一起清空。
    // 分配发生在取slot6之前，不能依赖函数实参的未指定求值顺序。
    auto* quick = CreateSaveFilePageAt4708C7();
    if (sparse) quick->availabilityAt00 = 0;
    ReplaceSaveFilePageAt451C60(source.CaptureSaveFileStateAt4180C0().quickPageAt2C, quick);
    if (fixedDiagnostics) fixedDiagnostics->quickPageCreated = true;
    SetLinearPattern(format.Value(), source, "qsave%02d");
    std::int32_t quickFileLatest = -1;
    std::uint64_t quickFileTime = 0;
    for (std::uint32_t itemNumber = 1; itemNumber <= itemsPerPage; ++itemNumber)
    {
        LocalReference item;
        AdoptSaveFileItemAt4709B5(item.value, CreateSaveFileItemAt470961());
        FormatRuntimeCStringAt417130(path.Value(), format.Value().GetString(), itemNumber);
        if (policy.rollingLatestFromFileTime)
        {
            std::uint64_t time;
            if (loader_.TryReadFileWriteTime(std::string(RuntimeStringView(path.Value())), time) &&
                (quickFileLatest < 0 || time > quickFileTime))
            { quickFileLatest = static_cast<int>(itemNumber) - 1; quickFileTime = time; }
        }
        if (LoadItem(*ResolveEnvironmentObjectReference<SaveFileNativeItem>(item.value),
            std::string(RuntimeStringView(path.Value()))))
            source.CaptureSaveFileStateAt4180C0().QuickPage().availabilityAt00 = 1;
        item.ReleaseCached(AppendSaveFileReferenceAt470D1A(source.CaptureSaveFileStateAt4180C0().QuickPage().itemsAt04,
            item.value, item.value.controlAt08));
    }
    const auto quickLatest = policy.rollingLatestFromFileTime ? quickFileLatest :
        Latest(source.CaptureSaveFileStateAt4180C0().quickPageAt2C);
    source.CaptureSaveFileStateAt4180C0().quickLatestItemAt04 = quickLatest;
    if (fixedDiagnostics) fixedDiagnostics->quickLatestWritten = true;

    auto* automatic = CreateSaveFilePageAt4708C7();
    if (sparse) automatic->availabilityAt00 = 0;
    ReplaceSaveFilePageAt451C60(source.CaptureSaveFileStateAt4180C0().autoPageAt38, automatic);
    if (fixedDiagnostics) fixedDiagnostics->autoPageCreated = true;
    SetLinearPattern(format.Value(), source, "asave%02d");
    std::int32_t autoFileLatest = -1;
    std::uint64_t autoFileTime = 0;
    for (std::uint32_t itemNumber = 1; itemNumber <= itemsPerPage; ++itemNumber)
    {
        LocalReference item;
        AdoptSaveFileItemAt4709B5(item.value, CreateSaveFileItemAt470961());
        FormatRuntimeCStringAt417130(path.Value(), format.Value().GetString(), itemNumber);
        if (policy.rollingLatestFromFileTime)
        {
            std::uint64_t time;
            if (loader_.TryReadFileWriteTime(std::string(RuntimeStringView(path.Value())), time) &&
                (autoFileLatest < 0 || time > autoFileTime))
            { autoFileLatest = static_cast<int>(itemNumber) - 1; autoFileTime = time; }
        }
        if (LoadItem(*ResolveEnvironmentObjectReference<SaveFileNativeItem>(item.value),
            std::string(RuntimeStringView(path.Value()))))
            source.CaptureSaveFileStateAt4180C0().AutoPage().availabilityAt00 = 1;
        item.ReleaseCached(AppendSaveFileReferenceAt470D1A(source.CaptureSaveFileStateAt4180C0().AutoPage().itemsAt04,
            item.value, item.value.controlAt08));
    }
    const auto autoLatest = policy.rollingLatestFromFileTime ? autoFileLatest :
        Latest(source.CaptureSaveFileStateAt4180C0().autoPageAt38);
    source.CaptureSaveFileStateAt4180C0().autoLatestItemAt08 = autoLatest;
    if (fixedDiagnostics)
    {
        fixedDiagnostics->autoLatestWritten = true;
        fixedDiagnostics->complete = true;
    }
}
}
