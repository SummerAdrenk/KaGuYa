#include "TitleData.h"

namespace KaGuYa::Start::Runtime::BakuTitleData
{
namespace
{
// 字符串逐字节来自Baku原PE，显式CP932；不受宿主ACP或编译器执行字符集影响。
// 原52D8C0：タイトル画面はじめからターゲット.vrm
constexpr char name0_1[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x82\xCD\x82\xB6\x82\xDF\x82\xA9\x82\xE7\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
// 原52D8E8：タイトル画面はじめからクリック.vrm
constexpr char name0_2[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x82\xCD\x82\xB6\x82\xDF\x82\xA9\x82\xE7\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
// 原52D90C：タイトル画面つづきからターゲット.vrm
constexpr char name1_1[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x82\xC2\x82\xC3\x82\xAB\x82\xA9\x82\xE7\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
// 原52D934：タイトル画面つづきからクリック.vrm
constexpr char name1_2[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x82\xC2\x82\xC3\x82\xAB\x82\xA9\x82\xE7\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
// 原52D958：タイトル画面CGモードターゲット.vrm
constexpr char name2_1[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x43\x47\x83\x82\x81\x5B\x83\x68\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
// 原52D97C：タイトル画面CGモードクリック.vrm
constexpr char name2_2[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x43\x47\x83\x82\x81\x5B\x83\x68\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
// 原52D9A0：タイトル画面CGモードオフ.vrm
constexpr char name2_3[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x43\x47\x83\x82\x81\x5B\x83\x68\x83\x49\x83\x74\x2E\x76\x72\x6D";
// 原52D9C0：タイトル画面回想モードターゲット.vrm
constexpr char name3_1[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x89\xF1\x91\x7A\x83\x82\x81\x5B\x83\x68\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
// 原52D9E8：タイトル画面回想モードクリック.vrm
constexpr char name3_2[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x89\xF1\x91\x7A\x83\x82\x81\x5B\x83\x68\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
// 原52DA0C：タイトル画面回想モードオフ.vrm
constexpr char name3_3[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x89\xF1\x91\x7A\x83\x82\x81\x5B\x83\x68\x83\x49\x83\x74\x2E\x76\x72\x6D";
// 原52DA2C：タイトル画面音楽鑑賞ターゲット.vrm
constexpr char name4_1[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
// 原52DA50：タイトル画面音楽鑑賞クリック.vrm
constexpr char name4_2[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
// 原52DA74：タイトル画面音楽鑑賞オフ.vrm
constexpr char name4_3[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x83\x49\x83\x74\x2E\x76\x72\x6D";
// 原52DA94：タイトル画面ゲームを終了ターゲット.vrm
constexpr char name5_1[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x83\x51\x81\x5B\x83\x80\x82\xF0\x8F\x49\x97\xB9\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
// 原52DABC：タイトル画面ゲームを終了クリック.vrm
constexpr char name5_2[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x83\x51\x81\x5B\x83\x80\x82\xF0\x8F\x49\x97\xB9\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
// 原52D8A0：タイトル画面.vrm
constexpr char background[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x2E\x76\x72\x6D";
// 原52F504：クリック音.sed
constexpr char click[] = "\x83\x4E\x83\x8A\x83\x62\x83\x4E\x89\xB9\x2E\x73\x65\x64";
// 原52F514：選択音.sed
constexpr char select[] = "\x91\x49\x91\xF0\x89\xB9\x2E\x73\x65\x64";
const LegacyTitleMenuData data{
    {{
        {{nullptr, name0_1, name0_2, nullptr, nullptr, nullptr, nullptr}, {0, -1}, 0},
        {{nullptr, name1_1, name1_2, nullptr, nullptr, nullptr, nullptr}, {0, -1}, 0},
        {{nullptr, name2_1, name2_2, name2_3, nullptr, nullptr, nullptr}, {0, -1}, 0},
        {{nullptr, name3_1, name3_2, name3_3, nullptr, nullptr, nullptr}, {0, -1}, 0},
        {{nullptr, name4_1, name4_2, name4_3, nullptr, nullptr, nullptr}, {0, -1}, 0},
        {{nullptr, name5_1, name5_2, nullptr, nullptr, nullptr, nullptr}, {0, -1}, 0}
    }},
    {{5, 1, -1, -1, 0, 2, -1, -1, 1, 3, -1, -1,
      2, 4, -1, -1, 3, 5, -1, -1, 4, 0, -1, -1}},
    {{1, 2, 3, 4}}
};
// 5254C0/5254F0/525230/525260：运行期根可覆盖这些默认名称。
const LegacyTitleMenuNames names{background, "", click, select};
// 原52F99C与52D8B4；资源名属于游戏数据，不改变共享场景/音频算法。
const LegacyTitleSceneData scene{data, names, "bg_black.bgd", "BGM19.bgm"};
const GuiStaticDescriptorNative24h sharedBackground{
    1, {{background, nullptr, nullptr, background, background, nullptr, nullptr}}, nullptr};
const std::array<GuiButtonDescriptorNative2Ch, 6> sharedButtons = [] {
    std::array<GuiButtonDescriptorNative2Ch, 6> result{};
    for (std::size_t i = 0; i < result.size(); ++i)
    {
        const auto& original = data.buttons[i];
        auto& button = result[i];
        button.common.eventAt00 = static_cast<std::int32_t>(i) + 3;
        // 旧slot0为空，普通按钮已画在背景上；新版3/4为focus/hover，5为pressed。
        button.common.namesAt04 = {{original.namesAt00[0], original.namesAt00[3], nullptr,
            original.namesAt00[1], original.namesAt00[1], original.namesAt00[2], nullptr}};
        // 旧命中使用第一张已载图片(target)的alpha；对应新版资源slot3。
        button.hitSlotAt24 = 3;
        button.childIdAt28 = -1;
    }
    return result;
}();
}
const LegacyTitleMenuData& Menu() noexcept { return data; }
const LegacyTitleMenuNames& Names() noexcept { return names; }
const LegacyTitleSceneData& Scene() noexcept { return scene; }
const GuiStaticDescriptorNative24h& SharedBackground() noexcept { return sharedBackground; }
std::span<const GuiButtonDescriptorNative2Ch, 6> SharedButtons() noexcept { return sharedButtons; }
}
