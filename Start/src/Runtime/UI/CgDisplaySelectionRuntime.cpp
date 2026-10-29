#include <Runtime/CgDisplaySelectionRuntime.h>

#include <bit>
#include <limits>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
bool BuildCgDisplaySelectionAt4AD780And4B74A0(
    const SecretCgArchive& secretCg,
    const ParamsGameSystem& gameSystem,
    const std::int32_t categoryIndex,
    const std::int32_t itemIndex,
    CgDisplaySelectionState& state)
{
    if (categoryIndex < 0)
        throw std::out_of_range("CModeCGdisp category index is negative");
    if (itemIndex < 0)
        throw std::out_of_range("CModeCGdisp item index is negative");

    const auto category = static_cast<std::size_t>(categoryIndex);
    const auto item = static_cast<std::size_t>(itemIndex);
    if (category >= secretCg.CategoryCount())
        return false;

    if (item >= secretCg.ItemCount(category))
        return false;

    const auto flagIndices = secretCg.Indices(category, item);
    if (flagIndices.size() >
        static_cast<std::size_t>((std::numeric_limits<std::int32_t>::max)()))
    {
        throw std::overflow_error(
            "CModeCGdisp selection count exceeds signed 32-bit range");
    }
    if (category >= gameSystem.registCg.size() ||
        item >= gameSystem.registCg[category].itemsAt04.size())
    {
        throw std::out_of_range(
            "CModeCGdisp selection has no matching RegistCg item");
    }

    const auto& registItem = gameSystem.registCg[category].itemsAt04[item];
    state.countAt18 = static_cast<std::int32_t>(flagIndices.size());
    // 4AD853/4AD85E 与 4AD8E9 分别读取同一 RegistCg item 的
    // +4/+8/+0C；第三项不是 flag index，而是 params 的 typed integer。
    state.valueAt1C = std::bit_cast<std::int32_t>(registItem.valueAt0C);
    state.xAt20 = std::bit_cast<std::int32_t>(registItem.xAt04);
    state.yAt24 = std::bit_cast<std::int32_t>(registItem.yAt08);
    state.mappingPairsAt2C.clear();
    state.availability.clear();
    state.mappingPairsAt2C.reserve(flagIndices.size());
    state.availability.reserve(flagIndices.size());

    for (std::size_t index = 0; index < flagIndices.size(); ++index)
    {
        const std::int32_t flagIndex = flagIndices[index];
        if (flagIndex < 0 ||
            flagIndex >= secretCg.CountAtVslot08())
        {
            throw std::out_of_range(
                "CModeCGdisp selection references an invalid secret-CG flag");
        }

        const bool available =
            secretCg.GetFlagAtVslot0C(static_cast<std::uint32_t>(flagIndex)) != 0;
        state.availability.push_back(available);
        if (available)
        {
            // 4B7570 先按 flagIndex 解析 CSecretCG+0x0C 的 cg_flag，
            // 再读取同一8-byte记录首DWORD，而非列表首索引。
            state.mappingPairsAt2C.emplace_back(
                static_cast<std::int32_t>(index),
                std::bit_cast<std::int32_t>(
                    secretCg.RecordAt(static_cast<std::size_t>(flagIndex)).idAt00));
        }
    }
    return true;
}
}
