#pragma once
#include <Runtime/LegacyDisplayDeviceOwner.h>

namespace KaGuYa::Start::Runtime
{
class SaveDataEnvironmentArchive;
class ILegacyDisplaySettingsSaveSource
{
public:
    virtual ~ILegacyDisplaySettingsSaveSource() = default;
    virtual SaveDataEnvironmentArchive& CurrentSaveData() = 0;
};
// 借用真实 slot1，不保留第二份配置。设备序号由owner过滤后的目录提供。
class LegacyDisplaySettingsSaveData final : public ILegacyDisplayDeviceStore
{
public:
    explicit LegacyDisplaySettingsSaveData(ILegacyDisplaySettingsSaveSource& source) noexcept : source_(source) {}
    std::uint32_t ReadScaling() override;
    void WriteScaling(std::uint32_t) override;
    LegacyDisplayMode ReadDeviceMode(std::uint32_t) override;
    void WriteDeviceMode(std::uint32_t, LegacyDisplayMode) override;
    std::uint32_t DeviceRecordCount() override;
    void ClearDeviceRecords() override;
    void AppendDeviceRecord(LegacyDisplayMode) override;
private:
    ILegacyDisplaySettingsSaveSource& source_;
};
}
