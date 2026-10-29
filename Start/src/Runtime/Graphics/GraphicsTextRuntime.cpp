#include <Runtime/GraphicsTextRuntime.h>

#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
GraphicsTextFontParameters TextDisplayFontSample18(const std::int32_t contextIndex)
{
    if (contextIndex < 0 || contextIndex >= 2)
        throw std::out_of_range("sample18 TextDisplay context outside 602198 table");

    GraphicsTextFontParameters result;
    // 602050 的完整 LOGFONTA 模板；4A2149..4A2156 以 context.height 的负数覆盖高度。
    result.font.lfHeight = -42;
    result.font.lfWeight = 700;
    result.font.lfCharSet = 128;
    result.font.lfQuality = 2;
    result.font.lfPitchAndFamily = 0x30;
    constexpr char face[] = "\x82\x6C\x82\x72 \x83\x53\x83\x56\x83\x62\x83\x4E";
    std::memcpy(result.font.lfFaceName, face, sizeof(face));
    result.characterSpacing = 1;
    result.lineSpacing = 2;
    result.renderMode = 2;
    // 60208C/602090 分别为黑、白；未覆盖的描边字段保留 40C6E0 的值。
    result.textColor = contextIndex == 0 ? 0u : 0x00FFFFFFu;
    result.shadowColor = 0xFFFFFFFFu;
    result.outlineColor = 0xFFFFFFFFu;
    return result;
}
}
