#pragma once

#include <Runtime/GraphicsSurfaceRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 4388B0 使用字体描述的 +60..+85；这里只表达字段，不向原程序暴露 C++ 布局。
struct GraphicsTextFontParameters final
{
    LOGFONTA font{};
    std::uint8_t characterSpacing = 0;
    std::uint8_t lineSpacing = 0;
    std::uint8_t renderMode = 0;
    bool vertical = false;
    COLORREF textColor = 0;
    COLORREF shadowColor = 0;
    std::int32_t shadowX = 0;
    std::int32_t shadowY = 0;
    COLORREF outlineColor = 0;
    std::uint8_t outlineWidth = 0;
};

class IGraphicsTextApi
{
public:
    virtual ~IGraphicsTextApi() = default;
    [[nodiscard]] virtual bool CreateTextFont(
        const GraphicsTextFontParameters& parameters) = 0;
    [[nodiscard]] virtual bool InitializeText(
        const char* textCp932, std::int32_t x, std::int32_t y) = 0;
    [[nodiscard]] virtual RECT ReadTextDrawRect() = 0;
    [[nodiscard]] virtual std::int32_t GetTextLength() = 0;
    virtual void SetTextBasePosition(std::int32_t x, std::int32_t y) = 0;
    [[nodiscard]] virtual bool BlitAllTextToSurface(
        GraphicsSurfaceHandle target, RECT* outputRect) = 0;
};

// 419090 每次提交一个字形；与整段 TextBltAll 分离，不强迫只测量的调用者实现绘制。
class IGraphicsTextGlyphApi
{
public:
    virtual ~IGraphicsTextGlyphApi() = default;
    [[nodiscard]] virtual bool BlitTextGlyphToSurface(GraphicsSurfaceHandle target,
        std::uint32_t index, RECT* outputRect) = 0;
};

// 4A1F30 / 602198：样本 18 的两个 context；越界不是默认字体选择。
[[nodiscard]] GraphicsTextFontParameters TextDisplayFontSample18(
    std::int32_t contextIndex);
}
