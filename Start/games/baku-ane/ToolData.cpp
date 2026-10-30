#include "ToolData.h"

namespace KaGuYa::Start::Runtime::BakuToolData
{
namespace
{
// Baku PE 532EA8..53381C的CP932资源名；不是独立的popup菜单表。
#define COMMAND "\x83\x52\x83\x7D\x83\x93\x83\x68"
#define ON "\x83\x49\x83\x93"
#define OFF "\x83\x49\x83\x74"
#define TARGET "\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67"
#define CLICK "\x83\x4E\x83\x8A\x83\x62\x83\x4E"
#define HIT "\x94\xCD\x88\xCD"
#define PLAY "\x83\x76\x83\x8C\x83\x43"
#define AUTO "\x83\x49\x81\x5B\x83\x67"
#define SKIP "\x83\x58\x83\x4C\x83\x62\x83\x76"
#define OPACITY "\x93\xA7\x96\xBE\x93\x78"
#define NORMAL(label) {COMMAND ON label ".vrm", COMMAND OFF label ".vrm", nullptr, \
    COMMAND TARGET label ".vrm", COMMAND TARGET label ".vrm", COMMAND CLICK label ".vrm", \
    COMMAND HIT label ".vrm"}
#define BUTTON(event, label) {{event, NORMAL(label), nullptr}, 6, -1}

// 56E7D8..56E884：空位置指针保留资源内的原点，不追加(0,0)平移。
constexpr GuiCheckBoxDescriptorNative2Ch saveLoad[]{
    BUTTON(8, "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68"),
    BUTTON(7, "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75"),
    BUTTON(1, "\x83\x8D\x81\x5B\x83\x68"),
    BUTTON(0, "\x83\x5A\x81\x5B\x83\x75")};
// 56E990/56E9D8：选中资源与child ID独立于事件编号。
constexpr ToolCheckDescriptorNative48h playback[]{
    {4, NORMAL(AUTO), {COMMAND PLAY AUTO ".vrm", COMMAND OFF AUTO ".vrm", nullptr,
        COMMAND PLAY AUTO ".vrm", COMMAND PLAY AUTO ".vrm", COMMAND CLICK "2" AUTO ".vrm",
        COMMAND HIT AUTO ".vrm"}, nullptr, 6, 26},
    {5, NORMAL(SKIP), {COMMAND PLAY SKIP ".vrm", COMMAND PLAY OFF SKIP ".vrm", nullptr,
        COMMAND PLAY SKIP ".vrm", COMMAND PLAY SKIP ".vrm", COMMAND PLAY SKIP ".vrm",
        COMMAND HIT SKIP ".vrm"}, nullptr, 6, -1}};
constexpr GuiCheckBoxDescriptorNative2Ch display[]{
    BUTTON(6, "\x83\x7B\x83\x43\x83\x58"),
    // 56E8B4的slot5指向5332AC：复用TARGET，不引用缺失的CLICK透明度资源。
    {{9, {COMMAND ON OPACITY ".vrm", COMMAND OFF OPACITY ".vrm", nullptr,
        COMMAND TARGET OPACITY ".vrm", COMMAND TARGET OPACITY ".vrm",
        COMMAND TARGET OPACITY ".vrm", COMMAND HIT OPACITY ".vrm"}, nullptr}, 6, -1},
    BUTTON(10, "\x8D\x44\x8A\xB4\x93\x78")};
constexpr GuiCheckBoxDescriptorNative2Ch title[]{BUTTON(11, "\x83\x5E\x83\x43\x83\x67\x83\x8B")};
constexpr GuiCheckBoxDescriptorNative2Ch stop[]{BUTTON(12, "\x83\x58\x83\x67\x83\x62\x83\x76")};
constexpr GuiCheckBoxDescriptorNative2Ch hide[]{BUTTON(2, "\x94\xF1\x95\x5C\x8E\xA6")};
// 4C9F30将事件0..12映射为{3,2,11,-1,4,5,6,1,0,7,8,9,10}，
// 与共享TickToolWindowNativeAt4BE200的动作协议一致。
constexpr ToolControlGroupNative0Ch normal[]{
    {1, 4, saveLoad}, {2, 2, playback}, {1, 3, display}, {1, 1, title}, {1, 1, hide}};
constexpr ToolControlGroupNative0Ch replay[]{
    {1, 4, saveLoad}, {2, 2, playback}, {1, 3, display}, {1, 1, stop}, {1, 1, hide}};
static_assert(std::size(normal) == 5 && std::size(replay) == 5);
constexpr ToolDialogNativeProfile profile{normal, replay};
// 4E2940读取56EA60主控件与56EA5C关联控件；两值与共享关系表一致。
constexpr ToolDialogNativeRelation relation{100, 10};

#undef BUTTON
#undef NORMAL
#undef OPACITY
#undef SKIP
#undef AUTO
#undef PLAY
#undef HIT
#undef CLICK
#undef TARGET
#undef OFF
#undef ON
#undef COMMAND
}
const ToolDialogNativeProfile& Profile() noexcept { return profile; }
const ToolDialogNativeRelation& Relation() noexcept { return relation; }
}
