#include <Runtime/SystemDialogNative.h>

// 权威PE SHA256 FBD6288599ABD4BF3CF3FC9717602DD81E056EE4F795955569DBC77FD7EAF277。
// VA只作审计数据，不在运行期强转或读取原EXE。CP932字节保留，编译器不重新编码。
namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr char resource5BB6D8[] = "\x76\x72\x6D\x3A\x5C\x83\x81\x83\x6A\x83\x85\x81\x5B\x98\x67\x2E\x61\x6C\x70"; // vrm:\メニュー枠.alp
constexpr char resource5BB6EC[] = "\x76\x72\x6D\x3A\x5C\x83\x81\x83\x6A\x83\x85\x81\x5B\x83\x49\x83\x93\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x89\xE6\x96\xCA\x2E\x61\x6C\x70"; // vrm:\メニューオンウィンドウ画面.alp
constexpr char resource5BB710[] = "\x76\x72\x6D\x3A\x5C\x83\x81\x83\x6A\x83\x85\x81\x5B\x83\x49\x83\x74\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x89\xE6\x96\xCA\x2E\x61\x6C\x70"; // vrm:\メニューオフウィンドウ画面.alp
constexpr char resource5BB734[] = "\x76\x72\x6D\x3A\x5C\x83\x81\x83\x6A\x83\x85\x81\x5B\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x89\xE6\x96\xCA\x2E\x61\x6C\x70"; // vrm:\メニューターゲットウィンドウ画面.alp
constexpr char resource5BB760[] = "\x76\x72\x6D\x3A\x5C\x83\x81\x83\x6A\x83\x85\x81\x5B\x83\x4E\x83\x8A\x83\x62\x83\x4E\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x89\xE6\x96\xCA\x2E\x61\x6C\x70"; // vrm:\メニュークリックウィンドウ画面.alp
constexpr char resource5BB788[] = "\x76\x72\x6D\x3A\x5C\x83\x81\x83\x6A\x83\x85\x81\x5B\x94\xCD\x88\xCD\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x89\xE6\x96\xCA\x2E\x61\x6C\x70"; // vrm:\メニュー範囲ウィンドウ画面.alp
constexpr char resource5BB7AC[] = "\x76\x72\x6D\x3A\x5C\x83\x81\x83\x6A\x83\x85\x81\x5B\x83\x49\x83\x93\x83\x74\x83\x8B\x83\x58\x83\x4E\x83\x8A\x81\x5B\x83\x93\x89\xE6\x96\xCA\x2E\x61\x6C\x70"; // vrm:\メニューオンフルスクリーン画面.alp
constexpr char resource5BB7D4[] = "\x76\x72\x6D\x3A\x5C\x83\x81\x83\x6A\x83\x85\x81\x5B\x83\x49\x83\x74\x83\x74\x83\x8B\x83\x58\x83\x4E\x83\x8A\x81\x5B\x83\x93\x89\xE6\x96\xCA\x2E\x61\x6C\x70"; // vrm:\メニューオフフルスクリーン画面.alp
constexpr char resource5BB7FC[] = "\x76\x72\x6D\x3A\x5C\x83\x81\x83\x6A\x83\x85\x81\x5B\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x83\x74\x83\x8B\x83\x58\x83\x4E\x83\x8A\x81\x5B\x83\x93\x89\xE6\x96\xCA\x2E\x61\x6C\x70"; // vrm:\メニューターゲットフルスクリーン画面.alp
constexpr char resource5BB82C[] = "\x76\x72\x6D\x3A\x5C\x83\x81\x83\x6A\x83\x85\x81\x5B\x83\x4E\x83\x8A\x83\x62\x83\x4E\x83\x74\x83\x8B\x83\x58\x83\x4E\x83\x8A\x81\x5B\x83\x93\x89\xE6\x96\xCA\x2E\x61\x6C\x70"; // vrm:\メニュークリックフルスクリーン画面.alp
constexpr char resource5BB858[] = "\x76\x72\x6D\x3A\x5C\x83\x81\x83\x6A\x83\x85\x81\x5B\x94\xCD\x88\xCD\x83\x74\x83\x8B\x83\x58\x83\x4E\x83\x8A\x81\x5B\x83\x93\x89\xE6\x96\xCA\x2E\x61\x6C\x70"; // vrm:\メニュー範囲フルスクリーン画面.alp
constexpr char resource5BB880[] = "\x76\x72\x6D\x3A\x5C\x83\x81\x83\x6A\x83\x85\x81\x5B\x83\x49\x83\x93\x90\xDD\x92\xE8\x89\xE6\x96\xCA\x2E\x61\x6C\x70"; // vrm:\メニューオン設定画面.alp
constexpr char resource5BB8A0[] = "\x76\x72\x6D\x3A\x5C\x83\x81\x83\x6A\x83\x85\x81\x5B\x83\x49\x83\x74\x90\xDD\x92\xE8\x89\xE6\x96\xCA\x2E\x61\x6C\x70"; // vrm:\メニューオフ設定画面.alp
constexpr char resource5BB8C0[] = "\x76\x72\x6D\x3A\x5C\x83\x81\x83\x6A\x83\x85\x81\x5B\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x90\xDD\x92\xE8\x89\xE6\x96\xCA\x2E\x61\x6C\x70"; // vrm:\メニューターゲット設定画面.alp
constexpr char resource5BB8E4[] = "\x76\x72\x6D\x3A\x5C\x83\x81\x83\x6A\x83\x85\x81\x5B\x83\x4E\x83\x8A\x83\x62\x83\x4E\x90\xDD\x92\xE8\x89\xE6\x96\xCA\x2E\x61\x6C\x70"; // vrm:\メニュークリック設定画面.alp
constexpr char resource5BB908[] = "\x76\x72\x6D\x3A\x5C\x83\x81\x83\x6A\x83\x85\x81\x5B\x94\xCD\x88\xCD\x90\xDD\x92\xE8\x89\xE6\x96\xCA\x2E\x61\x6C\x70"; // vrm:\メニュー範囲設定画面.alp
constexpr char resource5BB928[] = "\x76\x72\x6D\x3A\x5C\x83\x81\x83\x6A\x83\x85\x81\x5B\x83\x49\x83\x93\x83\x51\x81\x5B\x83\x80\x8F\x49\x97\xB9\x2E\x61\x6C\x70"; // vrm:\メニューオンゲーム終了.alp
constexpr char resource5BB948[] = "\x76\x72\x6D\x3A\x5C\x83\x81\x83\x6A\x83\x85\x81\x5B\x83\x49\x83\x74\x83\x51\x81\x5B\x83\x80\x8F\x49\x97\xB9\x2E\x61\x6C\x70"; // vrm:\メニューオフゲーム終了.alp
constexpr char resource5BB968[] = "\x76\x72\x6D\x3A\x5C\x83\x81\x83\x6A\x83\x85\x81\x5B\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x83\x51\x81\x5B\x83\x80\x8F\x49\x97\xB9\x2E\x61\x6C\x70"; // vrm:\メニューターゲットゲーム終了.alp
constexpr char resource5BB990[] = "\x76\x72\x6D\x3A\x5C\x83\x81\x83\x6A\x83\x85\x81\x5B\x83\x4E\x83\x8A\x83\x62\x83\x4E\x83\x51\x81\x5B\x83\x80\x8F\x49\x97\xB9\x2E\x61\x6C\x70"; // vrm:\メニュークリックゲーム終了.alp
constexpr char resource5BB9B4[] = "\x76\x72\x6D\x3A\x5C\x83\x81\x83\x6A\x83\x85\x81\x5B\x94\xCD\x88\xCD\x83\x51\x81\x5B\x83\x80\x8F\x49\x97\xB9\x2E\x61\x6C\x70"; // vrm:\メニュー範囲ゲーム終了.alp
constexpr char settings[] = "\x90\xDD\x92\xE8\x89\xE6\x96\xCA"; // 5B4E68：設定画面，显式8字节。
constexpr std::array<std::uint32_t, 9> rawStatic{ // 606978..606998。
    0x00000000u, 0x005BB6D8u, 0x00000000u, 0x00000000u, 0x005BB6D8u, 0x005BB6D8u, 0x00000000u, 0x00000000u, 0x00000000u
};
// 紧邻描述符的PE原字节；4F41A0不读取此区域，不能据此凭空添加navigation。
[[maybe_unused]] constexpr std::array<std::uint32_t, 5> adjacent60699C{8, 11, 6, 10, 9};
[[maybe_unused]] constexpr std::array<std::uint32_t, 4> endDispatch5095F4{
    0x0050956Cu, 0x0050958Au, 0x005095A8u, 0x005095DAu
};
constexpr std::array<std::array<std::uint32_t, 11>, 4> rawButtons{{ // 6069B0，步长2Ch。
    {{0x00000001u, 0x005BB6ECu, 0x005BB710u, 0x00000000u, 0x005BB734u, 0x005BB734u, 0x005BB760u, 0x005BB788u, 0x00000000u, 0x00000006u, 0xFFFFFFFFu}},
    {{0x00000002u, 0x005BB7ACu, 0x005BB7D4u, 0x00000000u, 0x005BB7FCu, 0x005BB7FCu, 0x005BB82Cu, 0x005BB858u, 0x00000000u, 0x00000006u, 0xFFFFFFFFu}},
    {{0x00000003u, 0x005BB880u, 0x005BB8A0u, 0x00000000u, 0x005BB8C0u, 0x005BB8C0u, 0x005BB8E4u, 0x005BB908u, 0x00000000u, 0x00000006u, 0xFFFFFFFFu}},
    {{0x00000004u, 0x005BB928u, 0x005BB948u, 0x00000000u, 0x005BB968u, 0x005BB968u, 0x005BB990u, 0x005BB9B4u, 0x00000000u, 0x00000006u, 0xFFFFFFFFu}},
}};
const GuiStaticDescriptorNative24h frame{
    0, {resource5BB6D8, nullptr, nullptr, resource5BB6D8, resource5BB6D8, nullptr, nullptr}, nullptr
};
const std::array<GuiButtonDescriptorNative2Ch, 4> buttons{{
    {{1, {resource5BB6EC, resource5BB710, nullptr, resource5BB734, resource5BB734, resource5BB760, resource5BB788}, nullptr}, 6, -1},
    {{2, {resource5BB7AC, resource5BB7D4, nullptr, resource5BB7FC, resource5BB7FC, resource5BB82C, resource5BB858}, nullptr}, 6, -1},
    {{3, {resource5BB880, resource5BB8A0, nullptr, resource5BB8C0, resource5BB8C0, resource5BB8E4, resource5BB908}, nullptr}, 6, -1},
    {{4, {resource5BB928, resource5BB948, nullptr, resource5BB968, resource5BB968, resource5BB990, resource5BB9B4}, nullptr}, 6, -1},
}};
constexpr std::array<std::uint32_t, 38> original{ // PE 5BE868。
    0x00509ED0u, 0x004FE5F0u, 0x005025A0u, 0x0050A5D0u, // +00
    0x004FF020u, 0x00509D20u, 0x00536310u, 0x00536500u, // +10
    0x0050FF40u, 0x00538660u, 0x00510630u, 0x00500800u, // +20
    0x00532170u, 0x0051E740u, 0x00510BA0u, 0x005106D0u, // +30
    0x00511610u, 0x0050F990u, 0x0052DBC0u, 0x00509550u, // +40
    0x0050AB70u, 0x0051B120u, 0x005267E0u, 0x005231C0u, // +50
    0x005230B0u, 0x0050FC80u, 0x00522E90u, 0x00510030u, // +60
    0x005100E0u, 0x0050A6A0u, 0x0050B920u, 0x00510220u, // +70
    0x00510170u, 0x005102B0u, 0x00518A80u, 0x005086B0u, // +80
    0x00525470u, 0x0051C100u, // +90
};
constexpr std::array<std::uint32_t, 38> modal{ // PE 5BE1F0。
    0x00509ED0u, 0x004FCB60u, 0x005025A0u, 0x0050A5D0u, // +00
    0x004FF020u, 0x00509D20u, 0x00536310u, 0x00536500u, // +10
    0x0050FF40u, 0x00538660u, 0x00510630u, 0x00500800u, // +20
    0x00532170u, 0x0051E740u, 0x00510BA0u, 0x005106D0u, // +30
    0x00511610u, 0x0050CF50u, 0x0052DBC0u, 0x00508E80u, // +40
    0x0050AB70u, 0x00518C30u, 0x005267E0u, 0x005231C0u, // +50
    0x005230B0u, 0x0050FC80u, 0x00522E90u, 0x00510030u, // +60
    0x005100E0u, 0x0050A6A0u, 0x0050B920u, 0x00510220u, // +70
    0x00510170u, 0x005102B0u, 0x00518A80u, 0x005086B0u, // +80
    0x00541C5Fu, 0x0051C100u, // +90
};
constexpr std::array<std::uint32_t, 7> input{ // PE 5C0C20。
    0x0050FFB0u, 0x00510080u, 0x0050A650u, 0x0050B8E0u, // +00
    0x005101D0u, 0x00510130u, 0x00510270u, // +10
};
// 5F6848 -> 5F686C：state, previous, unwind VA。4/6/7/8是正常尾terminate态。
[[maybe_unused]] constexpr std::array<std::array<std::uint32_t, 3>, 9> ctorUnwind{{
    {{0, 0xFFFFFFFFu, 0x005A3110u}},
    {{1, 0, 0x005A3118u}},
    {{2, 1, 0x005A3120u}},
    {{3, 2, 0x005A3128u}},
    {{4, 2, 0x00541E55u}},
    {{5, 2, 0x005A3130u}},
    {{6, 2, 0x00541E55u}},
    {{7, 1, 0x00541E55u}},
    {{8, 0, 0x00541E55u}},
}};
// 原FuncInfo指向的完整unwind map，成对保存(previous, action)，不是现代EH状态。
[[maybe_unused]] constexpr std::array<std::uint32_t, 2> destructor5F48C4{0xFFFFFFFFu, 0x00541E55u};
[[maybe_unused]] constexpr std::array<std::uint32_t, 2> deleting5F6960{0xFFFFFFFFu, 0x00541E55u};
[[maybe_unused]] constexpr std::array<std::uint32_t, 2> active5F48F0{0xFFFFFFFFu, 0x005A65D0u};
[[maybe_unused]] constexpr std::array<std::uint32_t, 4> beginModal5F491C{
    0xFFFFFFFFu, 0x005A61A0u, 0xFFFFFFFFu, 0x00541E55u
};
[[maybe_unused]] constexpr std::array<std::uint32_t, 8> drawing5F4950{
    0xFFFFFFFFu, 0x005A7FF0u, 0xFFFFFFFFu, 0x005A7FF8u,
    0xFFFFFFFFu, 0x00541E55u, 0xFFFFFFFFu, 0x005A8000u
};
[[maybe_unused]] constexpr std::array<std::uint32_t, 12> beginSystem5F6904{
    0xFFFFFFFFu, 0x005A6480u, 0xFFFFFFFFu, 0x00541E55u,
    0xFFFFFFFFu, 0x005A6488u, 0xFFFFFFFFu, 0x00541E55u,
    0xFFFFFFFFu, 0x005A6490u, 0xFFFFFFFFu, 0x00541E55u
};
[[maybe_unused]] constexpr std::array<std::uint32_t, 2> trigger5F6934{0xFFFFFFFFu, 0x005A6510u};
}
const GuiStaticDescriptorNative24h& SystemDialogNativeStaticSample18() noexcept { return frame; }
std::span<const GuiButtonDescriptorNative2Ch, 4> SystemDialogNativeButtonsSample18() noexcept { return buttons; }
std::span<const std::uint32_t, 38> SystemDialogNativeOriginalVtableSample18() noexcept { return original; }
std::span<const std::uint32_t, 38> SystemDialogNativeOriginalModalVtableSample18() noexcept { return modal; }
std::span<const std::uint32_t, 9> SystemDialogNativeRawStaticSample18() noexcept { return rawStatic; }
std::span<const std::array<std::uint32_t, 11>, 4> SystemDialogNativeRawButtonsSample18() noexcept { return rawButtons; }
std::span<const std::uint32_t, 7> SystemDialogNativeOriginalInputVtableSample18() noexcept { return input; }
std::span<const char, 8> SystemDialogNativeSettingsCommandSample18() noexcept
{ return std::span<const char, 8>{settings, 8}; }
}
