#pragma once

#include <Runtime/EnvironmentArchiveConstructionInputs.h>
#include <Runtime/EnvironmentSoundParamsOwner.h>

#include <memory>

namespace KaGuYa::Start::Runtime
{
// 样本18，45B410：同一 SaveData 持有两套独立构造的 CSoundParams。
// +1C 同时供序列化和 CSoundManager 使用（48AB24、45D1DA..45D1F1）。
// 正文精确保持原90h；现代播放接口在正文之外借用，不额外增持声音。
class SaveDataEnvironmentOwner final
{
public:
    // profile 必须与构造 sysData 的来源相同；六个未写存储字节必须显式传入。
    // 不提供空白归档或默认存储值的构造路径。
    SaveDataEnvironmentOwner(
        const SystemDataEnvironmentArchive& sysData,
        const ParamsRuntimeProfile& profile,
        const SaveDataConstructionStorage& storage,
        ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner());
    // 两套声音对象独立构造，各自三次取实际 source，不从 profile 复制 SettingTag。
    SaveDataEnvironmentOwner(
        const SystemDataEnvironmentArchive& sysData,
        const ParamsRuntimeProfile& profile,
        const SaveDataConstructionStorage& storage,
        ISoundParamsSettingsSource& soundSettings,
        ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner());
    SaveDataEnvironmentOwner(ISaveDataConstructionSource& source,
        const SaveDataConstructionStorage& storage, ISoundParamsSettingsSource& soundSettings,
        ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner());

    SaveDataEnvironmentOwner(const SaveDataEnvironmentOwner&) = delete;
    SaveDataEnvironmentOwner& operator=(const SaveDataEnvironmentOwner&) = delete;
    SaveDataEnvironmentOwner(SaveDataEnvironmentOwner&&) = delete;
    SaveDataEnvironmentOwner& operator=(SaveDataEnvironmentOwner&&) = delete;

    [[nodiscard]] const YtlReferenceSlotRuntime& SoundReferenceAt1C() const noexcept;
    [[nodiscard]] SaveDataEnvironmentArchive& SaveData() noexcept;
    [[nodiscard]] const SaveDataEnvironmentArchive& SaveData() const noexcept;
    [[nodiscard]] EnvironmentSoundParamsArchive& SoundParamsAt1C() noexcept;
    [[nodiscard]] const EnvironmentSoundParamsArchive& SoundParamsAt1C() const noexcept;
    [[nodiscard]] EnvironmentSoundParamsRuntime& SoundRuntimeAt1C() noexcept;
    [[nodiscard]] const EnvironmentSoundParamsRuntime& SoundRuntimeAt1C() const noexcept;
    // +28 独立初始化，不进入 SaveData 序列化；保留 offset 命名，
    // 本 owner 不引入尚未闭合证据的备份/恢复传递。
    [[nodiscard]] EnvironmentSoundParamsArchive& SoundParamsAt28() noexcept;
    [[nodiscard]] const EnvironmentSoundParamsArchive& SoundParamsAt28() const noexcept;
    [[nodiscard]] EnvironmentSoundParamsRuntime& SoundRuntimeAt28() noexcept;
    [[nodiscard]] const EnvironmentSoundParamsRuntime& SoundRuntimeAt28() const noexcept;

private:
    struct DeleteArchive
    {
        void operator()(SaveDataEnvironmentArchive* archive) const noexcept
        { if (archive) archive->DeleteAtVslot04(1); }
    };
    using ArchiveOwner = std::unique_ptr<SaveDataEnvironmentArchive, DeleteArchive>;
    explicit SaveDataEnvironmentOwner(ArchiveOwner archive);
    static ArchiveOwner Create(ISaveDataConstructionSource& source,
        const SaveDataConstructionStorage& storage, ISoundParamsSettingsSource& sounds,
        ISettingTagErrorSource& errors);
    static ArchiveOwner CreateFixed(const SystemDataEnvironmentArchive& system,
        const ParamsRuntimeProfile& profile, const SaveDataConstructionStorage& storage,
        ISoundParamsSettingsSource* sounds, ISettingTagErrorSource& errors);

    ArchiveOwner saveData_;
    EnvironmentSoundParamsRuntime soundParamsAt1C_;
    EnvironmentSoundParamsRuntime soundParamsAt28_;
};
}
