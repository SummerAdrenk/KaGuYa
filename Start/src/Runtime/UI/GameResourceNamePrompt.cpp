#include <Runtime/GameResourceNamePrompt.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Logging/Logger.h>

#include <algorithm>
#include <cstring>
#include <limits>
#include <string>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr int DialogId = 112;
constexpr int GivenId = 1401;
constexpr int FamilyId = 1400;
constexpr int SkipId = 1200;
constexpr int DefaultsId = 1100;
using Status = GameResourceNamePromptStatus;

struct NativeFailure final { DWORD code; };

[[noreturn]] void FailLastError(DWORD fallback = ERROR_GEN_FAILURE)
{
    const auto code = ::GetLastError();
    throw NativeFailure{code ? code : fallback};
}

// DialogBoxParamA和edit使用进程ACP；不把UTF-8日志编码或独立CRT locale当输入编码。
std::string LimitName(std::string_view text, UINT codePage, std::size_t maximum = 6)
{
    if (!maximum || maximum > static_cast<std::size_t>((std::numeric_limits<int>::max)()))
        throw NativeFailure{ERROR_INVALID_PARAMETER};
    if (text.empty()) return {};
    if (text.size() > static_cast<std::size_t>((std::numeric_limits<int>::max)()) ||
        text.find('\0') != std::string_view::npos)
        throw NativeFailure{ERROR_INVALID_DATA};
    if (!::MultiByteToWideChar(codePage, MB_ERR_INVALID_CHARS, text.data(),
        static_cast<int>(text.size()), nullptr, 0))
        FailLastError(ERROR_NO_UNICODE_TRANSLATION);
    auto size = (std::min)(text.size(), maximum);
    while (size && !::MultiByteToWideChar(codePage, MB_ERR_INVALID_CHARS,
        text.data(), static_cast<int>(size), nullptr, 0))
        --size;
    return std::string(text.substr(0, size));
}

struct NameModel final
{
    UINT codePage;
    bool editGiven;
    bool editFamily;
    GameNameByteLimits limits;
    GameResourceNamePromptContext context;
    std::string defaultGiven;
    std::string defaultFamily;
    std::string given;
    std::string family;
    bool skip = false;

    NameModel(const ParamsGameSystem& game, const SaveDataEnvironmentArchive& save, UINT page,
        GameNameByteLimits requestedLimits = {},
        GameResourceNamePromptContext requestedContext = GameResourceNamePromptContext::Title)
        : codePage(page), editGiven((game.defaultNameMode & 1u) != 0),
          editFamily((game.defaultNameMode & 2u) != 0), limits(requestedLimits), context(requestedContext),
          defaultGiven(LimitName(game.defaultGivenName.View(), page, limits.given)),
          defaultFamily(LimitName(game.defaultFamilyName.View(), page, limits.family)),
          given(editGiven ? LimitName(RuntimeStringView(save.stringAt58), page, limits.given) : defaultGiven),
          family(editFamily ? LimitName(RuntimeStringView(save.stringAt5C), page, limits.family) : defaultFamily) {}

    void Defaults() { given = defaultGiven; family = defaultFamily; }
    void Read(std::string_view givenText, std::string_view familyText, bool skipChecked)
    {
        given = editGiven ? LimitName(givenText, codePage, limits.given) : defaultGiven;
        family = editFamily ? LimitName(familyText, codePage, limits.family) : defaultFamily;
        skip = context == GameResourceNamePromptContext::Title && skipChecked;
    }
    int MissingField() const noexcept
    {
        if (editGiven && given.empty()) return GivenId;
        if (editFamily && family.empty()) return FamilyId;
        return 0;
    }
};

void CommitNames(const NameModel& model, SaveDataEnvironmentArchive& save)
{
    // 两个可能抛出的容量申请都完成后才提交文本/长度与跳过标志，避免半组姓名。
    auto* given = save.stringAt58.GetBuffer(static_cast<int>(model.given.size()));
    auto* family = save.stringAt5C.GetBuffer(static_cast<int>(model.family.size()));
    std::memcpy(given, model.given.data(), model.given.size());
    std::memcpy(family, model.family.data(), model.family.size());
    save.stringAt58.ReleaseBufferSetLength(static_cast<int>(model.given.size()));
    save.stringAt5C.ReleaseBufferSetLength(static_cast<int>(model.family.size()));
    // 02-10设置入口隐藏1200但接受仍清skip；标题入口保持原checkbox提交。
    save.valueAt54 = model.context == GameResourceNamePromptContext::LegacySettings ? 0 : (model.skip ? 1 : 0);
}

class MinimizeSuppression final
{
public:
    explicit MinimizeSuppression(RootFrameControlRuntime& frame)
        : frame_(frame), previous_(frame.ReadMinimizeByteAt1B2())
    { frame_.WriteMinimizeByteAt1B2(0); }
    ~MinimizeSuppression() noexcept { frame_.WriteMinimizeByteAt1B2(previous_); }
    MinimizeSuppression(const MinimizeSuppression&) = delete;
    MinimizeSuppression& operator=(const MinimizeSuppression&) = delete;
private:
    RootFrameControlRuntime& frame_;
    std::uint8_t previous_;
};

struct DialogState final
{
    NameModel model;
    GameResourceNamePromptResult result{};
    bool updating = false;
    bool initialized = false;
};

class UpdatingControls final
{
public:
    explicit UpdatingControls(DialogState& state) noexcept
        : flag_(state.updating), previous_(flag_) { flag_ = true; }
    ~UpdatingControls() noexcept { flag_ = previous_; }
    UpdatingControls(const UpdatingControls&) = delete;
    UpdatingControls& operator=(const UpdatingControls&) = delete;
private:
    bool& flag_;
    bool previous_;
};

HWND Control(HWND window, int id)
{
    const auto control = ::GetDlgItem(window, id);
    if (!control) throw NativeFailure{ERROR_INVALID_DATA};
    return control;
}

std::string ReadText(HWND control)
{
    ::SetLastError(ERROR_SUCCESS);
    const auto length = ::GetWindowTextLengthA(control);
    if (!length && ::GetLastError()) FailLastError();
    if (length == (std::numeric_limits<int>::max)()) throw NativeFailure{ERROR_NOT_ENOUGH_MEMORY};
    std::string text(static_cast<std::size_t>(length) + 1, '\0');
    ::SetLastError(ERROR_SUCCESS);
    const auto copied = ::GetWindowTextA(control, text.data(), length + 1);
    if (!copied && ::GetLastError()) FailLastError();
    // Unicode edit的ANSI长度查询可能偏大，按实际返回字节数收束。
    text.resize(static_cast<std::size_t>(copied));
    return text;
}

void WriteText(HWND window, int id, const std::string& text)
{
    if (!::SetWindowTextA(Control(window, id), text.c_str())) FailLastError();
}

void WriteNames(HWND window, DialogState& state)
{
    UpdatingControls updating(state);
    WriteText(window, GivenId, state.model.given);
    WriteText(window, FamilyId, state.model.family);
}

void End(HWND window, DialogState& state, Status status)
{
    if (!::EndDialog(window, status == Status::Accepted ? IDOK : IDCANCEL)) FailLastError();
    state.result.status = status;
}

void NormalizeEdit(HWND window, int id, DialogState& state)
{
    if (state.updating || !state.initialized) return;
    const auto control = Control(window, id);
    const auto text = ReadText(control);
    const auto maximum = id == GivenId ? state.model.limits.given : state.model.limits.family;
    const auto trimmed = LimitName(text, state.model.codePage, maximum);
    if (trimmed == text) return;
    DWORD start = 0, end = 0;
    ::SendMessageA(control, EM_GETSEL, reinterpret_cast<WPARAM>(&start), reinterpret_cast<LPARAM>(&end));
    UpdatingControls updating(state);
    WriteText(window, id, trimmed);
    ::SendMessageA(control, EM_SETSEL, start, end);
}

INT_PTR Dispatch(HWND window, UINT message, WPARAM w, DialogState& state)
{
    if (message == WM_INITDIALOG)
    {
        for (const auto id : {GivenId, FamilyId, SkipId, DefaultsId, IDOK, IDCANCEL})
            static_cast<void>(Control(window, id));
        ::EnableWindow(Control(window, GivenId), state.model.editGiven);
        ::EnableWindow(Control(window, FamilyId), state.model.editFamily);
        WriteNames(window, state);
        if (!::CheckDlgButton(window, SkipId, BST_UNCHECKED)) FailLastError();
        if (state.model.context == GameResourceNamePromptContext::LegacySettings)
            ::ShowWindow(Control(window, SkipId), SW_HIDE);
        state.initialized = true;
        const auto focus = state.model.editFamily ? FamilyId : (state.model.editGiven ? GivenId : IDOK);
        ::SetFocus(Control(window, focus));
        return FALSE; // Focus was selected explicitly.
    }
    if (message == WM_CLOSE)
    {
        End(window, state, Status::Cancelled);
        return TRUE;
    }
    if (message != WM_COMMAND || !state.initialized) return FALSE;
    const int id = LOWORD(w);
    if ((id == GivenId || id == FamilyId) && HIWORD(w) == EN_CHANGE)
        NormalizeEdit(window, id, state);
    else if (id == IDCANCEL && HIWORD(w) == BN_CLICKED)
        End(window, state, Status::Cancelled);
    else if (id == DefaultsId && HIWORD(w) == BN_CLICKED)
    {
        state.model.Defaults();
        WriteNames(window, state);
    }
    else if (id == IDOK && HIWORD(w) == BN_CLICKED)
    {
        state.model.Read(ReadText(Control(window, GivenId)), ReadText(Control(window, FamilyId)),
            ::IsDlgButtonChecked(window, SkipId) == BST_CHECKED);
        if (const auto missing = state.model.MissingField())
        {
            const auto* text = missing == GivenId
                ? L"\u540d\u524d\u304c\u8a2d\u5b9a\u3055\u308c\u3066\u3044\u307e\u305b\u3093\uff01"
                : L"\u540d\u5b57\u304c\u8a2d\u5b9a\u3055\u308c\u3066\u3044\u307e\u305b\u3093\uff01";
            if (!::MessageBoxW(window, text, L"\u540d\u524d\u306e\u5909\u66f4", MB_OK | MB_ICONWARNING))
                FailLastError();
            ::SetFocus(Control(window, missing));
        }
        else End(window, state, Status::Accepted);
    }
    else return FALSE;
    return TRUE;
}

INT_PTR CALLBACK NameDialogProcedure(HWND window, UINT message, WPARAM w, LPARAM l) noexcept
{
    auto* state = message == WM_INITDIALOG ? reinterpret_cast<DialogState*>(l)
        : reinterpret_cast<DialogState*>(::GetWindowLongPtrA(window, DWLP_USER));
    if (!state) return FALSE;
    try
    {
        if (message == WM_INITDIALOG)
        {
            ::SetLastError(ERROR_SUCCESS);
            const auto previous = ::SetWindowLongPtrA(window, DWLP_USER, reinterpret_cast<LONG_PTR>(state));
            if (!previous && ::GetLastError()) FailLastError();
        }
        if (message == WM_NCDESTROY)
        {
            ::SetWindowLongPtrA(window, DWLP_USER, 0);
            return FALSE;
        }
        if (state->result.exception) return TRUE;
        return Dispatch(window, message, w, *state);
    }
    catch (const NativeFailure& error)
    { state->result = {Status::Error, error.code, std::current_exception()}; }
    catch (...)
    { state->result = {Status::Error, ERROR_UNHANDLED_EXCEPTION, std::current_exception()}; }
    // EndDialog延迟到回调返回后销毁；C++异常不能跨入User32。
    ::EndDialog(window, -1);
    return TRUE;
}

GameResourceNamePromptResult CompleteDialog(const DialogState& state, INT_PTR nativeResult,
    DWORD nativeError, SaveDataEnvironmentArchive& save)
{
    if (state.result.exception) return state.result;
    if (nativeResult == -1 || nativeResult == 0)
        return {Status::Error, nativeError ? nativeError : ERROR_GEN_FAILURE, {}};
    if (nativeResult == IDCANCEL && state.result.status == Status::Cancelled) return state.result;
    if (nativeResult != IDOK || state.result.status != Status::Accepted || !state.initialized)
        return {Status::Error, ERROR_INVALID_DATA, {}};
    CommitNames(state.model, save);
    return state.result;
}
}

GameResourceNamePromptResult ShowGameResourceNamePrompt(
    const GameResourceNamePromptDependencies& dependencies) noexcept
{
    GameResourceNamePromptResult result;
    try
    {
        auto* root = dependencies.root.Published();
        auto* environment = dependencies.environment.Published();
        const auto* params = dependencies.params.Published();
        if (!root || !environment || !params || !params->GameSystem() ||
            !root->Frame().MinimizeByteAt1B2Published())
            throw NativeFailure{ERROR_INVALID_STATE};
        auto* save = static_cast<SaveDataEnvironmentArchive*>(ResolveEnvironmentArchiveAt417990(*environment, 1));
        if (!save || !dependencies.resourceInstance) throw NativeFailure{ERROR_INVALID_PARAMETER};
        const auto parent = reinterpret_cast<HWND>(environment->windowAt2C);
        if (!::IsWindow(parent)) throw NativeFailure{ERROR_INVALID_WINDOW_HANDLE};
        if (::GetWindowThreadProcessId(parent, nullptr) != ::GetCurrentThreadId())
            throw NativeFailure{ERROR_INVALID_THREAD_ID};
        if (!::FindResourceA(dependencies.resourceInstance, MAKEINTRESOURCEA(DialogId), RT_DIALOG))
            FailLastError(ERROR_RESOURCE_NAME_NOT_FOUND);

        DialogState state{NameModel(*params->GameSystem(), *save, ::GetACP(), dependencies.limits, dependencies.context)};
        MinimizeSuppression suppress(root->Frame());
        ::SetLastError(ERROR_SUCCESS);
        const auto modal = ::DialogBoxParamA(dependencies.resourceInstance, MAKEINTRESOURCEA(DialogId),
            parent, &NameDialogProcedure, reinterpret_cast<LPARAM>(&state));
        const auto nativeError = ::GetLastError();
        result = CompleteDialog(state, modal, nativeError, *save);
    }
    catch (const NativeFailure& error)
    { result = {Status::Error, error.code, std::current_exception()}; }
    catch (...)
    { result = {Status::Error, ERROR_UNHANDLED_EXCEPTION, std::current_exception()}; }
    if (result.status == Status::Error)
        Logging::WriteFormat(Logging::Level::Error,
            "Start_RS：原生姓名对话框失败，资源=112，错误=%lu，异常=%d。",
            result.win32Error, result.exception ? 1 : 0);
    return result;
}
}
