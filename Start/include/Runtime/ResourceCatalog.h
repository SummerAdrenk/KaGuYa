#pragma once

#include <Runtime/ArchiverInfoReference.h>
#include <Runtime/ArchiveNamespaceMapRuntime.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <variant>
#include <utility>

namespace KaGuYa::Start::Runtime
{
class ArchiverInfoRuntime;
class ArchiveFileNodeRuntime;
class IStartFileApi;
struct ResourceFileEntry final
{
    // fileinfo 同时保留完整文件名和无扩展名 title；无前缀查询只比较后者。
    std::string name;
    std::string title;
    std::uint64_t offset = 0;
    std::uint32_t length = 0;
    std::uint16_t flags = 0;
};

enum class ResourceChildKind : std::uint8_t
{
    Folder,
    File,
};

struct ResourceChildOrderEntry final
{
    ResourceChildKind kind = ResourceChildKind::File;
    std::size_t index = 0;
};

struct ResourceFolderEntry final
{
    std::string name;
    std::vector<ResourceFolderEntry> folders;
    std::vector<ResourceFileEntry> files;
    // 原 fileinfo 容器允许目录与文件交错；两个类型 vector 之外必须另存原始顺序。
    std::vector<ResourceChildOrderEntry> childOrder;
};

struct ResourceArchiveEntry final
{
    // namespace 使用原 InstallTable 名称，比较时区分大小写。
    std::string nameSpace;
    std::string archivePath;
    std::uint32_t archiveVersion = 0;
    ResourceFolderEntry root;
};

struct ResolvedResourceEntry final
{
    std::string archivePath;
    std::uint32_t archiveVersion = 0;
    // fileinfo+8的无扩展名title，仅作为目录元数据；不是CFileReader+10的逻辑引用。
    std::string title;
    std::uint64_t offset = 0;
    std::uint32_t length = 0;
    std::uint16_t flags = 0;
};

// ArchiverInfo+38 的已确认字段；R+08/+0C 的来源尚未确认，不补默认业务值。
#pragma pack(push, 4)
struct ArchiveQueryState final
{
    const char* archivePath;
    std::uint32_t archiveVersion;
    std::byte unknownAt08[8];
    std::uint64_t offset;
    std::uint32_t length;
    std::uint16_t flags;
    std::byte paddingAt1E[2];
};
#pragma pack(pop)
static_assert(sizeof(ArchiveQueryState) == 0x20);
static_assert(alignof(ArchiveQueryState) == 4);
static_assert(offsetof(ArchiveQueryState, archivePath) == 0x00);
static_assert(offsetof(ArchiveQueryState, archiveVersion) == 0x04);
static_assert(offsetof(ArchiveQueryState, unknownAt08) == 0x08);
static_assert(offsetof(ArchiveQueryState, offset) == 0x10);
static_assert(offsetof(ArchiveQueryState, length) == 0x18);
static_assert(offsetof(ArchiveQueryState, flags) == 0x1C);
static_assert(offsetof(ArchiveQueryState, paddingAt1E) == 0x1E);

// 生产视图借用 owner 的同一结果区；离线工具条目仍返回自持快照。
// 视图不增持归档，调用者必须保证目录/owner 在使用期间存活。
class ResourceReadQuery final
{
public:
    explicit ResourceReadQuery(ResolvedResourceEntry snapshot) : snapshot_(std::move(snapshot)) {}
    explicit ResourceReadQuery(const ArchiveQueryState& state) noexcept : state_(&state) {}
    [[nodiscard]] const char* Path() const noexcept { return state_ ? state_->archivePath : snapshot_.archivePath.c_str(); }
    [[nodiscard]] std::uint32_t Version() const noexcept { return state_ ? state_->archiveVersion : snapshot_.archiveVersion; }
    [[nodiscard]] std::uint64_t Offset() const noexcept { return state_ ? state_->offset : snapshot_.offset; }
    [[nodiscard]] std::uint32_t Length() const noexcept { return state_ ? state_->length : snapshot_.length; }
    [[nodiscard]] std::uint16_t Flags() const noexcept { return state_ ? state_->flags : snapshot_.flags; }
    // 原查询视图才有R+08；按字节转交，不给未命名/可能未初始化的槽补业务默认值。
    // 显式工具快照不含此原槽，返回false且不改目标；不是生产查询失败后的替代路径。
    bool CopyNativeWordAt08(std::int32_t& destination) const noexcept;
private:
    ResolvedResourceEntry snapshot_;
    const ArchiveQueryState* state_ = nullptr;
};

class ResourceCatalog final
{
public:
    ResourceCatalog() = default;
    ResourceCatalog(ResourceCatalog&&) noexcept;
    ResourceCatalog& operator=(ResourceCatalog&&) noexcept;
    ResourceCatalog(const ResourceCatalog&) = delete;
    ResourceCatalog& operator=(const ResourceCatalog&) = delete;
    // 非拥有访问外观：map/files 须保持地址稳定，且寿命覆盖所有借用。
    // 不分配内存或增持 Ref；析构绝不清理借用的树。
    // 移动转移借用关系；显式 Clear() 仍清理同一棵外部树。
    [[nodiscard]] static ResourceCatalog BorrowNativeMap(
        ArchiveNamespaceMapRuntime& map, IStartFileApi& files) noexcept;
    // 显式离线/fixture复制，不复制或共享生产manager的map。
    [[nodiscard]] ResourceCatalog CopyFixtureCatalog() const;
    void AddArchive(ResourceArchiveEntry archive);
    // 458BB5必须在manager构造catch之外；工具值目录不提前分配原sentinel。
    void InitializeNativeMap(IStartFileApi& files);
    [[nodiscard]] ArchiveNamespaceMapRuntime& NativeMap();
    [[nodiscard]] const ArchiveNamespaceMapRuntime& NativeMap() const;
    [[nodiscard]] bool HasNamespace(const RuntimeCString& name) const;
    // 调用者先完成459246的临时pair；本层不再复制一份临时Ref或提前判重。
    void InsertRuntimeArchiveAt459293(const RuntimeCString& name,
        const YtlReferenceSlotRuntime& archive);
    // 仅用于现有测试的explicit fixture，不作为生产正文的所有权入口。
    void AddRuntimeArchive(std::string nameSpace,
        std::shared_ptr<ArchiverInfoRuntime> archive, IStartFileApi& files);
    void Clear() noexcept;

    [[nodiscard]] std::size_t ArchiveCount() const noexcept;
    // 显式诊断快照；正式缓存写回直接遍历NativeMap，不经过本函数。
    [[nodiscard]] std::vector<const ArchiverInfoRuntime*> RuntimeArchivesInNamespaceOrder() const;
    [[nodiscard]] std::optional<ResolvedResourceEntry> Resolve(
        const std::string& resourceName) const;
    [[nodiscard]] std::optional<ResourceReadQuery> ResolveForRead(const std::string& resourceName) const;
    // 483DCA -> 416B90：进入名称解析前不复制或测长调用方捕获的 chars。
    [[nodiscard]] std::optional<ResourceReadQuery> ResolveForReadBorrowed(const char* resourceName,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) const;

private:
    struct RuntimeArchive final
    {
        std::string nameSpace;
        std::shared_ptr<ArchiverInfoRuntime> owner;
        IStartFileApi* files;
        [[nodiscard]] ArchiverInfoRuntime* Get() const noexcept;
    };
    struct CatalogArchive final
    {
        std::variant<ResourceArchiveEntry, RuntimeArchive> value;
        [[nodiscard]] const std::string& NameSpace() const;
    };
    struct LocatedResource final
    {
        const CatalogArchive* archive;
        std::variant<const ResourceFileEntry*, const ArchiveFileNodeRuntime*> file;
        ArchiverInfoRuntime* runtime = nullptr;
        const ArchiveQueryState* published = nullptr;
        [[nodiscard]] const ArchiveQueryState* PublishQuery() const;
        [[nodiscard]] ResolvedResourceEntry ExplicitSnapshot(const ArchiveQueryState* state) const;
    };
    [[nodiscard]] std::optional<LocatedResource> Locate(const std::string& resourceName) const;
    [[nodiscard]] std::optional<LocatedResource> LocateNative(const char* resourceName,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) const;
    [[nodiscard]] const CatalogArchive* FindArchive(
        const std::string& nameSpace) const noexcept;
    [[nodiscard]] ArchiveNamespaceMapRuntime* ActiveNativeMap() noexcept;
    [[nodiscard]] const ArchiveNamespaceMapRuntime* ActiveNativeMap() const noexcept;

    // 延迟构造工具夹具存储，也避免借用时分配调试版 STL vector 的代理对象。
    // 原生所有权始终属于 nativeMap_ 或外部拥有者，不落入工具夹具条目。
    std::optional<std::vector<CatalogArchive>> archives_;
    std::optional<ArchiveNamespaceMapRuntime> nativeMap_;
    ArchiveNamespaceMapRuntime* borrowedNativeMap_ = nullptr;
    IStartFileApi* nativeFiles_ = nullptr;
};
}
