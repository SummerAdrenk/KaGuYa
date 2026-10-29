#pragma once

#include <Runtime/ScrVmMusicHandler.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;

// 稳定的进程 Environment 必须活过本适配器；498280 可重建归档槽，不能缓存 slot2 或字符串。
class EnvironmentMusicNameRuntime final : public IScrVmMusicNameWriter
{
public:
    explicit EnvironmentMusicNameRuntime(EnvironmentStartupRuntime& environment) noexcept;

    void WriteCurrentMusicNameAt2C(const char* name) override;

private:
    EnvironmentStartupRuntime& environment_;
};
}
