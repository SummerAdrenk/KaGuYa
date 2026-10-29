#include "CommandData.h"

namespace KaGuYa::Start::Runtime::LoveHolicCommandData
{
namespace
{
// 原Start.exe：ImageBase=0x400000；下列为首选VA，RVA=VA-0x400000。
// source=.BuildCache/pe/love-x-holic-miwaku-no-otome-to-hakudaku-kankei/Start.exe。
// raw偏移/原字节/IDA路径及ARC头见command-variants-data/08.json。
// CP932逐字节保留；nullptr不补成ON，position=null保留ARC内嵌原点。
// VA 0x540DFC, RVA 0x140DFC: 儀式可否_儀式断る_click.vrm
constexpr char resource540DFC[] = "\x8B\x56\x8E\xAE\x89\xC2\x94\xDB\x5F\x8B\x56\x8E\xAE\x92\x66\x82\xE9\x5F\x63\x6C\x69\x63\x6B\x2E\x76\x72\x6D";
// VA 0x540E18, RVA 0x140E18: 儀式可否_儀式断る_target.vrm
constexpr char resource540E18[] = "\x8B\x56\x8E\xAE\x89\xC2\x94\xDB\x5F\x8B\x56\x8E\xAE\x92\x66\x82\xE9\x5F\x74\x61\x72\x67\x65\x74\x2E\x76\x72\x6D";
// VA 0x540E38, RVA 0x140E38: 儀式可否_儀式断る_on.vrm
constexpr char resource540E38[] = "\x8B\x56\x8E\xAE\x89\xC2\x94\xDB\x5F\x8B\x56\x8E\xAE\x92\x66\x82\xE9\x5F\x6F\x6E\x2E\x76\x72\x6D";
// VA 0x540E54, RVA 0x140E54: 儀式可否_儀式承諾する_click.vrm
constexpr char resource540E54[] = "\x8B\x56\x8E\xAE\x89\xC2\x94\xDB\x5F\x8B\x56\x8E\xAE\x8F\xB3\x91\xF8\x82\xB7\x82\xE9\x5F\x63\x6C\x69\x63\x6B\x2E\x76\x72\x6D";
// VA 0x540E74, RVA 0x140E74: 儀式可否_儀式承諾する_target.vrm
constexpr char resource540E74[] = "\x8B\x56\x8E\xAE\x89\xC2\x94\xDB\x5F\x8B\x56\x8E\xAE\x8F\xB3\x91\xF8\x82\xB7\x82\xE9\x5F\x74\x61\x72\x67\x65\x74\x2E\x76\x72\x6D";
// VA 0x540E98, RVA 0x140E98: 儀式可否_儀式承諾する_on.vrm
constexpr char resource540E98[] = "\x8B\x56\x8E\xAE\x89\xC2\x94\xDB\x5F\x8B\x56\x8E\xAE\x8F\xB3\x91\xF8\x82\xB7\x82\xE9\x5F\x6F\x6E\x2E\x76\x72\x6D";
// VA 0x540EB8, RVA 0x140EB8: ADV画面_コマンドボタン_click_食.vrm
constexpr char resource540EB8[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x83\x7B\x83\x5E\x83\x93\x5F\x63\x6C\x69\x63\x6B\x5F\x90\x48\x2E\x76\x72\x6D";
// VA 0x540EDC, RVA 0x140EDC: ADV画面_コマンドボタン_target_食.vrm
constexpr char resource540EDC[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x83\x7B\x83\x5E\x83\x93\x5F\x74\x61\x72\x67\x65\x74\x5F\x90\x48\x2E\x76\x72\x6D";
// VA 0x540F04, RVA 0x140F04: ADV画面_コマンドボタン_click_恋愛.vrm
constexpr char resource540F04[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x83\x7B\x83\x5E\x83\x93\x5F\x63\x6C\x69\x63\x6B\x5F\x97\xF6\x88\xA4\x2E\x76\x72\x6D";
// VA 0x540F2C, RVA 0x140F2C: ADV画面_コマンドボタン_target_恋愛.vrm
constexpr char resource540F2C[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x83\x7B\x83\x5E\x83\x93\x5F\x74\x61\x72\x67\x65\x74\x5F\x97\xF6\x88\xA4\x2E\x76\x72\x6D";
// VA 0x540F54, RVA 0x140F54: ADV画面_コマンドボタン_click_勉強.vrm
constexpr char resource540F54[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x83\x7B\x83\x5E\x83\x93\x5F\x63\x6C\x69\x63\x6B\x5F\x95\xD7\x8B\xAD\x2E\x76\x72\x6D";
// VA 0x540F7C, RVA 0x140F7C: ADV画面_コマンドボタン_target_勉強.vrm
constexpr char resource540F7C[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x83\x7B\x83\x5E\x83\x93\x5F\x74\x61\x72\x67\x65\x74\x5F\x95\xD7\x8B\xAD\x2E\x76\x72\x6D";
// VA 0x540FA4, RVA 0x140FA4: ADV画面_コマンドボタン_click_趣味.vrm
constexpr char resource540FA4[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x83\x7B\x83\x5E\x83\x93\x5F\x63\x6C\x69\x63\x6B\x5F\x8E\xEF\x96\xA1\x2E\x76\x72\x6D";
// VA 0x540FCC, RVA 0x140FCC: ADV画面_コマンドボタン_target_趣味.vrm
constexpr char resource540FCC[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x83\x7B\x83\x5E\x83\x93\x5F\x74\x61\x72\x67\x65\x74\x5F\x8E\xEF\x96\xA1\x2E\x76\x72\x6D";
// VA 0x540FF4, RVA 0x140FF4: ADV画面_コマンドボタン_click_会話.vrm
constexpr char resource540FF4[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x83\x7B\x83\x5E\x83\x93\x5F\x63\x6C\x69\x63\x6B\x5F\x89\xEF\x98\x62\x2E\x76\x72\x6D";
// VA 0x54101C, RVA 0x14101C: ADV画面_コマンドボタン_target_会話.vrm
constexpr char resource54101C[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x83\x7B\x83\x5E\x83\x93\x5F\x74\x61\x72\x67\x65\x74\x5F\x89\xEF\x98\x62\x2E\x76\x72\x6D";
// VA 0x541044, RVA 0x141044: ADV画面_コマンドボタン.vrm
constexpr char resource541044[] = "\x41\x44\x56\x89\xE6\x96\xCA\x5F\x83\x52\x83\x7D\x83\x93\x83\x68\x83\x7B\x83\x5E\x83\x93\x2E\x76\x72\x6D";

// kind0背景保留自己的event；状态槽0/3/4共用原资源指针。
constexpr GuiStaticDescriptorNative24h backgrounds[]{
    {7, {resource541044, nullptr, nullptr, resource541044, resource541044, nullptr, nullptr}, nullptr}, // 0x579F84
};

// kind1的0..4没有ON图，命中槽固定4；5/6有ON图，命中槽固定0。
constexpr GuiButtonDescriptorNative2Ch buttons[]{
    {{0, {nullptr, nullptr, nullptr, resource54101C, resource54101C, resource540FF4, nullptr}, nullptr}, 4, -1}, // 0x57AF48
    {{1, {nullptr, nullptr, nullptr, resource540FCC, resource540FCC, resource540FA4, nullptr}, nullptr}, 4, -1}, // 0x57AF74
    {{2, {nullptr, nullptr, nullptr, resource540F7C, resource540F7C, resource540F54, nullptr}, nullptr}, 4, -1}, // 0x57AFA0
    {{3, {nullptr, nullptr, nullptr, resource540F2C, resource540F2C, resource540F04, nullptr}, nullptr}, 4, -1}, // 0x57AFCC
    {{4, {nullptr, nullptr, nullptr, resource540EDC, resource540EDC, resource540EB8, nullptr}, nullptr}, 4, -1}, // 0x57AFF8
    {{5, {resource540E98, nullptr, nullptr, resource540E74, resource540E74, resource540E54, nullptr}, nullptr}, 0, -1}, // 0x57B024
    {{6, {resource540E38, nullptr, nullptr, resource540E18, resource540E18, resource540DFC, nullptr}, nullptr}, 0, -1}, // 0x57B050
};

// 外层恰好两条，无255哨兵；内部保留负source终止对。
// 0x57BDE8
constexpr GuiDialogNavigationMapping pairs0[]{{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0}, {5, 6}, {6, 5}, {-1, 0}};
// 0x57BE28
constexpr GuiDialogNavigationMapping pairs1[]{{0, 4}, {1, 0}, {2, 1}, {3, 2}, {4, 3}, {5, 6}, {6, 5}, {-1, 0}};
constexpr SelectNavigationDescriptorNative8h navigation[]{
    {4, pairs0}, // 0x57BE68
    {3, pairs1}, // 0x57BE70
};

// 509480: prog_param00<<6；每行8对(event,state)，包含背景event7。
// 220份实际SCR的36处PROGRAM19前赋值仅0(24处)/1(12处)，故只收录两行。
// 原码无范围检查；不是从相邻数据推测声明长度。域外与任意存档恢复未证实。
// 详情script-domain.json；主线必须明确拒绝负数/>=2，不clamp或取模。
constexpr CommandSelectionControlState layout0[]{{0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 0}, {6, 0}, {7, 1}}; // 0x57b080
constexpr CommandSelectionControlState layout1[]{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 1}, {6, 1}, {7, 0}}; // 0x57b0c0
constexpr CommandSelectionStateLayout layouts[]{{layout0}, {layout1}};
// 50FFB6仅接受0..6；50FFCD将5/6减5，其余不变。
constexpr std::int32_t results[]{0, 1, 2, 3, 4, 0, 1};

// CRT 0x534b40 -> BSS 0x580954；原串VA 0x539974。
constexpr char confirmCue[] = "\x83\x4E\x83\x8A\x83\x62\x83\x4E\x89\xB9\x2E\x73\x65\x64";

// CRT 0x534b60 -> BSS 0x580958；原串VA 0x539984。
constexpr char hoverCue[] = "\x91\x49\x91\xF0\x89\xB9\x2E\x73\x65\x64";

constexpr CommandSelectionResourceProfile profile{
    buttons, navigation, backgrounds, {}, CommandSelectionAvailability::LayoutTable, layouts, results, confirmCue, hoverCue};
}

const CommandSelectionResourceProfile& Profile() noexcept { return profile; }
const char* ConfirmCue() noexcept { return confirmCue; }
const char* HoverCue() noexcept { return hoverCue; }
}
