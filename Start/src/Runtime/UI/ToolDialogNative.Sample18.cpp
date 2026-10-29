#include <Runtime/ToolDialogNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
// PE604EE0..605128原表，宏仅拼接已逐字节核对的静态字符串，不作运行时资源推断。
#define PREFIX "\x83\x52\x83\x7D\x83\x93\x83\x68"
#define ON "\x83\x49\x83\x93"
#define OFF "\x83\x49\x83\x74"
#define TARGET "\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67"
#define CLICK "\x83\x4E\x83\x8A\x83\x62\x83\x4E"
#define HIT "\x94\xCD\x88\xCD"
#define PLAY "\x83\x76\x83\x8C\x83\x43"
#define AUTO "\x83\x49\x81\x5B\x83\x67"
#define SKIP "\x83\x58\x83\x4C\x83\x62\x83\x76"
#define NORMAL(label) { PREFIX ON label ".vrm", PREFIX OFF label ".vrm", nullptr, \
    PREFIX TARGET label ".vrm", PREFIX TARGET label ".vrm", PREFIX CLICK label ".vrm", PREFIX HIT label ".vrm" }
#define BUTTON(id, label) {{id, NORMAL(label), nullptr}, 6, -1}
constexpr GuiCheckBoxDescriptorNative2Ch first[]{
    BUTTON(8, "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68"),
    BUTTON(7, "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75"),
    BUTTON(1, "\x83\x8D\x81\x5B\x83\x68"),
    BUTTON(0, "\x83\x5A\x81\x5B\x83\x75")};
constexpr ToolCheckDescriptorNative48h checks[]{
    {4, NORMAL(AUTO), {PREFIX PLAY AUTO ".vrm", PREFIX OFF AUTO ".vrm", nullptr,
        PREFIX PLAY AUTO ".vrm", PREFIX PLAY AUTO ".vrm", PREFIX CLICK "2" AUTO ".vrm",
        PREFIX HIT AUTO ".vrm"}, nullptr, 6, 26},
    {5, NORMAL(SKIP), {PREFIX PLAY SKIP ".vrm", PREFIX PLAY OFF SKIP ".vrm", nullptr,
        PREFIX PLAY SKIP ".vrm", PREFIX PLAY SKIP ".vrm", PREFIX PLAY SKIP ".vrm",
        PREFIX HIT SKIP ".vrm"}, nullptr, 6, -1}};
constexpr GuiCheckBoxDescriptorNative2Ch third[]{
    BUTTON(6, "\x83\x7B\x83\x43\x83\x58"),
    BUTTON(9, "\x93\xA7\x96\xBE\x93\x78"),
    BUTTON(10, "\x8D\x44\x8A\xB4\x93\x78")};
constexpr GuiCheckBoxDescriptorNative2Ch title[]{BUTTON(11, "\x83\x5E\x83\x43\x83\x67\x83\x8B")};
constexpr GuiCheckBoxDescriptorNative2Ch stop[]{BUTTON(12, "\x83\x58\x83\x67\x83\x62\x83\x76")};
constexpr GuiCheckBoxDescriptorNative2Ch last[]{BUTTON(2, "\x94\xF1\x95\x5C\x8E\xA6")};
constexpr ToolControlGroupNative0Ch normal[]{
    {1, 4, first}, {2, 2, checks}, {1, 3, third}, {1, 1, title}, {1, 1, last}};
constexpr ToolControlGroupNative0Ch replay[]{
    {1, 4, first}, {2, 2, checks}, {1, 3, third}, {1, 1, stop}, {1, 1, last}};
const ToolDialogNativeProfile profile{normal, replay};
#undef BUTTON
#undef NORMAL
#undef SKIP
#undef AUTO
#undef PLAY
#undef HIT
#undef CLICK
#undef TARGET
#undef OFF
#undef ON
#undef PREFIX
}
const ToolDialogNativeProfile& Sample18ToolDialogNativeProfile() noexcept { return profile; }
}
