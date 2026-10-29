#pragma once

#include <Runtime/ScrSaveFileArchiveStateRuntime.h>
#include <Runtime/ScrVmParamsGenerationBinding.h>

#include <cstdint>
#include <memory>
#include <string>

namespace KaGuYa::Start::Runtime
{
class ScrScriptRuntimeState;
class SaveLoadFileFactoryRuntime;
class ISaveLoadPreviewServices;
class ISaveLoadDeleteFileApi;
class SaveFileStateNativeArchive;
using SaveFileCatalog = SaveFileStateNativeArchive;

enum class RollingSaveKind { Auto, Quick };

class IRollingSaveConfirmation
{
public:
    virtual ~IRollingSaveConfirmation() = default;
    virtual void CaptureDisplayAt48CA30() = 0;
    [[nodiscard]] virtual std::int32_t RunModalAt5071D0() = 0;
};

class IRollingSaveConfirmationServices
{
public:
    virtual ~IRollingSaveConfirmationServices() = default;
    // 必须创建4E3330(mode=3)的真实对象；不得以其他确认框或默认同意代替。
    [[nodiscard]] virtual std::unique_ptr<IRollingSaveConfirmation>
        CreateQuickSaveConfirmationAt4E3330() = 0;
};

// 仅48E5F0的page=-2/-1完整分支；不冒充普通页及48D290的实现。
class RollingSaveCatalogRefreshRuntime final
{
public:
    RollingSaveCatalogRefreshRuntime(SaveFileCatalog& catalog,
        IStartFileApi& files, const SaveDataEnvironmentArchive& save,
        const std::string& directoryCp932, IScrVmProgramRandSource& random,
        const SaveFileFormatProfile& profile) noexcept;

    void RefreshRollingItemAt48E5F0(RollingSaveKind kind, std::int32_t item);

private:
    SaveFileCatalog& catalog_;
    IStartFileApi& files_;
    const SaveDataEnvironmentArchive& save_;
    const std::string& directory_;
    IScrVmProgramRandSource& random_;
    const SaveFileFormatProfile& profile_;
};

class RollingSaveRuntime final
{
public:
    // script与archive借同一可替换generation槽；临时读档只在refresh栈内创建。
    RollingSaveRuntime(ScrScriptRuntimeState& script,
        ScrVmParamsGenerationBinding generation,
        SaveFileCatalog& catalog, const SaveDataEnvironmentArchive& save,
        IStartFileApi& files, ISaveLoadPreviewServices& preview,
        ISaveLoadDeleteFileApi& deletion, const std::string& directoryCp932,
        IScrVmProgramRandSource& random, const SaveFileFormatProfile& profile,
        IRollingSaveConfirmationServices& confirmation);

    void AutoSaveAt46B280(std::uint8_t captureFresh);
    // 原栈参占DWORD，但486143只将AL复制到factory+0C。
    void QuickSaveAt485FC0(std::int32_t captureFresh);

    // 原this中的factory+0C由显式byte表达；两入口不清description、不刷新目录。
    [[nodiscard]] std::int32_t RollAutoAt4883F0(std::uint8_t captureFresh);
    [[nodiscard]] std::int32_t RollQuickAt4888C0(std::uint8_t captureFresh);

private:
    [[nodiscard]] SaveLoadFileFactoryRuntime CreateFactory(RollingSaveKind kind);
    [[nodiscard]] std::int32_t Roll(SaveLoadFileFactoryRuntime& factory,
        RollingSaveKind kind, std::uint8_t captureFresh);
    [[nodiscard]] std::int32_t& Latest(RollingSaveKind kind) noexcept;

    ScrScriptRuntimeState& script_;
    ScrVmParamsGenerationBinding generation_;
    SaveFileCatalog& catalog_;
    const SaveDataEnvironmentArchive& save_;
    ISaveLoadPreviewServices& preview_;
    ISaveLoadDeleteFileApi& deletion_;
    const std::string& directory_;
    IRollingSaveConfirmationServices& confirmation_;
    ScrSaveFileArchiveStateRuntime archive_;
    SaveFileContainerRuntime container_;
    RollingSaveCatalogRefreshRuntime refresh_;
};
}
