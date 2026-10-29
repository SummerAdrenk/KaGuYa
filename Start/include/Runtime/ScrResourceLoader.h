#pragma once

#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/PersistenceFileAccess.h>
#include <Runtime/ResourceCatalog.h>
#include <Runtime/ScrContainer.h>

#include <cstdint>
#include <stdexcept>
#include <string>

namespace KaGuYa::Start::Runtime
{
enum class ScrResourceLoadError : std::uint8_t
{
    ResourceNotFound = 0,
    FileOpenFailed = 1,
    FileSeekFailed = 2,
    FileReadFailed = 3,
    KeyUnavailable = 4,
    PayloadMalformed = 5,
};

class ScrResourceLoadException final : public std::runtime_error
{
public:
    ScrResourceLoadException(ScrResourceLoadError error, const char* message);
    [[nodiscard]] ScrResourceLoadError Error() const noexcept;

private:
    ScrResourceLoadError error_;
};

struct LoadedScrResource final
{
    // 4D475B..4D477A：CFileReader名称保留传入的namespace、路径和扩展名。
    std::string resourceName;
    std::uint32_t archiveVersion = 0;
    ScrContainer container;
};

// 工具/诊断入口，保留精细错误分类；不代替生产LoadScript的工厂catch清理/空reader判定。
[[nodiscard]] LoadedScrResource LoadScrResource(
    const ResourceCatalog& catalog,
    const std::string& resourceName,
    std::int32_t scriptId,
    const FileTransactionContext* transactionContext,
    IStartFileApi& api,
    const IEnvironmentKeyTableSource& environmentKeyBytes);
}
