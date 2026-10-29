#pragma once

#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/SaveDataEnvironmentArchive.h>

#include <array>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
struct ParamsRuntimeProfile;
class SystemDataEnvironmentArchive;

// 45B410 未写 +06/+07、+62/+63、+86/+87；要求调用方显式提供存储值，
// 不提供默认构造，也不把现代零初始化宣称为原 operator new 的默认内容。
struct SaveDataConstructionStorage final
{
    constexpr SaveDataConstructionStorage(
        std::array<std::uint8_t, 2> bytesAt06,
        std::array<std::uint8_t, 2> bytesAt62,
        std::array<std::uint8_t, 2> bytesAt86) noexcept
        : storageBytesAt06(bytesAt06), storageBytesAt62(bytesAt62), storageBytesAt86(bytesAt86)
    {
    }

    std::array<std::uint8_t, 2> storageBytesAt06;
    std::array<std::uint8_t, 2> storageBytesAt62;
    std::array<std::uint8_t, 2> storageBytesAt86;
};

// 45B410：profile 必须来自构造该 SysData 的同一 params owner；返回独立值快照。
// ready=0 的拒绝是现代 builder 的成功来源前置条件，原 45B410 没有检查 ready。
// 已由调用方诊断的原恢复结果照值复制，不重新求值或清除诊断。
// 不创建声音对象、发布环境槽、读取路径或写入 env.ini。
[[nodiscard]] SaveDataConstructionInputs BuildSaveDataConstructionInputs(
    const SystemDataEnvironmentArchive& sysData,
    const ParamsRuntimeProfile& profile,
    const SaveDataConstructionStorage& storage);

// 45BCA0 / 4786A0：只装配来源表，首匹配和 flag ID 到索引的转换仍由真实
// SecretCgArchive 执行；保留 CP932、原序、重复项、空名称及空数组。
[[nodiscard]] SecretCgConstructionInputs BuildSecretCgConstructionInputs(
    const ParamsRuntimeProfile& profile);
}
