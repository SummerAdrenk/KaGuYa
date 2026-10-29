#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// 对应样本可执行文件内由 sub_498020 检索的 CP932 固定名称表。
class ScrVmExecutableProfile final
{
public:
    explicit ScrVmExecutableProfile(std::vector<std::string> namedValues);

    // 40B592..40B5A8 按首 NUL 构造查询串；现代字符串分配异常允许传出。
    [[nodiscard]] std::int32_t ResolveNamedValue(std::string_view cp932Name) const;
    [[nodiscard]] const std::string& NameAt(std::size_t index) const;
    [[nodiscard]] std::size_t Size() const noexcept;

private:
    // 固定 PE 的 41+45 项均非 null；此值模型不表示 4980E3 的 null 表项异常。
    std::vector<std::string> namedValues_;
};

// 只属于 18 号《ママトモ》Start.exe 的 41+45 项 PE 固定表。
[[nodiscard]] ScrVmExecutableProfile BuildMamatomoScrVmExecutableProfile();
}
