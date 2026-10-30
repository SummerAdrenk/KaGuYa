#include "WindowData.h"

namespace KaGuYa::Start::Runtime::BakuWindowData
{
namespace
{
// Baku 46E6F0读取563E58两项；显式坐标不由18号分辨率缩放。
constexpr WaitMarkNativeProfile wait{{
    {"\x83\x7D\x81\x5B\x83\x4E.vrm", 1, 1, 961, 529},
    {"\x83\x49\x81\x5B\x83\x67\x83\x7D\x81\x5B\x83\x4E.vrm", 1, 1, 955, 529}}};

constexpr char normal[] = "\x91\x49\x91\xF0\x8E\x88.vrm";
constexpr char target[] = "\x91\x49\x91\xF0\x8E\x88\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67.vrm";
constexpr char click[] = "\x91\x49\x91\xF0\x8E\x88\x83\x4E\x83\x8A\x83\x62\x83\x4E.vrm";
// 56EAFC的七资源槽；4E3230随后按56EB28覆盖各控件ID和位置。
constexpr GuiButtonDescriptorNative2Ch descriptor{{0,
    {normal, nullptr, nullptr, target, target, click, nullptr}, nullptr}, 7, -1};
constexpr SelectButtonPlacementNative10h placements[]{
    {&descriptor, 115, 107, 0}, {&descriptor, 115, 212, 1}, {&descriptor, 115, 317, 2}};
constexpr std::int32_t two[]{0, 1};
constexpr std::int32_t three[]{0, 1, 2};
constexpr GuiDialogNavigationMapping twoPairs[]{{0, 1}, {1, 0}, {-1, 0}};
constexpr GuiDialogNavigationMapping threeForward[]{{0, 1}, {1, 2}, {2, 0}, {-1, 0}};
constexpr GuiDialogNavigationMapping threeBackward[]{{0, 2}, {1, 0}, {2, 1}, {-1, 0}};
constexpr SelectNavigationDescriptorNative8h twoNavigation[]{{2, twoPairs}, {1, twoPairs}, {0xFF, nullptr}};
constexpr SelectNavigationDescriptorNative8h threeNavigation[]{{2, threeForward}, {1, threeBackward}, {0xFF, nullptr}};
// 4E3570取56EB6C/56EC04；0/1候选仍无表，不虚构新选择行为。
const SelectDialogNativeProfile selection{placements, {nullptr, nullptr, two, three},
    {nullptr, nullptr, twoNavigation, threeNavigation}, std::size(placements)};

// 4E2D80 -> 4FDFD0在56EA90+4Ch取轨道；行程修正仍为共享的(0,0,-12,0)。
constexpr OpacityDialogNativeProfile opacity{{699, 556, 784, 575}, {}};
}

const WaitMarkNativeProfile& WaitMark() noexcept { return wait; }
const SelectDialogNativeProfile& Selection() noexcept { return selection; }
const OpacityDialogNativeProfile& Opacity() noexcept { return opacity; }
}
