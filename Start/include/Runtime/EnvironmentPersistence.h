#pragma once

#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/PersistenceFileAccess.h>
#include <Runtime/SaveDataEnvironmentArchive.h>

#include <string>

namespace KaGuYa::Start::Runtime
{
class CategorizedSoundEnvironmentAdapter;
struct EnvironmentPersistencePaths final
{
    std::string env;
    std::string secret;
    std::string global;
};

class EnvironmentPersistenceCoordinator final
{
public:
    EnvironmentPersistenceCoordinator(
        SaveDataEnvironmentArchive& saveData,
        SecretCgArchive& secretCg,
        SecretScArchive& secretSc,
        GlobalParamsArchive& globalParams,
        const IEnvironmentKeyByteSource& keyBytes,
        EnvironmentPersistencePaths paths,
        const FileTransactionContext* transactionContext,
        IStartFileApi& fileApi,
        CategorizedSoundEnvironmentAdapter* categorizedSound = nullptr) noexcept;

    [[nodiscard]] bool TryLoadEnvironment();
    [[nodiscard]] bool WriteEnvironment();
    [[nodiscard]] bool TryLoadSecret();
    [[nodiscard]] bool WriteSecret();
    [[nodiscard]] bool TryLoadGlobal();
    [[nodiscard]] bool WriteGlobal();

private:
    SaveDataEnvironmentArchive& saveData_;
    SecretCgArchive& secretCg_;
    SecretScArchive& secretSc_;
    GlobalParamsArchive& globalParams_;
    const IEnvironmentKeyByteSource& keyBytes_;
    EnvironmentPersistencePaths paths_;
    const FileTransactionContext* transactionContext_ = nullptr;
    IStartFileApi& fileApi_;
    CategorizedSoundEnvironmentAdapter* categorizedSound_ = nullptr;
};
}
