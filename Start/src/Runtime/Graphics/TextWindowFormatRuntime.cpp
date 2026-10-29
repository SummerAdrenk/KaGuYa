#include <Runtime/TextWindowFormatRuntime.h>
#include <Runtime/ParamsRuntimeProfile.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <Runtime/ScrVmValueTable.h>
#include <Logging/Logger.h>

#include <atlbase.h>
#include <locale.h>
#include <mbstring.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
class Cp932Names final
{
public:
    Cp932Names() : locale_(_create_locale(LC_CTYPE, ".932"))
    {
        if (!locale_) throw std::runtime_error("CTextWin name comparison requires CP932 locale");
    }
    ~Cp932Names() { _free_locale(locale_); }
    Cp932Names(const Cp932Names&) = delete;
    Cp932Names& operator=(const Cp932Names&) = delete;
    bool Different(const RuntimeCString& current, const char* original) const
    {
        // 4C27C3/4C2804只判原默认名指针；null经4C29C0抛E_FAIL，不等同空串。
        if (!original) throw ATL::CAtlException(E_FAIL);
        return _mbscmp_l(reinterpret_cast<const unsigned char*>(current.GetString()),
            reinterpret_cast<const unsigned char*>(original), locale_) != 0;
    }
private:
    _locale_t locale_;
};
}

Win32TextWindowFormatErrorDisplay::Win32TextWindowFormatErrorDisplay(
    const HWND& window, const std::string& titleCp932) noexcept : window_(window), title_(titleCp932) {}

void Win32TextWindowFormatErrorDisplay::ShowMessageReadError()
{
    // 5B48F8..5B491F 原 CP932 字节；4C28A3 使用原 HWND/caption、MB_ICONERROR。
    constexpr char message[] = "\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x66"
        "\x81\x5B\x83\x5E\x82\xCC\x93\xC7\x82\xDD\x8D\x9E\x82\xDD\x82\xC9"
        "\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD";
    static_cast<void>(::MessageBoxA(window_, message, title_.c_str(), MB_ICONERROR));
}

TextWindowFormatRuntime::TextWindowFormatRuntime(TextWindowLayerRuntime& window,
    MessageFormatSelectionRuntime& messages, const ParamsGameSystem& params,
    const SaveDataEnvironmentArchive& save, UnSaveDataRuntime& unsave,
    IGraphicsTextApi& text, IGraphicsTextGlyphApi& glyphs, ITextWindowTextServices& layout,
    ITextWindowFormatErrorDisplay& errors) noexcept
    : window_(window), messages_(messages), params_(params), save_(save), unsave_(unsave),
      text_(text), glyphs_(glyphs), layout_(layout), errors_(errors) {}

bool TextWindowFormatRuntime::SetFormatAt4C2740(std::int32_t index, bool initialize)
{
    auto& state = window_.TextState();
    MessageFormatSelectionTarget output{state.name, state.voices, state.body};
    ClearMessageFormatAt4C5850(output);
    std::uint32_t variant;
    {
        Cp932Names names;
        // 4C27D9：名不同便短路；只在名相同后才比较姓。不是 defaultNameMode 开关。
        // 原比较仅借chars、不Clone；姓字段必须留在短路右侧，不能预取两组指针。
        variant = names.Different(save_.stringAt58, params_.defaultGivenName.Native().GetString()) ||
            names.Different(save_.stringAt5C, params_.defaultFamilyName.Native().GetString()) ? 1u : 0u;
    }
    const auto oldRead = messages_.ReadFormatAt417C40(index, output, variant, true);
    if (oldRead != 0 && oldRead != 1)
    {
        errors_.ShowMessageReadError();
        return false;
    }
    // 4C28B9..4C2914 的成员/共享引用发布顺序；本方法没有局部 catch 或回滚。
    state.unreadAt9A = static_cast<std::uint8_t>(oldRead == 0);
    state.shadowAt98 = save_.bytesAt60[0];
    state.renderAt99 = save_.bytesAt60[1];
    state.formatIndexAt94 = index;
    auto& voiceOutput = unsave_.ArrayReferenceAt3C();
    // 4C28ED..4C2908 通过410360复制三字段，不能把来源的bias强制归零。
    voiceOutput = state.voices;
    state.playbackStartedAt54 = std::uint8_t{0};
    state.glyphCursorAt50 = 0;
    window_.ClearPreviousFormatRegionAt4C291B();
    if (initialize) static_cast<void>(window_.PrepareTextAt415150(text_, glyphs_, layout_));
    return true;
}

bool TextWindowFormatRuntime::RebuildAt4C1650(const ScrVmValueTable& activeParams, bool refreshUnread)
{
    return RebuildForWindowAt4C1650(window_, activeParams, refreshUnread, layout_);
}

bool TextWindowFormatRuntime::RebuildForWindowAt4C1650(TextWindowLayerRuntime& window,
    const ScrVmValueTable& activeParams, bool refreshUnread, ITextWindowTextServices& layout)
{
    // 5EE484 -> 5EE470 -> 5EE450 -> 4C1715：原 catch(...) 不撤销先前副作用。
    try
    {
        const auto index = activeParams.CurrentParamsGeneration().messageFormatIndexAt60;
        if (index == -1) throw 1;
        auto& state = window.TextState();
        if (!state.unreadAt9A)
            throw std::logic_error("CTextWin rebuild reads uninitialized +9A");
        const auto previousUnread = *state.unreadAt9A;
        TextWindowFormatRuntime target(window, messages_, params_, save_, unsave_, text_, glyphs_, layout, errors_);
        if (!target.SetFormatAt4C2740(index, false)) throw 2;
        if (!refreshUnread) state.unreadAt9A = previousUnread;
        static_cast<void>(window.PrepareTextAt415150(text_, glyphs_, layout));
        const auto length = static_cast<std::uint32_t>(text_.GetTextLength());
        // 4C16E1 不检查 AL；失败返回与异常必须分开，不能自行改为整句 TextBltAll。
        static_cast<void>(window.BlitRangeAt419090(0, length, text_, glyphs_, layout));
        state.playbackStartedAt54 = std::uint8_t{1};
        state.waitFramesAt58 = 0;
        state.autoWaitFramesAt5C = 0;
        state.glyphCursorAt50 = -1;
        return true;
    }
    catch (...)
    {
        KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Error,
            "CTextWin 4C1650 rebuild failed; retained partial format/layout state");
        return false;
    }
}
}
