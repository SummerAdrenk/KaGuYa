#pragma once

#include <Runtime/ParamsText.h>
#include <Runtime/RuntimeCString.h>

#include <string_view>

namespace KaGuYa::Start::Runtime
{
// 单一4字节CString槽；不保存UTF-16/窄串镜像，不代表整个11Ch GS的构造EH已迁移。
class ParamsScalarString final
{
public:
    ParamsScalarString();
    explicit ParamsScalarString(IRuntimeStringManagerSource& strings);
    ~ParamsScalarString() noexcept;

    // profile/严格parser/夹具的显式深快照；生产reader直接写Native()。
    ParamsScalarString(const ParamsScalarString& other);
    ParamsScalarString& operator=(const ParamsScalarString& other);
    ParamsScalarString(const ParamsText& snapshot);
    ParamsScalarString& operator=(const ParamsText& snapshot);
    void AssignSnapshot(std::string_view bytes);

    [[nodiscard]] RuntimeCString& Native() noexcept { return value_; }
    [[nodiscard]] const RuntimeCString& Native() const noexcept { return value_; }
    [[nodiscard]] std::string_view View() const noexcept { return RuntimeStringView(value_); }
    [[nodiscard]] bool operator==(const ParamsScalarString& other) const noexcept
        { return View() == other.View(); }

private:
    // 原始manager入口负责构造/释放，不能再由ATL自动析构重复减持。
    union { RuntimeCString value_; };
};
static_assert(sizeof(ParamsScalarString) == 4);
}
