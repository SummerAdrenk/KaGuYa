#pragma once

#include <Runtime/ClickInfoMouseMapping.h>
#include <Runtime/RootControlNativeState.h>

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
enum class EnvironmentArchiveKind
{
    SaveData,
    UnSaveData,
    SecretCG,
    SecretSC,
    ClickInfo,
    SaveFileState,
    GlobalParams,
};

inline constexpr std::array<EnvironmentArchiveKind, 7> EnvironmentArchiveRegistrationOrder = {
    EnvironmentArchiveKind::SaveData,
    EnvironmentArchiveKind::UnSaveData,
    EnvironmentArchiveKind::SecretCG,
    EnvironmentArchiveKind::SecretSC,
    EnvironmentArchiveKind::ClickInfo,
    EnvironmentArchiveKind::SaveFileState,
    EnvironmentArchiveKind::GlobalParams,
};

class IPreWindowServices
{
public:
    virtual ~IPreWindowServices() = default;

    virtual void RegisterEnvironmentArchives(
        const std::array<EnvironmentArchiveKind, 7>& order) = 0;

    // 480230：env.ini 反序列化成功返回真；返回假时 4778B0 立即调用 488740 写默认值。
    [[nodiscard]] virtual bool PrepareEnvironmentPersistence() = 0;
    virtual void WriteEnvironmentPersistence() = 0;
    virtual void InstallWindowModeManager() = 0;

    [[nodiscard]] virtual const SaveDataEnvironmentArchive& SaveDataAtSlot1() const = 0;
    [[nodiscard]] virtual ClickInfoArchive& MutableClickInfoAtSlot5() = 0;

    [[nodiscard]] virtual std::string BuildInitializationErrorText() = 0;
    virtual void ShowInitializationError(
        std::string_view text,
        std::string_view caption,
        unsigned int type) = 0;
};

class PreWindowInitializer final
{
public:
    [[nodiscard]] bool Run(
        PreWindowLifecycleState& lifecycleState,
        IPreWindowServices& services) const;
};
}
