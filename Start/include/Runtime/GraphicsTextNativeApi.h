#pragma once

#include <Runtime/GraphicsRootNative.h>
#include <Runtime/GraphicsTextRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 4388B0读取到+55；这是带显式未写槽的传参正文，不把现代参数类当成原布局。
struct GraphicsTextNativeFontParameters final
{
    LOGFONTA fontAt00;
    std::uint8_t characterSpacingAt3C;
    std::uint8_t lineSpacingAt3D;
    std::uint8_t renderModeAt3E;
    std::uint8_t unwrittenAt3F;
    COLORREF textColorAt40;
    COLORREF shadowColorAt44;
    COLORREF outlineColorAt48;
    std::int32_t shadowXAt4C;
    std::int32_t shadowYAt50;
    std::uint8_t outlineWidthAt54;
    std::uint8_t verticalAt55;
    std::uint8_t alignmentAt56[2];
};
static_assert(sizeof(LOGFONTA) == 0x3C);
static_assert(sizeof(GraphicsTextNativeFontParameters) == 0x58);
static_assert(offsetof(GraphicsTextNativeFontParameters, characterSpacingAt3C) == 0x3C);
static_assert(offsetof(GraphicsTextNativeFontParameters, renderModeAt3E) == 0x3E);
static_assert(offsetof(GraphicsTextNativeFontParameters, textColorAt40) == 0x40);
static_assert(offsetof(GraphicsTextNativeFontParameters, shadowColorAt44) == 0x44);
static_assert(offsetof(GraphicsTextNativeFontParameters, outlineColorAt48) == 0x48);
static_assert(offsetof(GraphicsTextNativeFontParameters, shadowXAt4C) == 0x4C);
static_assert(offsetof(GraphicsTextNativeFontParameters, shadowYAt50) == 0x50);
static_assert(offsetof(GraphicsTextNativeFontParameters, outlineWidthAt54) == 0x54);
static_assert(offsetof(GraphicsTextNativeFontParameters, verticalAt55) == 0x55);
GraphicsTextNativeFontParameters* InitializeGraphicsTextFontNativeAt40C6E0(
    GraphicsTextNativeFontParameters& parameters) noexcept;

// 只借服务；每次取当前Graphics根和module，不缓存导出、不补缺失导出的替代行为。
class GraphicsTextNativeApi final : public IGraphicsTextApi, public IGraphicsTextGlyphApi
{
public:
    GraphicsTextNativeApi(GraphicsRootOwner& owner, IRuntimeStringManagerSource& strings,
        IGraphicsModuleNativeWin32Api& win32) noexcept;
    [[nodiscard]] bool CreateTextFontNativeAt4388B0(
        const GraphicsTextNativeFontParameters* parameters);
    // 现代调用者仍按字段转换；原生TextWin必须调用上面的真实正文入口。
    [[nodiscard]] bool CreateTextFont(const GraphicsTextFontParameters& parameters) override;
    [[nodiscard]] bool InitializeText(const char* textCp932,
        std::int32_t x, std::int32_t y) override;
    RECT* ReadTextDrawRectNativeAt418A30(RECT* output);
    [[nodiscard]] RECT ReadTextDrawRect() override;
    [[nodiscard]] std::int32_t GetTextLength() override;
    void SetTextBasePosition(std::int32_t x, std::int32_t y) override;
    [[nodiscard]] bool BlitAllTextToSurface(GraphicsSurfaceHandle target, RECT* output) override;
    [[nodiscard]] bool BlitTextGlyphToSurface(GraphicsSurfaceHandle target,
        std::uint32_t index, RECT* output) override;

private:
    [[nodiscard]] bool CaptureModule(GraphicsModuleNativeBody8h*& module);
    GraphicsRootOwner& owner_;
    IRuntimeStringManagerSource& strings_;
    IGraphicsModuleNativeWin32Api& win32_;
};
}
