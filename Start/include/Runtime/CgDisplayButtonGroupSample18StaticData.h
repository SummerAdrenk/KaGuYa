#pragma once

#include <Runtime/CgDisplayButtonGroupCatalogRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 只对应 18#ママトモ 原 Start.exe 的 .rdata/.data 固定表。
// 地址和值均由原 PE 提取；其他 executable profile 必须提供自己的实现。
class CgDisplayButtonGroupSample18StaticDataAt499DC0 final
    : public ICgDisplayButtonGroupStaticDataAt499DC0
{
public:
    [[nodiscard]] CgDisplayButtonGroupResourceIdentifierRefAt499DC0
        ReadResourcePointerAtAddress(std::uintptr_t pointerAddress) const override;
    [[nodiscard]] std::int32_t ReadDwordAtAddress(
        std::uintptr_t address) const override;
    [[nodiscard]] CgDisplayButtonGroupResourceIdentifierRefAt499DC0
        FormatResourceIdentifierAt417130(
            std::uintptr_t formatAddress,
            std::int32_t categoryIndex,
            std::int32_t itemIndex) const override;
    [[nodiscard]] std::vector<std::array<std::int32_t, 4>>
        ReadNavigationTableAt6021E8() const override;
    [[nodiscard]] std::vector<std::array<std::int32_t, 4>>
        ReadNavigationTableAt602478() const;
};
}
