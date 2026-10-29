#include <Runtime/ScriptEndCommandNative.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/ScrVmFileCallHandlers.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
union TextSlot
{
    RuntimeCString value;
    TextSlot() noexcept {}
    ~TextSlot() noexcept {}
};
struct TextUnwind
{
    RuntimeCString* text;
    ~TextUnwind() noexcept { if (text) DestroyRuntimeCStringAt40CD40(text); }
};
void Show(IScrVmCommandErrorSink& errors, const char* message)
{
    const auto& environment = errors.CaptureAt4180C0();
    const char* const caption = environment.titleAt10.GetString();
    const HWND window = reinterpret_cast<HWND>(environment.windowAt2C);
    static_cast<void>(errors.ShowMessageBoxA(window, message, caption, MB_ICONERROR));
}
std::int32_t ReadNamedRootFirst(IScriptFileCommandSource& source,
    IScrVmCommandErrorSink& errors, const char* name)
{
    const auto root = source.CaptureAfterEnsureAt413BE0();
    const auto* table = source.CaptureSlot0At4180C0();
    const auto index = source.ResolveAt498020(table, name);
    return ReadScriptValueAt4C73A0(root.paramsAt47BDB0(root.object), index, errors);
}
void ReportEnd(IScriptFileCommandSource& source, IScrVmCommandErrorSink& errors,
    IRuntimeStringManagerSource& strings, bool missingControl)
{
    TextSlot text;
    InitializeEmptyRuntimeCString(text.value, strings);
    TextUnwind unwind{&text.value};
    const auto root = source.CaptureAfterEnsureAt413BE0();
    const char* name;
    if (missingControl)
    {
        // 4CDABC先捕获Pattern，再对已捕获root读取4AD260；不能沿用LoadScript输入ID。
        const auto names = source.CapturePatternNames();
        const auto id = root.paramsAt47BDB0(root.object).SaveParams2At30().scriptFileIdAtSaveParams04;
        name = names.FindNameAt4AD1B0(id);
    }
    else
    {
        // 4CDB34..4CDB86顺序相反：读当前环境ID，再捕获Pattern、查名。
        const auto id = root.paramsAt47BDB0(root.object).SaveParams2At30().scriptFileIdAtSaveParams04;
        const auto names = source.CapturePatternNames();
        name = names.FindNameAt4AD1B0(id);
    }
    // 原PE的CP932格式串；%s的null行为交给原CString格式化叶，不自行补空串。
    constexpr char endFormat[] = "CmdEnd\x8F\x88\x97\x9D\x82\xAA\x8E\xB8\x94\x73"
        "\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49 : %s \x83\x74\x83\x40\x83\x43\x83\x8B";
    constexpr char missingFormat[] = "\x90\xA7\x8C\xE4\x83\x58\x83\x4E\x83\x8A\x83\x76"
        "\x83\x67\x82\xAA\x8C\xA9\x82\xC2\x82\xA9\x82\xE8\x82\xDC\x82\xB9\x82\xF1"
        "\x82\xC5\x82\xB5\x82\xBD\x81\x49 : %s \x83\x74\x83\x40\x83\x43\x83\x8B";
    static_assert(sizeof(missingFormat) == 53);
    FormatRuntimeCStringAt417130(text.value, missingControl ? missingFormat : endFormat, name);
    // 4CDB9D先取Environment再捕获文字；MessageBox后仍释放缓存header。
    const auto& environment = errors.CaptureAt4180C0();
    const char* const cachedText = text.value.GetString();
    const char* const caption = environment.titleAt10.GetString();
    const HWND window = reinterpret_cast<HWND>(environment.windowAt2C);
    static_cast<void>(errors.ShowMessageBoxA(window, cachedText, caption, MB_ICONERROR));
    // 4CDBBA先撤state；正常Free抛出直接外传，不再次减持。
    unwind.text = nullptr;
    ReleaseRuntimeCStringHeader(reinterpret_cast<ATL::CStringData*>(const_cast<char*>(cachedText)) - 1);
}
}

std::int32_t ExecuteScriptEndAt4CD8D0(ScrScriptRuntimeState& runtime,
    IScriptFileCommandSource& source, IScrVmScriptSwitchBoundary& scriptSwitch,
    IScrVmCommandErrorSink& errors, IRuntimeStringManagerSource& strings)
{
    // 4CD8F8..4CD965首次slot0在root之前，后续具名读取均相反。
    const auto* table = source.CaptureSlot0At4180C0();
    const auto root = source.CaptureAfterEnsureAt413BE0();
    const auto index = source.ResolveAt498020(table, "game_end");
    if (ReadScriptValueAt4C73A0(root.paramsAt47BDB0(root.object), index, errors) == 0)
    {
        ReportEnd(source, errors, strings, false);
        return 2;
    }
    if (ReadNamedRootFirst(source, errors, "game_end") != 3) return 2;
    const auto target = ReadNamedRootFirst(source, errors, "follow_return");
    const auto result = scriptSwitch.Switch(runtime, target);
    if (!result.success)
    {
        // 现有LoadScript适配器返回错误正文；先显示Load错误，再进入END自己的CString报告。
        if(!result.failureAlreadyReported)Show(errors, result.message.c_str());
        ReportEnd(source, errors, strings, true);
        return 2;
    }
    const auto point = ReadNamedRootFirst(source, errors, "follow_point");
    runtime.SetStagedProgramCounter(std::bit_cast<std::uint32_t>(point));
    const auto writeRoot = source.CaptureAfterEnsureAt413BE0();
    const auto* writeTable = source.CaptureSlot0At4180C0();
    const auto writeIndex = source.ResolveAt498020(writeTable, "game_end");
    WriteScriptValueAt4C8220(writeRoot.paramsAt47BDB0(writeRoot.object), writeIndex, 0, errors);
    return 0;
}
}
