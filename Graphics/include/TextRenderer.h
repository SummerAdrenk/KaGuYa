#pragma once

#include "SurfaceReference.h"
#include <Windows.h>

#include <cstdint>
#include <memory>
#include <string_view>

namespace KaGuYa::Graphics
{
class SurfaceInfo;
struct TextState;
struct TextStateDeleter
{
    void operator()(TextState* state) const;
};
using TextRendererOwner = std::unique_ptr<TextState, TextStateDeleter>;
TextRendererOwner CreateTextRenderer();

struct TextFontOptions
{
    LOGFONTA logFont{};
    std::uint8_t characterSpacing = 0;
    std::uint8_t lineSpacing = 0;
    std::uint8_t renderMode = 1;
    bool vertical = false;
    COLORREF textColor = RGB(255, 255, 255);
    COLORREF shadowColor = CLR_INVALID;
    int shadowOffsetX = 0;
    int shadowOffsetY = 0;
    COLORREF outlineColor = CLR_INVALID;
    std::uint8_t outlineWidth = 0;
};

bool ConfigureTextFont(const TextFontOptions& options);
int SetTextBasePosition(int x, int y);
const char* SetTextName(const char* name, bool second);
bool InitializeTextLayout(const char* text, int baseX, int baseY);
int GetTextGlyphCount();
RECT GetTextBounds();
bool DrawTextGlyph(SurfaceInfo& destination, unsigned int index, bool blend, RECT* drawnRect);
bool DrawAllText(SurfaceInfo& destination, bool blend, RECT* drawnRect);
// 原1003CAA0直接使用调用者返回槽。须传入手工存储中的空Ref，不可同时带调用者自动析构。
// 异常后该槽已结束生命周期，调用者不可再退它；正常返回后才接管其退休责任。
// 原1000E5A0导出使用同一输出槽，不经值返回/复制，也不依赖NRVO。
void CreateCharacterSurfaceInto(SurfaceReference& result, std::uint16_t character);
void ResetTextRenderer();
}
