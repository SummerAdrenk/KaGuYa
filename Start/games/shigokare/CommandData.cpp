#include "CommandData.h"

namespace KaGuYa::Start::Runtime::ShigokareCommandData
{
namespace
{
// 原Start.exe：ImageBase=0x400000；下列为首选VA，RVA=VA-0x400000。
// source=.BuildCache/pe/shigokare-ecchi-na-joshi-daisei-to-doki-x2-love-lesson/Start.exe。
// raw偏移/原字节/IDA路径及ARC头见command-variants-data/07.json。
// CP932逐字节保留；nullptr不补成ON，position=null保留ARC内嵌原点。
// VA 0x530B84, RVA 0x130B84: ADV画面_コマンド_近づく_click.vrm
constexpr char resource530B84[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x5F\x8B\xDF\x82\xC3\x82\xAD\x5F\x63\x6C\x69\x63\x6B\x2E\x76\x72\x6D";
// VA 0x530BA8, RVA 0x130BA8: ADV画面_コマンド_近づく_target.vrm
constexpr char resource530BA8[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x5F\x8B\xDF\x82\xC3\x82\xAD\x5F\x74\x61\x72\x67\x65\x74\x2E\x76\x72\x6D";
// VA 0x530BCC, RVA 0x130BCC: ADV画面_コマンド_近づく.vrm
constexpr char resource530BCC[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x5F\x8B\xDF\x82\xC3\x82\xAD\x2E\x76\x72\x6D";
// VA 0x530BE8, RVA 0x130BE8: ADV画面_コマンド_見つめる_click.vrm
constexpr char resource530BE8[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x5F\x8C\xA9\x82\xC2\x82\xDF\x82\xE9\x5F\x63\x6C\x69\x63\x6B\x2E\x76\x72\x6D";
// VA 0x530C0C, RVA 0x130C0C: ADV画面_コマンド_見つめる_target.vrm
constexpr char resource530C0C[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x5F\x8C\xA9\x82\xC2\x82\xDF\x82\xE9\x5F\x74\x61\x72\x67\x65\x74\x2E\x76\x72\x6D";
// VA 0x530C34, RVA 0x130C34: ADV画面_コマンド_見つめる.vrm
constexpr char resource530C34[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x5F\x8C\xA9\x82\xC2\x82\xDF\x82\xE9\x2E\x76\x72\x6D";
// VA 0x530C54, RVA 0x130C54: ADV画面_コマンド_課題をする_click.vrm
constexpr char resource530C54[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x5F\x89\xDB\x91\xE8\x82\xF0\x82\xB7\x82\xE9\x5F\x63\x6C\x69\x63\x6B\x2E\x76\x72\x6D";
// VA 0x530C7C, RVA 0x130C7C: ADV画面_コマンド_課題をする_target.vrm
constexpr char resource530C7C[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x5F\x89\xDB\x91\xE8\x82\xF0\x82\xB7\x82\xE9\x5F\x74\x61\x72\x67\x65\x74\x2E\x76\x72\x6D";
// VA 0x530CA4, RVA 0x130CA4: ADV画面_コマンド_課題をする.vrm
constexpr char resource530CA4[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x5F\x89\xDB\x91\xE8\x82\xF0\x82\xB7\x82\xE9\x2E\x76\x72\x6D";
// VA 0x530CC4, RVA 0x130CC4: ADV画面_コマンド_話す_click.vrm
constexpr char resource530CC4[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x5F\x98\x62\x82\xB7\x5F\x63\x6C\x69\x63\x6B\x2E\x76\x72\x6D";
// VA 0x530CE4, RVA 0x130CE4: ADV画面_コマンド_話す_target.vrm
constexpr char resource530CE4[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x5F\x98\x62\x82\xB7\x5F\x74\x61\x72\x67\x65\x74\x2E\x76\x72\x6D";
// VA 0x530D08, RVA 0x130D08: ADV画面_コマンド_話す.vrm
constexpr char resource530D08[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x5F\x98\x62\x82\xB7\x2E\x76\x72\x6D";
// VA 0x530D24, RVA 0x130D24: ADV画面_コマンド_ベース.vrm
constexpr char resource530D24[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x5F\x83\x78\x81\x5B\x83\x58\x2E\x76\x72\x6D";

// kind0背景保留自己的event；状态槽0/3/4共用原资源指针。
constexpr GuiStaticDescriptorNative24h backgrounds[]{
    {4, {resource530D24, nullptr, nullptr, resource530D24, resource530D24, nullptr, nullptr}, nullptr}, // 0x566CA4
};

// 4E3AD0按44h读取name/hit七对；空状态的hit=7原样保留。
// 每个状态独立选择alpha命中槽，绝不是toggle或统一固定hit。
constexpr GuiMultiCheckButtonDescriptorNative44h multiButtons[]{
    {0, {{{resource530D08, 0}, {nullptr, 7}, {nullptr, 7}, {resource530CE4, 3}, {resource530CE4, 4}, {resource530CC4, 5}, {nullptr, 7}}}, nullptr, -1}, // 0x566CC8
    {1, {{{resource530CA4, 0}, {nullptr, 7}, {nullptr, 7}, {resource530C7C, 3}, {resource530C7C, 4}, {resource530C54, 5}, {nullptr, 7}}}, nullptr, -1}, // 0x566D0C
    {2, {{{resource530C34, 0}, {nullptr, 7}, {nullptr, 7}, {resource530C0C, 3}, {resource530C0C, 4}, {resource530BE8, 5}, {nullptr, 7}}}, nullptr, -1}, // 0x566D50
    {3, {{{resource530BCC, 0}, {nullptr, 7}, {nullptr, 7}, {resource530BA8, 3}, {resource530BA8, 4}, {resource530B84, 5}, {nullptr, 7}}}, nullptr, -1}, // 0x566D94
};

// 外层恰好两条，无255哨兵；内部保留负source终止对。
// 0x567370
constexpr GuiDialogNavigationMapping pairs0[]{{0, 3}, {1, 0}, {2, 1}, {3, 2}, {-1, 0}};
// 0x567348
constexpr GuiDialogNavigationMapping pairs1[]{{0, 1}, {1, 2}, {2, 3}, {3, 0}, {-1, 0}};
constexpr SelectNavigationDescriptorNative8h navigation[]{
    {1, pairs0}, // 0x567398
    {2, pairs1}, // 0x5673A0
};

// CRT 0x5243f0 -> BSS 0x56b3b0；原串VA 0x529C04。
constexpr char confirmCue[] = "\x83\x4E\x83\x8A\x83\x62\x83\x4E\x89\xB9\x2E\x73\x65\x64";

// CRT 0x524410 -> BSS 0x56b3b4；原串VA 0x529C14。
constexpr char hoverCue[] = "\x91\x49\x91\xF0\x89\xB9\x2E\x73\x65\x64";

// Begin 4D8B90不读mask；Poll 4FDE30接受0..3原样返回，空results表示恒等。
constexpr CommandSelectionResourceProfile profile{
    {}, navigation, backgrounds, multiButtons, CommandSelectionAvailability::All, {}, {}, confirmCue, hoverCue};
}

const CommandSelectionResourceProfile& Profile() noexcept { return profile; }
const char* ConfirmCue() noexcept { return confirmCue; }
const char* HoverCue() noexcept { return hoverCue; }
}
