#include <Runtime/TitleDialogNative.h>

namespace KaGuYa::Start::Runtime::TitleDialogData
{
namespace
{
// Pure 563A20..563B27：前六槽与Mamatomo相同，第七槽保留实际hitcheck资源。
#define TITLE_HIT(command) "vrm:\\\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA_" command "_hitcheck.alp"
constexpr const char* hitchecks[6]{
    TITLE_HIT("\x82\xCD\x82\xB6\x82\xDF\x82\xA9\x82\xE7"), // 52D0F4
    TITLE_HIT("\x82\xC2\x82\xC3\x82\xAB\x82\xA9\x82\xE7"), // 52D028
    TITLE_HIT("\x82\x62\x82\x66\x8A\xD3\x8F\xDC"), // 52CF68
    TITLE_HIT("\x82\x72\x82\x83\x82\x85\x82\x8E\x82\x85\x8A\xD3\x8F\xDC"), // 52CE88
    TITLE_HIT("\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC"), // 52CDC8
    TITLE_HIT("\x83\x51\x81\x5B\x83\x80\x8F\x49\x97\xB9")}; // 52CCFC
#undef TITLE_HIT
}

std::span<const GuiButtonDescriptorNative2Ch, 6> PureHolicButtons() noexcept
{
    static const auto buttons = []
    {
        std::array<GuiButtonDescriptorNative2Ch, 6> result;
        const auto shared = ModernTitleDialogButtons();
        for (std::size_t i = 0; i != result.size(); ++i)
        {
            result[i] = shared[i];
            result[i].common.namesAt04[6] = hitchecks[i];
            result[i].hitSlotAt24 = 6;
        }
        return result;
    }();
    return buttons;
}
}
