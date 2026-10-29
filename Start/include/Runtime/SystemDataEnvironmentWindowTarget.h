#pragma once

#include <variant>

namespace KaGuYa::Start::Runtime
{
struct ParamsGameSystem;
struct SystemDataEnvironmentOwnerInitialization;
class EnvironmentRootOwner;

// 旧快照入口仅供尚未迁移的装配器/夹具；原生入口直接写六次捕获所得的60h根。
// 两种目标互斥，不同步双份窗口状态，也不在原生捕获失败时退回快照。
class SystemDataEnvironmentWindowTarget final
{
public:
    SystemDataEnvironmentWindowTarget(SystemDataEnvironmentOwnerInitialization* snapshot = nullptr) noexcept
        : target_(snapshot) {}
    SystemDataEnvironmentWindowTarget(EnvironmentRootOwner& root) noexcept : target_(&root) {}
    void Apply(const ParamsGameSystem& retainedGameSystem) const;
private:
    std::variant<SystemDataEnvironmentOwnerInitialization*, EnvironmentRootOwner*> target_;
};
}
