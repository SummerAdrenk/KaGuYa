#include <Runtime/SelectDialogNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
// PE603C94与607098..60718C：保留原null资源、五份独立按钮及两个/三个选项表。
constexpr char normal[] = "\x91\x49\x91\xF0\x8E\x88.vrm";
constexpr char target[] = "\x91\x49\x91\xF0\x8E\x88\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67.vrm";
constexpr char click[] = "\x91\x49\x91\xF0\x8E\x88\x83\x4E\x83\x8A\x83\x62\x83\x4E.vrm";
constexpr GuiButtonDescriptorNative2Ch descriptor{{0,
    {normal, nullptr, nullptr, target, target, click, nullptr}, nullptr}, 7, -1};
constexpr SelectButtonPlacementNative10h placements[]{
    {&descriptor, 145, 237, 0}, {&descriptor, 145, 347, 1},
    {&descriptor, 145, 200, 2}, {&descriptor, 145, 310, 3}, {&descriptor, 145, 420, 4}};
constexpr std::int32_t two[]{0, 1};
constexpr std::int32_t three[]{2, 3, 4};
constexpr GuiDialogNavigationMapping twoPairs[]{{0, 1}, {1, 0}, {-1, 0}};
constexpr GuiDialogNavigationMapping threeForward[]{{2, 3}, {3, 4}, {4, 2}, {-1, 0}};
constexpr GuiDialogNavigationMapping threeBackward[]{{2, 4}, {3, 2}, {4, 3}, {-1, 0}};
constexpr SelectNavigationDescriptorNative8h twoNavigation[]{{2, twoPairs}, {1, twoPairs}, {0xFF, nullptr}};
constexpr SelectNavigationDescriptorNative8h threeNavigation[]{{2, threeForward}, {1, threeBackward}, {0xFF, nullptr}};
const SelectDialogNativeProfile profile{placements, {nullptr, nullptr, two, three},
    {nullptr, nullptr, twoNavigation, threeNavigation}};
}
const SelectDialogNativeProfile& Sample18SelectDialogNativeProfile() noexcept { return profile; }
}
