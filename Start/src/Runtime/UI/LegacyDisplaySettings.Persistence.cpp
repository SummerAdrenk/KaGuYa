#include <Runtime/LegacyDisplaySettingsPersistence.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <cstring>
#include <limits>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
std::uint32_t LegacyDisplaySettingsSaveData::ReadScaling()
{
    std::uint32_t value;
    // 01:409730/40B600；原 SaveData+0C 已属于 bytesAt04 的既有 12 字节 wire。
    std::memcpy(&value, source_.CurrentSaveData().bytesAt04.data() + 8, sizeof(value));
    return value;
}
void LegacyDisplaySettingsSaveData::WriteScaling(std::uint32_t value)
{
    std::memcpy(source_.CurrentSaveData().bytesAt04.data() + 8, &value, sizeof(value));
}
LegacyDisplayMode LegacyDisplaySettingsSaveData::ReadDeviceMode(std::uint32_t index)
{
    auto& records = source_.CurrentSaveData().recordsAt10;
    if (index >= records.size()) throw std::out_of_range("Legacy display device has no SaveData record");
    const auto& r = records[index];
    return {r.valueAt00, r.valueAt04, r.valueAt08, r.valueAt0C};
}
void LegacyDisplaySettingsSaveData::WriteDeviceMode(std::uint32_t index, LegacyDisplayMode mode)
{
    // 01:417A50/417A9D/417AEB/417B39，03:419F50；不 resize、不制造设备记录或副文件。
    auto& records = source_.CurrentSaveData().recordsAt10;
    if (index >= records.size()) throw std::out_of_range("Legacy display device has no SaveData record");
    records[index] = {mode.width, mode.height, mode.bitsPerPixel, mode.frequency};
}
std::uint32_t LegacyDisplaySettingsSaveData::DeviceRecordCount()
{ return source_.CurrentSaveData().recordsAt10.size(); }
void LegacyDisplaySettingsSaveData::ClearDeviceRecords()
{ source_.CurrentSaveData().recordsAt10.clear(); }
void LegacyDisplaySettingsSaveData::AppendDeviceRecord(LegacyDisplayMode mode)
{
    // 01:467D2B..467E73：仅启动数量不符分支逐项追加；失败保持已完成的记录前缀。
    auto& records = source_.CurrentSaveData().recordsAt10;
    const auto count = records.size();
    if (count == std::numeric_limits<std::uint32_t>::max()) throw std::length_error("Too many display records");
    records.resize(count + 1);
    records[count] = {mode.width, mode.height, mode.bitsPerPixel, mode.frequency};
}
}
