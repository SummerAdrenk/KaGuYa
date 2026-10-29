#pragma once

#include <Runtime/ResourceVersionRuntime.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentRootOwner;

// 借用同一进程根owner，不拥有版本对象，不缓存捕获所得的+54 Ref或解析结果。
class EnvironmentRootVersionSource final : public IResourceVersionSource
{
public:
    explicit EnvironmentRootVersionSource(EnvironmentRootOwner& root) noexcept : root_(root) {}
    [[nodiscard]] const ResourceVersionRuntime& CaptureResourceVersionAt4180C0() const override;
private:
    EnvironmentRootOwner& root_;
};
}
