#include "LegacyMenuSelectionNative.Internal.h"

namespace KaGuYa::Start::Runtime::LegacyMenuSelectionDetail
{
void RenderLabels(Panel& p)
{
    auto& s = Services(p);
    GraphicsTextFontParameters font;
    // 原0x404D00/0x4FDCB6：32高、bold700、日文字集128、lineSpacing20、装饰偏移(1,2)。
    font.font.lfHeight = -32; font.font.lfWeight = 700;
    font.font.lfCharSet = 128; font.font.lfQuality = 2; font.font.lfPitchAndFamily = 0x30;
    ::lstrcpynA(font.font.lfFaceName, s.source.FontFace(), LF_FACESIZE);
    font.lineSpacing = 20; font.renderMode = 2;
    font.textColor = p.colors[0]; font.shadowColor = p.colors[2];
    font.outlineColor = 0xFFFFFFFFu; font.shadowX = 1; font.shadowY = 2;
    auto* image = reinterpret_cast<GraphicsSurfaceNativeBody*>(p.canvas.baseAt00);
    for (std::size_t i = 0; i < 3; ++i)
    {
        const int index = s.source.ReadValue(Profile(p).labelVariables[i]);
        // 原0x4FE128只把-1当无文字；其它负值不clamp或静默跳过。
        if (index == -1) continue;
        MessageTextCommandReference unusedName, body;
        RuntimeCStringArrayReference voices;
        const int read = s.source.ReadLabel(index, {unusedName, voices, body});
        if (read != 0 && read != 1) throw 1;
        if (!body) throw 1;
        // 原0x4FE24E：0为未读，1为已读；只变字色，buttons始终可用。
        font.textColor = p.colors[read == 0 ? 0 : 1];
        // 原0x4FE2BE没有检查字体返回值；文字Initialize失败仍按原throw 2外传。
        static_cast<void>(s.text.CreateTextFont(font));
        auto text = body->GetTextAt47D550();
        if (!s.text.InitializeText(text.GetString(), 0, 0)) throw 2;
        const auto rect = s.text.ReadTextDrawRect();
        s.text.SetTextBasePosition(p.labelPoints[i].x-image->originXAt10-(rect.right-rect.left)/2,
            p.labelPoints[i].y-image->originYAt14-(rect.bottom-rect.top)/2);
        for (int glyph = 0; glyph < s.text.GetTextLength(); ++glyph)
        {
            RECT drawn{};
            // 原0x4FE441忽略TextBlt返回值；不要把它伪造成字体/文字初始化的成功门。
            static_cast<void>(s.glyphs.BlitTextGlyphToSurface(image->handleAt04, static_cast<unsigned>(glyph), &drawn));
        }
    }
    // voices仅随消息局部引用退役，原主菜单不播放这些声音，也无hover/confirm cue。
}
}
