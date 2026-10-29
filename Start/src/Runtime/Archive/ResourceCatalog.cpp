#include <Runtime/ResourceCatalog.h>
#include <Runtime/ArchiveDirectoryRuntime.h>
#include <Runtime/ArchiverInfoRuntime.h>
#include <Runtime/ArchiveResourceNameRuntime.h>

#include <mbstring.h>

#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
bool ResourceReadQuery::CopyNativeWordAt08(std::int32_t& destination) const noexcept
{
    if (!state_) return false;
    // 4D4A76/484302：只转交当前四字节表示；不能提前取数或替换成archiveVersion。
    std::memcpy(&destination, state_->unknownAt08, sizeof(destination));
    return true;
}

namespace
{
using Byte = unsigned char;

[[nodiscard]] const Byte* Bytes(const std::string& value) noexcept
{
    return reinterpret_cast<const Byte*>(value.c_str());
}

[[nodiscard]] bool EqualCaseSensitive(
    const std::string& first,
    const std::string& second) noexcept
{
    return _mbscmp(Bytes(first), Bytes(second)) == 0;
}

[[nodiscard]] bool EqualCaseInsensitive(
    const std::string& first,
    const std::string& second) noexcept
{
    return _mbsicmp(Bytes(first), Bytes(second)) == 0;
}

[[nodiscard]] std::size_t ByteOffset(
    const std::string& value,
    const Byte* const position) noexcept
{
    return static_cast<std::size_t>(position - Bytes(value));
}

[[nodiscard]] const ResourceFileEntry* ResolveExplicitPath(
    const ResourceFolderEntry& folder,
    const std::string& relativePath)
{
    const Byte* const separator = _mbschr(Bytes(relativePath), '\\');
    if (separator == nullptr)
    {
        for (const ResourceFileEntry& file : folder.files)
        {
            if (EqualCaseInsensitive(file.name, relativePath))
                return &file;
        }
        return nullptr;
    }

    const std::size_t separatorOffset = ByteOffset(relativePath, separator);
    const std::string folderName = relativePath.substr(0, separatorOffset);
    const std::string remainder = relativePath.substr(separatorOffset + 1);
    for (const ResourceFolderEntry& child : folder.folders)
    {
        if (EqualCaseInsensitive(child.name, folderName))
            return ResolveExplicitPath(child, remainder);
    }
    return nullptr;
}

[[nodiscard]] const ArchiveFileNodeRuntime* ResolveExplicitPath(
    const ArchiveDirectoryRuntime& directory,
    const std::string& relativePath)
{
    const Byte* const separator = _mbschr(Bytes(relativePath), '\\');
    if (!separator)
    {
        for (const auto* cursor = directory.beginAt00; cursor != directory.endAt04; ++cursor)
        {
            if (!ResolveArchiveNode(*cursor)->IsFile()) continue;
            const auto* node = ResolveArchiveNode(*cursor);
            if (_mbsicmp(reinterpret_cast<const Byte*>(node->NameAt04().GetString()),
                Bytes(relativePath)) == 0)
                return static_cast<const ArchiveFileNodeRuntime*>(ResolveArchiveNode(*cursor));
        }
        return nullptr;
    }

    const auto separatorOffset = ByteOffset(relativePath, separator);
    const std::string folderName = relativePath.substr(0, separatorOffset);
    for (const auto* cursor = directory.beginAt00; cursor != directory.endAt04; ++cursor)
    {
        if (ResolveArchiveNode(*cursor)->IsFile()) continue;
        const auto* node = ResolveArchiveNode(*cursor);
        if (_mbsicmp(reinterpret_cast<const Byte*>(node->NameAt04().GetString()),
            Bytes(folderName)) != 0) continue;
        // 43D06B..43D0B4：命中第一个同名目录后才构造余串，失败不回退到后续同名目录。
        const std::string remainder = relativePath.substr(separatorOffset + 1);
        const auto* folder = static_cast<const ArchiveFolderNodeRuntime*>(ResolveArchiveNode(*cursor));
        return ResolveExplicitPath(folder->ChildrenAt08(), remainder);
    }
    return nullptr;
}

[[nodiscard]] const ArchiveFileNodeRuntime* ResolveRootTitle(
    const ArchiveDirectoryRuntime& directory, const std::string& title)
{
    for (const auto* cursor = directory.beginAt00; cursor != directory.endAt04; ++cursor)
    {
        const auto* node = ResolveArchiveNode(*cursor);
        // 416F77..416F83没有IsFile：直接将正文+08当作char*，不能跳过folder或补空值。
        const char* candidate;
        std::memcpy(&candidate, reinterpret_cast<const std::byte*>(node) + 0x08, sizeof(candidate));
        if (_mbsicmp(reinterpret_cast<const Byte*>(candidate), Bytes(title)) == 0)
            return reinterpret_cast<const ArchiveFileNodeRuntime*>(ResolveArchiveNode(*cursor));
    }
    return nullptr;
}

struct NativeQueryMatch final
{
    const ArchiveFileNodeRuntime* file;
    const ArchiveQueryState* state;
};

[[nodiscard]] std::optional<NativeQueryMatch> ResolveNativeExplicitPath(
    ArchiverInfoRuntime& owner, const ArchiveDirectoryRuntime& directory, const char* relativePath,
    IRuntimeStringManagerSource& strings)
{
    // 43CF1B先捕获原char*，40B530再构造本层CString；无目录分支比较的是前者。
    auto capturedPath = ArchiveResourceNameAt40B530(relativePath, strings);
    const RuntimeCString& path = capturedPath.Value();
    const auto* first = reinterpret_cast<const Byte*>(path.GetString());
    const Byte* const slash = path.GetLength() > 0 ? _mbschr(first, '\\') : nullptr;
    if (slash)
    {
        const int offset = static_cast<int>(slash - first);
        auto folderName = ArchiveResourceNameLeftAt419D00(path, offset, strings);
        const char* const folderCharacters = folderName.Value().GetString();
        for (auto* cursor = directory.beginAt00; cursor != directory.endAt04; ++cursor)
        {
            if (ResolveArchiveNode(*cursor)->IsFile()) continue;
            const auto* node = ResolveArchiveNode(*cursor);
            if (!folderCharacters) throw ATL::CAtlException(E_FAIL); // 43CFF5。
            if (_mbsicmp(reinterpret_cast<const Byte*>(node->NameAt04().GetString()),
                reinterpret_cast<const Byte*>(folderCharacters)) != 0) continue;
            // 43D06B：命中后才分配余串，且分配回调结束后重解当前目录Ref。
            auto tail = ArchiveResourceNameMidAt420C80(path, offset + 1,
                path.GetLength() - offset - 1, strings);
            const auto* folder = static_cast<const ArchiveFolderNodeRuntime*>(ResolveArchiveNode(*cursor));
            const char* const tailCharacters = tail.Value().GetString();
            const auto result = ResolveNativeExplicitPath(owner, folder->ChildrenAt08(), tailCharacters, strings);
            // 43D0BB..43D052：先发布结果，再按缓存余串/目录名、当前路径释放。
            // 每次正常Free前撤销该槽的展开资格；失败时只清理尚未释放的前缀。
            tail.ReleaseNormalCached(tailCharacters);
            folderName.ReleaseNormalCached(folderCharacters);
            capturedPath.ReleaseNormal();
            return result;
        }
        folderName.ReleaseNormalCached(folderCharacters);
        capturedPath.ReleaseNormal();
        return std::nullopt;
    }
    for (auto* cursor = directory.beginAt00; cursor != directory.endAt04; ++cursor)
    {
        if (!ResolveArchiveNode(*cursor)->IsFile()) continue;
        const auto* node = ResolveArchiveNode(*cursor);
        if (!relativePath) throw ATL::CAtlException(E_FAIL); // 43D158，仅检查Str2。
        if (_mbsicmp(reinterpret_cast<const Byte*>(node->NameAt04().GetString()),
            reinterpret_cast<const Byte*>(relativePath)) != 0) continue;
        const auto* file = static_cast<const ArchiveFileNodeRuntime*>(ResolveArchiveNode(*cursor));
        // 43D17D..43D219：结果先写+38，随后才释放各层CString，不能将发布延后到Locate外。
        const NativeQueryMatch result{file, &owner.PublishQueryAt38(*file)};
        capturedPath.ReleaseNormal();
        return result;
    }
    capturedPath.ReleaseNormal();
    return std::nullopt;
}

[[nodiscard]] std::optional<NativeQueryMatch> ResolveNativeRootTitle(
    ArchiverInfoRuntime& owner, const char* title)
{
    const auto& directory = owner.DirectoryAt04();
    for (auto* cursor = directory.beginAt00; cursor != directory.endAt04; ++cursor)
    {
        const auto* node = ResolveArchiveNode(*cursor);
        const char* candidate;
        std::memcpy(&candidate, reinterpret_cast<const std::byte*>(node) + 0x08, sizeof(candidate));
        if (!title) throw ATL::CAtlException(E_FAIL); // 416F77，仅在比较点检查Str2。
        if (_mbsicmp(reinterpret_cast<const Byte*>(candidate),
            reinterpret_cast<const Byte*>(title)) != 0) continue;
        const auto* file = reinterpret_cast<const ArchiveFileNodeRuntime*>(ResolveArchiveNode(*cursor));
        return NativeQueryMatch{file, &owner.PublishQueryAt38(*file)};
    }
    return std::nullopt;
}

[[nodiscard]] std::string ExtractSuffix(const std::string& resourceName)
{
    const Byte* const dot = _mbsrchr(Bytes(resourceName), '.');
    if (dot == nullptr)
        return {};

    const std::size_t offset = ByteOffset(resourceName, dot);
    if (offset + 1 >= resourceName.size())
        return {};
    return resourceName.substr(offset + 1);
}

[[nodiscard]] std::string ExtractTitle(const std::string& resourceName)
{
    std::string title = resourceName;
    if (const Byte* const slash = _mbsrchr(Bytes(title), '\\'); slash != nullptr)
    {
        const std::size_t offset = ByteOffset(title, slash);
        if (offset + 1 < title.size())
            title = title.substr(offset + 1);
    }

    if (const Byte* const dot = _mbsrchr(Bytes(title), '.'); dot != nullptr)
    {
        const std::size_t offset = ByteOffset(title, dot);
        // 40C277..40C2C1：仅非空扩展名可取点前内容，其余返回空串。
        if (offset + 1 < title.size())
            return title.substr(0, offset);
    }
    return {};
}

[[nodiscard]] ResolvedResourceEntry CopyResolved(
    const ResourceArchiveEntry& archive,
    const ResourceFileEntry& file)
{
    return ResolvedResourceEntry{
        archive.archivePath,
        archive.archiveVersion,
        file.title,
        file.offset,
        file.length,
        file.flags};
}
}

ResourceCatalog::ResourceCatalog(ResourceCatalog&& source) noexcept
    : archives_(std::move(source.archives_)), nativeMap_(std::move(source.nativeMap_)),
      borrowedNativeMap_(std::exchange(source.borrowedNativeMap_, nullptr)),
      nativeFiles_(std::exchange(source.nativeFiles_, nullptr))
{
    source.nativeMap_.reset(); // 仅C++外层owner转移，节点地址及Ref计数不变。
}

ResourceCatalog& ResourceCatalog::operator=(ResourceCatalog&& source) noexcept
{
    if (this != &source)
    {
        nativeMap_ = std::move(source.nativeMap_);
        source.nativeMap_.reset();
        archives_ = std::move(source.archives_);
        borrowedNativeMap_ = std::exchange(source.borrowedNativeMap_, nullptr);
        nativeFiles_ = std::exchange(source.nativeFiles_, nullptr);
    }
    return *this;
}

ResourceCatalog ResourceCatalog::BorrowNativeMap(
    ArchiveNamespaceMapRuntime& map, IStartFileApi& files) noexcept
{
    ResourceCatalog result;
    result.borrowedNativeMap_ = &map;
    result.nativeFiles_ = &files;
    return result;
}

ArchiveNamespaceMapRuntime* ResourceCatalog::ActiveNativeMap() noexcept
{
    return borrowedNativeMap_ ? borrowedNativeMap_ : (nativeMap_ ? &*nativeMap_ : nullptr);
}

const ArchiveNamespaceMapRuntime* ResourceCatalog::ActiveNativeMap() const noexcept
{
    return borrowedNativeMap_ ? borrowedNativeMap_ : (nativeMap_ ? &*nativeMap_ : nullptr);
}

void ResourceCatalog::AddArchive(ResourceArchiveEntry archive)
{
    if (ActiveNativeMap()) throw std::logic_error("tool entry cannot be added to native namespace map");
    if (FindArchive(archive.nameSpace) != nullptr)
        throw std::invalid_argument("duplicate resource namespace");
    if (!archives_) archives_.emplace();
    archives_->push_back({std::move(archive)});
}

ResourceCatalog ResourceCatalog::CopyFixtureCatalog() const
{
    if (ActiveNativeMap()) throw std::logic_error("native namespace map has no fixture copy operation");
    ResourceCatalog result;
    result.archives_ = archives_;
    return result;
}

void ResourceCatalog::InitializeNativeMap(IStartFileApi& files)
{
    if (archives_ && !archives_->empty()) throw std::logic_error("native namespace map cannot adopt fixture entries");
    if (ActiveNativeMap())
    {
        if (nativeFiles_ != &files) throw std::logic_error("native namespace map file API changed");
        return;
    }
    nativeMap_.emplace();
    nativeFiles_ = &files;
}

ArchiveNamespaceMapRuntime& ResourceCatalog::NativeMap()
{
    auto* const map = ActiveNativeMap();
    if (!map) throw std::logic_error("native namespace map has not been constructed");
    return *map;
}

const ArchiveNamespaceMapRuntime& ResourceCatalog::NativeMap() const
{
    const auto* const map = ActiveNativeMap();
    if (!map) throw std::logic_error("native namespace map has not been constructed");
    return *map;
}

bool ResourceCatalog::HasNamespace(const RuntimeCString& name) const
{
    const auto& map = NativeMap();
    return map.Find(name) != map.End();
}

void ResourceCatalog::InsertRuntimeArchiveAt459293(const RuntimeCString& name,
    const YtlReferenceSlotRuntime& archive)
{
    // 456CC0的返回值在459298未参与分支；重复候选由树自己释放。
    (void)NativeMap().Insert(name, archive);
}

void ResourceCatalog::AddRuntimeArchive(std::string nameSpace,
    std::shared_ptr<ArchiverInfoRuntime> archive, IStartFileApi& files)
{
    if (ActiveNativeMap()) throw std::logic_error("fixture entry cannot be added to native namespace map");
    if (!archive) throw std::invalid_argument("runtime archive owner is null");
    // 显式fixture沿用判重协议，但不新建或接管任何原Ref control。
    if (FindArchive(nameSpace) != nullptr) throw 4;
    if (!archives_) archives_.emplace();
    archives_->push_back({RuntimeArchive{std::move(nameSpace), std::move(archive), &files}});
}

const std::string& ResourceCatalog::CatalogArchive::NameSpace() const
{
    if (const auto* tool = std::get_if<ResourceArchiveEntry>(&value)) return tool->nameSpace;
    return std::get<RuntimeArchive>(value).nameSpace;
}

ArchiverInfoRuntime* ResourceCatalog::RuntimeArchive::Get() const noexcept
{
    return owner.get();
}

const ArchiveQueryState* ResourceCatalog::LocatedResource::PublishQuery() const
{
    if (published) return published;
    if (runtime)
        return &runtime->PublishQueryAt38(*std::get<const ArchiveFileNodeRuntime*>(file));
    return nullptr;
}

ResolvedResourceEntry ResourceCatalog::LocatedResource::ExplicitSnapshot(const ArchiveQueryState* state) const
{
    if (state)
    {
        const auto* native = std::get<const ArchiveFileNodeRuntime*>(file);
        return {state->archivePath, state->archiveVersion, native->TitleAt08().GetString(),
            state->offset, state->length, state->flags};
    }
    return CopyResolved(std::get<ResourceArchiveEntry>(archive->value),
        *std::get<const ResourceFileEntry*>(file));
}

void ResourceCatalog::Clear() noexcept
{
    if (auto* const map = ActiveNativeMap()) map->Clear();
    if (archives_) archives_->clear();
}

std::size_t ResourceCatalog::ArchiveCount() const noexcept
{
    if (const auto* const map = ActiveNativeMap()) return map->size();
    return archives_ ? archives_->size() : 0;
}

std::vector<const ArchiverInfoRuntime*> ResourceCatalog::RuntimeArchivesInNamespaceOrder() const
{
    if (const auto* const map = ActiveNativeMap())
    {
        std::vector<const ArchiverInfoRuntime*> result;
        for (auto* node = map->Begin(); node != map->End();
            node = ArchiveNamespaceMapRuntime::Next(node))
            result.push_back(node->valueAt14.get());
        return result;
    }
    std::vector<const RuntimeArchive*> ordered;
    if (!archives_) return {};
    for (const auto& archive : *archives_)
    {
        if (const auto* runtime = std::get_if<RuntimeArchive>(&archive.value))
            ordered.push_back(runtime);
    }
    std::sort(ordered.begin(), ordered.end(), [](const auto* first, const auto* second) {
        return _mbscmp(Bytes(first->nameSpace), Bytes(second->nameSpace)) < 0;
    });
    std::vector<const ArchiverInfoRuntime*> result;
    result.reserve(ordered.size());
    for (const auto* archive : ordered) result.push_back(archive->Get());
    return result;
}

std::optional<ResourceCatalog::LocatedResource> ResourceCatalog::Locate(
    const std::string& resourceName) const
{
    if (ActiveNativeMap()) return LocateNative(resourceName.c_str());
    const Byte* const colon = _mbschr(Bytes(resourceName), ':');
    const auto colonOffset = colon ? ByteOffset(resourceName, colon) : 0;
    // 416CB1..416CD5/420C80：名称准备先于查map及属性探测，冒号后固定跳过两个字节。
    const std::string nameSpace = colon ? resourceName.substr(0, colonOffset) : ExtractSuffix(resourceName);
    const std::string name = colon ? (colonOffset + 2 < resourceName.size()
        ? resourceName.substr(colonOffset + 2) : std::string{}) : ExtractTitle(resourceName);
    const CatalogArchive* const stored = FindArchive(nameSpace);
    if (!stored) return std::nullopt;

    if (const auto* runtime = std::get_if<RuntimeArchive>(&stored->value))
    {
        // 416F07/416F44：只解析一次归档Ref，两条查询都先验证可用性，再访问同一正文。
        auto* owner = runtime->Get();
        if (!owner->IsAvailableAt43B2B0(*runtime->files)) return std::nullopt;
        const auto* file = colon ? ResolveExplicitPath(owner->DirectoryAt04(), name)
            : ResolveRootTitle(owner->DirectoryAt04(), name);
        if (!file) return std::nullopt;
        return LocatedResource{stored, file, owner};
    }

    const auto& archive = std::get<ResourceArchiveEntry>(stored->value);
    if (colon)
    {
        const auto* file = ResolveExplicitPath(archive.root, name);
        if (!file) return std::nullopt;
        return LocatedResource{stored, file};
    }
    for (const ResourceFileEntry& file : archive.root.files)
    {
        if (EqualCaseInsensitive(file.title, name))
            return LocatedResource{stored, &file};
    }
    return std::nullopt;
}

std::optional<ResourceCatalog::LocatedResource> ResourceCatalog::LocateNative(
    const char* resourceName, IRuntimeStringManagerSource& strings) const
{
    ArchiveQueryNameRuntime query(resourceName, strings);
    auto* const node = NativeMap().Find(query.Namespace());
    // 416ED7：查树后缓存name；属性探测、递归与正常释放均沿用这一指针。
    const char* const name = query.Name().GetString();
    std::optional<LocatedResource> result;
    if (node != NativeMap().End())
    {
        // 416EF2/416F44：只捕获一次正文；属性探测回调不重新解析map中的Ref。
        auto* const owner = node->valueAt14.get();
        if (owner->IsAvailableAt43B2B0(*nativeFiles_))
        {
            const auto found = query.IsExplicit()
                ? ResolveNativeExplicitPath(*owner, owner->DirectoryAt04(), name, strings)
                : ResolveNativeRootTitle(*owner, name);
            if (found) result.emplace(LocatedResource{nullptr, found->file, owner, found->state});
        }
    }
    query.ReleaseNormalAt416F9C(name);
    return result;
}

std::optional<ResolvedResourceEntry> ResourceCatalog::Resolve(const std::string& resourceName) const
{
    const auto found = Locate(resourceName);
    if (!found) return std::nullopt;
    // 兼容快照消费者也执行原成功查询的结果区写入，不能绕开对旧借用视图的影响。
    const auto* state = found->PublishQuery();
    return found->ExplicitSnapshot(state);
}

std::optional<ResourceReadQuery> ResourceCatalog::ResolveForRead(const std::string& resourceName) const
{
    const auto found = Locate(resourceName);
    if (!found) return std::nullopt;
    if (const auto* state = found->PublishQuery())
        return ResourceReadQuery(*state);
    return ResourceReadQuery(found->ExplicitSnapshot(nullptr));
}

std::optional<ResourceReadQuery> ResourceCatalog::ResolveForReadBorrowed(
    const char* resourceName, IRuntimeStringManagerSource& strings) const
{
    // 值目录是显式 fixture；只有此旧接口适配允许生成现代名称快照。
    if (!ActiveNativeMap()) return ResolveForRead(resourceName ? std::string(resourceName) : std::string{});
    const auto found = LocateNative(resourceName, strings);
    if (!found) return std::nullopt;
    return ResourceReadQuery(*found->PublishQuery());
}

const ResourceCatalog::CatalogArchive* ResourceCatalog::FindArchive(
    const std::string& nameSpace) const noexcept
{
    if (!archives_) return nullptr;
    for (const CatalogArchive& archive : *archives_)
    {
        if (EqualCaseSensitive(archive.NameSpace(), nameSpace))
            return &archive;
    }
    return nullptr;
}
}
