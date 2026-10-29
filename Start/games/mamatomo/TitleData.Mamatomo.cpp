#include <Runtime/TitleDialogNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
// Mamatomo 5B8168..5B851C、Hakoniwa 575A88..575E3C及Nurse 52F340..52F700
// 的表字段和CP932原字节已逐项相等；共享数据不表示三个游戏所有行为相同。
// Bunny/Pure also share the background, navigation, black and load-scene bytes.
#define TITLE_PREFIX "vrm:\\\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA"
constexpr char base[] = TITLE_PREFIX "\x83\x78\x81\x5B\x83\x58.bmp";
#define TITLE_NAMES(index, command) \
    constexpr char off##index[] = TITLE_PREFIX "_" command "_off.alp"; \
    constexpr char lock##index[] = TITLE_PREFIX "_" command "_lock.alp"; \
    constexpr char target##index[] = TITLE_PREFIX "_" command "_target.alp"; \
    constexpr char click##index[] = TITLE_PREFIX "_" command "_click.alp"
TITLE_NAMES(0, "\x82\xCD\x82\xB6\x82\xDF\x82\xA9\x82\xE7");
TITLE_NAMES(1, "\x82\xC2\x82\xC3\x82\xAB\x82\xA9\x82\xE7");
TITLE_NAMES(2, "\x82\x62\x82\x66\x8A\xD3\x8F\xDC");
TITLE_NAMES(3, "\x82\x72\x82\x83\x82\x85\x82\x8E\x82\x85\x8A\xD3\x8F\xDC");
TITLE_NAMES(4, "\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC");
TITLE_NAMES(5, "\x83\x51\x81\x5B\x83\x80\x8F\x49\x97\xB9");
#undef TITLE_NAMES
#undef TITLE_PREFIX
const GuiStaticDescriptorNative24h staticRecord{
    1, {{base, nullptr, nullptr, base, base, nullptr, nullptr}}, nullptr};
#define TITLE_BUTTON(index, event) \
    {{event, {{off##index, lock##index, nullptr, target##index, target##index, click##index, nullptr}}, nullptr}, 0, -1}
const GuiButtonDescriptorNative2Ch buttons[6]{
    TITLE_BUTTON(0, 3), TITLE_BUTTON(1, 4), TITLE_BUTTON(2, 5),
    TITLE_BUTTON(3, 6), TITLE_BUTTON(4, 7), TITLE_BUTTON(5, 8)};
#undef TITLE_BUTTON
// 6083E0 order is input1 -> 6083A8, then input2 -> 608370.
constexpr GuiDialogNavigationMapping navigation[2][7]{
    {{3, 8}, {4, 3}, {5, 4}, {6, 5}, {7, 6}, {8, 7}, {-1, 0}},
    {{3, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 8}, {8, 3}, {-1, 0}}};
constexpr char blackName[] = "bg_black.bgd"; // 5B4DD8
constexpr char loadSceneName[] = "\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA"; // 5AE224, length10
}
const GuiStaticDescriptorNative24h& ModernTitleDialogStatic() noexcept { return staticRecord; }
std::span<const GuiButtonDescriptorNative2Ch, 6> ModernTitleDialogButtons() noexcept { return buttons; }
const GuiDialogNavigationMapping* ModernTitleDialogNavigation(std::int32_t index) noexcept
{ return navigation[index]; }
const char* ModernTitleDialogBlackName() noexcept { return blackName; }
const char* ModernTitleDialogLoadSceneName() noexcept { return loadSceneName; }
}
