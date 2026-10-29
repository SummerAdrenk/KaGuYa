#include "SettingPage01PreviewBlockNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace SettingPage01PreviewBlockNativeDetail;
void BuildSettingPage01PreviewTextNativeAt53AA70(Body& b, std::int32_t renderMode, std::int32_t shadow)
{
    // Unbound profile is an integration error, not a recoverable font-render failure.
    const auto& profile = CurrentSettingTextDisplayProfile().preview;
    try
    {
        auto& s = RequireSettingPage01PreviewBlockNativeServices();
        auto& pages = RequireSettingPage02NativeServices();
        GraphicsTextNativeFontParameters parameters; InitializeGraphicsTextFontNativeAt40C6E0(parameters);
        parameters.fontAt00.lfHeight = profile.fontHeight; parameters.fontAt00.lfWeight = profile.fontWeight;
        ::lstrcpynA(parameters.fontAt00.lfFaceName, profile.fontFace.data(), 32);
        parameters.textColorAt40 = profile.textColor;
        parameters.outlineColorAt48 = profile.outlineColor; parameters.outlineWidthAt54 = profile.outlineWidth;
        parameters.renderModeAt3E = profile.renderModes[renderMode != 0];
        parameters.shadowColorAt44 = shadow ? profile.shadowColor : profile.noShadowColor;
        const auto offset = shadow ? profile.shadowOffset : profile.noShadowOffset;
        parameters.shadowXAt4C = offset.x; parameters.shadowYAt50 = offset.y;
        parameters.characterSpacingAt3C = profile.characterSpacing; parameters.lineSpacingAt3D = profile.lineSpacing;
        parameters.verticalAt55 = static_cast<std::uint8_t>(profile.vertical);
        if (!s.text.CreateFontAt4388B0(&parameters)) throw 1;
        const auto* characters = profile.sample.data();
        // Explicit original byte count (or the documented 18 override), never Unicode character count.
        const auto size = -profile.fontHeight;
        const auto width = Signed((static_cast<std::uint32_t>(profile.sample.size() / 2) + 1u) * static_cast<unsigned>(size));
        // 40C2E0本身提供raw24构造失败清理；407A80失败后不补删已完成正文。
        auto* created = NewGraphicsSurfaceAt40C2E0(pages.surfaces, pages.strings);
        AdoptGraphicsSurfaceAt407A80(b.textAtA8, created);
        if (!Resolve<Surface>(b.textAtA8)->CreateAt415010(pages.surfaces, width, size, 1, 1)) throw 1;
        if (!s.text.InitializeAt4389D0(characters, 0, 0)) throw 1;
        const auto handle = Read(Resolve<Surface>(b.textAtA8)->handleAt04);
        if (!s.text.BlitAllAt53A980(handle, nullptr)) throw 1;
    }
    catch (...)
    {
        // 原53AC13/4155F0只在control存在时释放并清base/control；bias不清。
        // 清理自身可抛，不能套noexcept或伪装成文本创建成功。
        if (Read(b.textAtA8.controlAt08))
        { ReleaseOld(b.textAtA8); b.textAtA8.controlAt08 = nullptr; b.textAtA8.baseAt00 = 0; }
    }
}
}
