#include <Runtime/RootArchiveResourceSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/FileManagerNative.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
std::size_t CapturedArchiveCatalog::ArchiveCount() const noexcept
{
    if (fixture_) return fixture_->ArchiveCount();
    return ResourceCatalog::BorrowNativeMap(*map_, *files_).ArchiveCount();
}
std::optional<ResourceReadQuery> CapturedArchiveCatalog::ResolveForRead(const std::string& name) const
{
    if (fixture_) return fixture_->ResolveForRead(name);
    return ResourceCatalog::BorrowNativeMap(*map_, *files_).ResolveForRead(name);
}
std::optional<ResourceReadQuery> CapturedArchiveCatalog::ResolveForReadBorrowed(const char* name,
    IRuntimeStringManagerSource& strings) const
{
    if (fixture_) return fixture_->ResolveForReadBorrowed(name, strings);
    return ResourceCatalog::BorrowNativeMap(*map_, *files_).ResolveForReadBorrowed(name, strings);
}

CapturedArchiveCatalog RootArchiveResourceSource::CaptureCatalogAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    auto* const root = root_.ReadPublishedAt6198C4();
    // 已捕获root的getter不再次Ensure；map与稳定files只借用，不增持任何Ref。
    auto* const manager = ResolveEnvironmentObjectReference<FileManagerNative28h>(root->FileManagerSlotAt00());
    // 空manager仍传目录地址+04；416B90先准备名字，再在原树访问点失败，不归入“未找到”。
    return {manager->directoryAt04, root_.Files()};
}
}
