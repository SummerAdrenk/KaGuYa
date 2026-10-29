#pragma once

#include <Runtime/ResourceCatalog.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;

// 一次getter捕获的非拥有句柄；复制/移动不复制目录或Ref，析构不清理借用对象。
// native固定map/files，显式fixture固定catalog；所有被借对象须覆盖查询寿命。
class CapturedArchiveCatalog final
{
public:
    explicit CapturedArchiveCatalog(const ResourceCatalog& fixture) noexcept : fixture_(&fixture) {}
    CapturedArchiveCatalog(ArchiveNamespaceMapRuntime& map, IStartFileApi& files) noexcept
        : map_(&map), files_(&files) {}
    [[nodiscard]] std::size_t ArchiveCount() const noexcept;
    [[nodiscard]] std::optional<ResourceReadQuery> ResolveForRead(const std::string& name) const;
    [[nodiscard]] std::optional<ResourceReadQuery> ResolveForReadBorrowed(const char* name,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) const;
private:
    const ResourceCatalog* fixture_ = nullptr;
    ArchiveNamespaceMapRuntime* map_ = nullptr;
    IStartFileApi* files_ = nullptr;
};

// 483DCA/42BCD1：caller 到达原 getter 时点才取源，不在服务构造时缓存目录。
class IArchiveResourceSource
{
public:
    virtual ~IArchiveResourceSource() = default;
    [[nodiscard]] virtual CapturedArchiveCatalog CaptureCatalogAt413BE0() = 0;
};

class RootArchiveResourceSource final : public IArchiveResourceSource
{
public:
    explicit RootArchiveResourceSource(RootRuntimeOwner& root) noexcept : root_(root) {}
    [[nodiscard]] CapturedArchiveCatalog CaptureCatalogAt413BE0() override;
private:
    RootRuntimeOwner& root_;
};

// 显式工具/夹具入口；没有实际 root 时由调用方选择，不作为生产失败后的兜底。
class FixedArchiveResourceSource final : public IArchiveResourceSource
{
public:
    explicit FixedArchiveResourceSource(const ResourceCatalog& catalog) noexcept : catalog_(catalog) {}
    [[nodiscard]] CapturedArchiveCatalog CaptureCatalogAt413BE0() override
    { return CapturedArchiveCatalog(catalog_); }
private:
    const ResourceCatalog& catalog_;
};
}
