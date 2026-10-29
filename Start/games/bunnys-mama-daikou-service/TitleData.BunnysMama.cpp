#include <Runtime/TitleDialogNative.h>

namespace KaGuYa::Start::Runtime::TitleDialogData
{
namespace
{
// Bunny 55FAF8/55FB24：只有event5/6的七槽路径不同于Pure，其他按钮共享已核数据。
#define TITLE_PREFIX "vrm:\\\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA_"
#define TITLE_NAMES(index, command) \
    constexpr char off##index[] = TITLE_PREFIX command "_off.alp"; \
    constexpr char lock##index[] = TITLE_PREFIX command "_lock.alp"; \
    constexpr char target##index[] = TITLE_PREFIX command "_target.alp"; \
    constexpr char click##index[] = TITLE_PREFIX command "_click.alp"; \
    constexpr char hit##index[] = TITLE_PREFIX command "_hitcheck.alp"
TITLE_NAMES(2, "CG\x83\x82\x81\x5B\x83\x68"); // 52A19C..52A100
TITLE_NAMES(3, "\x89\xF1\x91\x7A\x83\x82\x81\x5B\x83\x68"); // 52A0D8..52A034
#undef TITLE_NAMES
#undef TITLE_PREFIX
}

std::span<const GuiButtonDescriptorNative2Ch, 6> BunnysMamaButtons()
{
    static const auto buttons = []
    {
        std::array<GuiButtonDescriptorNative2Ch, 6> result;
        const auto shared = ModernTitleDialogButtons(TitleButtonResources::PureHolic);
        for (std::size_t i = 0; i != result.size(); ++i) result[i] = shared[i];
        result[2].common.namesAt04 = {off2, lock2, nullptr, target2, target2, click2, hit2};
        result[3].common.namesAt04 = {off3, lock3, nullptr, target3, target3, click3, hit3};
        return result;
    }();
    return buttons;
}
}
