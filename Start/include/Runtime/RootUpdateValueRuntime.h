#pragma once

#include <Runtime/ScrVmUpdateHandler.h>
#include <Windows.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;
class RootRuntimeOwner;

// 借用稳定进程 owner；构造不创建 root/CParams。两者须长于本适配器。
// 每次只借本次捕获的 CParams，不缓存正文，也不保活旧代或跨正文释放使用。
class RootUpdateValueRuntime final : public IScrVmUpdateValueSource
{
public:
    RootUpdateValueRuntime(EnvironmentStartupRuntime& environment, RootRuntimeOwner& root,
        decltype(&::MessageBoxA) messageBox) noexcept;

    [[nodiscard]] std::int32_t ReadUpdateValueAt4C73A0(std::int32_t index) const override;

private:
    EnvironmentStartupRuntime& environment_;
    RootRuntimeOwner& root_;
    decltype(&::MessageBoxA) messageBox_;
};
}
