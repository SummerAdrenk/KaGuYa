#pragma once
#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentRootOwner;
class ParamsRuntimeRootOwner;
class SystemDataEnvironmentArchive;
struct ParamsGameSystem;
enum class SaveDataName { Given, Family };
enum class SaveDataSystemField : std::uint32_t { At40 = 0x40, At44 = 0x44, At50 = 0x50, At54 = 0x54, At38 = 0x38, At3C = 0x3C };
class ISaveDataConstructionSource
{
public:
    virtual ~ISaveDataConstructionSource() = default;
    virtual const RuntimeCString& ReadName(SaveDataName name) = 0;
    virtual std::uint32_t ReadSystemValue(SaveDataSystemField field) = 0;
};

// 工具/夹具的显式固定来源；仍在各调用点读取字段，不提前制作名字或标量快照。
class SaveDataFixedConstructionSource final : public ISaveDataConstructionSource
{
public:
    SaveDataFixedConstructionSource(const SystemDataEnvironmentArchive& system, const ParamsGameSystem& game) noexcept
        : system_(system), game_(game) {}
    const RuntimeCString& ReadName(SaveDataName name) override;
    std::uint32_t ReadSystemValue(SaveDataSystemField field) override;
private:
    const SystemDataEnvironmentArchive& system_;
    const ParamsGameSystem& game_;
};

// 原根入口：每次重新418480/4180C0，不缓存前一次捕获的GS或SysData。
class SaveDataRootConstructionSource final : public ISaveDataConstructionSource
{
public:
    SaveDataRootConstructionSource(EnvironmentRootOwner& environment, ParamsRuntimeRootOwner& params) noexcept
        : environment_(environment), params_(params) {}
    const RuntimeCString& ReadName(SaveDataName name) override;
    std::uint32_t ReadSystemValue(SaveDataSystemField field) override;
private:
    EnvironmentRootOwner& environment_;
    ParamsRuntimeRootOwner& params_;
};

}
