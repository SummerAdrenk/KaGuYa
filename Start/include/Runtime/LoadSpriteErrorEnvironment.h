#pragma once

#include <Runtime/ProgramWindowFieldReferences.h>
#include <Runtime/RuntimeCString.h>
#include <string>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;
class EnvironmentRootOwner;

struct LoadSpriteErrorEnvironmentView final
{
    LoadSpriteErrorEnvironmentView(const std::string& caption, ProgramWindowHandleReference window) noexcept
        : windowAt2C(window), caption_(&caption) {}
    LoadSpriteErrorEnvironmentView(const RuntimeCString& caption, ProgramWindowHandleReference window) noexcept
        : windowAt2C(window), caption_(&caption) {}
    [[nodiscard]] const char* CaptionAt10() const noexcept
    {
        return std::visit([](auto p) -> const char*
        {
            if constexpr (std::same_as<std::remove_cvref_t<decltype(*p)>, RuntimeCString>)
                return p->GetString();
            else return p->c_str();
        }, caption_);
    }
    ProgramWindowHandleReference windowAt2C;
private:
    std::variant<const std::string*, const RuntimeCString*> caption_;
};

class ILoadSpriteErrorEnvironmentSource
{
public:
    virtual ~ILoadSpriteErrorEnvironmentSource() = default;
    virtual LoadSpriteErrorEnvironmentView CaptureAt4180C0() = 0;
};

// 真实0x60环境根来源；仅借用原字符串/窗口槽，绝不建立同步副本。
class RootLoadSpriteErrorEnvironment final : public ILoadSpriteErrorEnvironmentSource
{
public:
    explicit RootLoadSpriteErrorEnvironment(EnvironmentRootOwner& owner) noexcept : owner_(owner) {}
    LoadSpriteErrorEnvironmentView CaptureAt4180C0() override;
private:
    EnvironmentRootOwner& owner_;
};

// 已存在 Environment 的借用适配。不是4180C0的singleton锁/懒创建实现。
class ExistingLoadSpriteErrorEnvironment final : public ILoadSpriteErrorEnvironmentSource
{
public:
    explicit ExistingLoadSpriteErrorEnvironment(EnvironmentStartupRuntime& environment) noexcept
        : environment_(environment) {}
    LoadSpriteErrorEnvironmentView CaptureAt4180C0() override;
private:
    EnvironmentStartupRuntime& environment_;
};

// 固定对象图的显式适配，供独立资源验证；仍在调用点读取字段，不保留值快照。
class FixedLoadSpriteErrorEnvironment final : public ILoadSpriteErrorEnvironmentSource
{
public:
    FixedLoadSpriteErrorEnvironment(ProgramWindowHandleReference window, const std::string& caption) noexcept
        : view_{caption, window} {}
    LoadSpriteErrorEnvironmentView CaptureAt4180C0() override { return view_; }
private:
    LoadSpriteErrorEnvironmentView view_;
};
}
