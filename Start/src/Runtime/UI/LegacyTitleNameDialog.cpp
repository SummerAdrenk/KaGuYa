#include <Runtime/LegacyTitleNameDialog.h>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T> T Read(const T& value) noexcept { return *static_cast<const volatile T*>(&value); }

// 4088C8/408989：临时 CString 的正常释放可传播，且不因成员析构重试减持。
union TemporaryString
{
    TemporaryString() noexcept {}
    ~TemporaryString() noexcept {}
    RuntimeCString value;
};
struct StringCleanup final
{
    RuntimeCString& value;
    ~StringCleanup() noexcept(false) { DestroyRuntimeCStringAt40CD40(&value); }
};
}

const DialogEndpointNativeVtable4 LegacyTitleNameDialog::table_{
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::processAt00)>(&Process),
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::deleteAt04)>(&Delete),
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::procedureAt08)>(&Procedure),
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::finalMessageAt0C)>(&Final)};

LegacyTitleNameDialog::LegacyTitleNameDialog(ILegacyTitleNameServices& source,
    ILegacyTitleNameApi& api, IRuntimeStringManagerSource& strings)
    : source_(source), api_(api), strings_(strings), endpoint_(api.EndpointServices())
{
    static_assert(std::is_standard_layout_v<Binding> && offsetof(Binding, endpoint) == 0);
    auto& body = binding_.endpoint;
    binding_.owner = this;
    body.vptrAt00 = &table_;
    body.windowAt04 = nullptr;
    body.thunkAt14 = nullptr;
    body.currentMessageAt18 = nullptr;
    body.destroyPendingAt1C = 0;
    // 49E920 无自身 EH：第二个初始化失败不能自动取得第一个字符串的展开资格。
    InitializeEmptyRuntimeCString(given_.value, strings_);
    InitializeEmptyRuntimeCString(family_.value, strings_);
    alive_ = true;
}
LegacyTitleNameDialog::~LegacyTitleNameDialog() noexcept(false) { if (alive_) Release(); }
void LegacyTitleNameDialog::Release()
{
    alive_ = false;
    // 49EFC0 -> 41ED00(family), 41ED00(given), 408D90 -> 41FFE0 -> 420000
    // -> 4149C0 -> 4149E0：前一叶抛出即停止；不强行 finally 清完后续成员。
    DestroyRuntimeCStringAt40CD40(&family_.value);
    DestroyRuntimeCStringAt40CD40(&given_.value);
    ReleaseDialogEndpointThunkNativeAt40E730(binding_.endpoint, endpoint_);
}
BOOL __fastcall LegacyTitleNameDialog::Process(DialogEndpointNativeBody20h* endpoint, void*,
    HWND window, UINT message, WPARAM w, LPARAM l, LRESULT* result, DWORD map)
{
    return reinterpret_cast<Binding*>(endpoint)->owner->ProcessMessage(window, message, w, l, *result, map);
}
DialogEndpointNativeBody20h* __fastcall LegacyTitleNameDialog::Delete(
    DialogEndpointNativeBody20h* endpoint, void*, DWORD flags)
{
    auto* owner = reinterpret_cast<Binding*>(endpoint)->owner;
    owner->Release();
    if (flags & 1u) delete owner;
    return endpoint;
}
DLGPROC __fastcall LegacyTitleNameDialog::Procedure(DialogEndpointNativeBody20h*, void*)
{ return &BoundDialogEndpointNativeAt415810; }
void __fastcall LegacyTitleNameDialog::Final(DialogEndpointNativeBody20h*, void*, HWND) {}
HWND LegacyTitleNameDialog::Window() const noexcept { return Read(binding_.endpoint.windowAt04); }

INT_PTR LegacyTitleNameDialog::RunModal()
{
    DialogEndpointNativeScope scope(endpoint_);
    const auto parent = api_.ActiveWindow();
    auto& body = binding_.endpoint;
    auto* thunk = Read(body.thunkAt14);
    if (!thunk)
    {
        thunk = AllocateWindowThunkNativeAt53F0E4(endpoint_.thunks);
        body.thunkAt14 = thunk;
    }
    // 4BB96B -> 413750 -> 413770：分配失败 SetLastError(14)，不进入 DialogBox。
    if (!thunk) { api_.LastError(ERROR_OUTOFMEMORY); return -1; }
    InitializeWindowThunkNativeAt53F1E6(thunk, 0, nullptr, endpoint_.thunks);
    PushDialogCreationNativeAt42F8B9(body, endpoint_);
    const auto instance = source_.ResourceInstance();
    return api_.DialogBoxAt4BB950(instance, MAKEINTRESOURCEA(112), parent, &InitialDialogEndpointNativeAt4382D0, 0);
}

BOOL LegacyTitleNameDialog::ExchangeText(int id, RuntimeCString& text, bool save)
{
    BOOL okay;
    if (save)
    {
        // 4BB9D0：先记长度，再 GetBufferSetLength；按首 NUL ReleaseBuffer(-1)，
        // 但是否短读取 GetWindowText 返回值，不拿 CString 最终长度替代。
        const auto control = api_.Control(Window(), id);
        const auto length = api_.TextLength(control);
        auto* buffer = GetRuntimeCStringBufferSetLengthAt417260(text, length);
        int copied = -1;
        if (buffer)
        {
            copied = api_.GetText(control, buffer, length + 1);
            ReleaseRuntimeCStringBufferAt486B10(text);
        }
        okay = copied >= length;
    }
    else okay = api_.SetControlText(Window(), id, text.GetString());
    if (!okay)
    {
        // 4BBAAC -> 4BC570：读/写失败都 Beep(-1) -> 重新取控件 -> SetFocus。
        static_cast<void>(api_.Beep(static_cast<UINT>(-1)));
        const auto control = api_.Control(Window(), id);
        static_cast<void>(api_.Focus(control));
    }
    return okay;
}
BOOL LegacyTitleNameDialog::UpdateData(bool save, int id)
{
    // 49ED60：given -> family -> checkbox；任一文本失败便短路后续绑定。
    if ((id == -1 || id == 1401) && !ExchangeText(1401, given_.value, save)) return FALSE;
    if ((id == -1 || id == 1400) && !ExchangeText(1400, family_.value, save)) return FALSE;
    if (id == -1 || id == 1200)
    {
        // 4BBB20 -> 4BC4D0：值2不是 true；不使用 IsDlgButtonChecked 的非零判定。
        const auto check = skip_ ? BST_CHECKED : BST_UNCHECKED;
        const auto control = api_.Control(Window(), 1200);
        if (save) skip_ = api_.Send(control, BM_GETCHECK, 0, 0) == BST_CHECKED;
        else static_cast<void>(api_.Send(control, BM_SETCHECK, check, 0));
    }
    return TRUE;
}
BOOL LegacyTitleNameDialog::Initialize()
{
    // 49EBD0：逐次取保存值/位/默认值；禁用发生在该默认字符串重新取源之前。
    AssignRuntimeCStringAt410220(given_.value, source_.SavedGivenName());
    if (!source_.EditableNameBit(0))
    {
        static_cast<void>(api_.Enable(api_.Control(Window(), 1401), FALSE));
        AssignRuntimeCStringAt410220(given_.value, source_.DefaultGivenName());
    }
    AssignRuntimeCStringAt410220(family_.value, source_.SavedFamilyName());
    skip_ = false;
    if (!source_.EditableNameBit(1))
    {
        static_cast<void>(api_.Enable(api_.Control(Window(), 1400), FALSE));
        AssignRuntimeCStringAt410220(family_.value, source_.DefaultFamilyName());
    }
    static_cast<void>(UpdateData(false));
    static_cast<void>(CenterDialogWindowNativeAt413990(&binding_.endpoint.windowAt04, nullptr, endpoint_.api));
    // 49ED05 再取 family bit；49ED49 的直接目标4128C0是 SetFocus，不是同名析构。
    if (!source_.EditableNameBit(1)) static_cast<void>(api_.Focus(api_.Control(Window(), 1401)));
    return TRUE;
}
void LegacyTitleNameDialog::RequiredName(bool given)
{
    // 原52FC74/52FC58，CP932字节；不依赖编译器执行字符集转换。
    static constexpr char givenText[] = "\x96\xBC\x91\x4F\x82\xAA\x90\xDD\x92\xE8\x82\xB3\x82\xEA\x82\xC4\x82\xA2\x82\xDC\x82\xB9\x82\xF1\x81\x49";
    static constexpr char familyText[] = "\x96\xBC\x8E\x9A\x82\xAA\x90\xDD\x92\xE8\x82\xB3\x82\xEA\x82\xC4\x82\xA2\x82\xDC\x82\xB9\x82\xF1\x81\x49";
    auto& root = source_.CaptureRoot();
    const auto* caption = root.Caption().GetString();
    const auto window = root.MainWindow();
    static_cast<void>(api_.ShowMessage(window, given ? givenText : familyText, caption, MB_ICONHAND));
}
void LegacyTitleNameDialog::Accept()
{
    // 49EE10 忽略 DDX 的 FALSE，但不吞异常；只验证当次开启的字段。
    static_cast<void>(UpdateData(true));
    if (source_.EditableNameBit(0) && given_.value.IsEmpty()) { RequiredName(true); return; }
    if (source_.EditableNameBit(1) && family_.value.IsEmpty()) { RequiredName(false); return; }
    // 49EEB2..49EF02 非事务提交；每一目的字段单独解析，后续抛出不回滚前缀。
    source_.WriteSkipNamePrompt(skip_);
    AssignRuntimeCStringAt410220(source_.SavedGivenName(), given_.value);
    AssignRuntimeCStringAt410220(source_.SavedFamilyName(), family_.value);
    static_cast<void>(api_.End(Window(), IDOK));
}
void LegacyTitleNameDialog::Defaults()
{
    static_cast<void>(UpdateData(true));
    // 49EF20：默认按钮与初始化/提交次序不同，先 family 后 given。
    if (source_.EditableNameBit(1)) AssignRuntimeCStringAt410220(family_.value, source_.DefaultFamilyName());
    if (source_.EditableNameBit(0)) AssignRuntimeCStringAt410220(given_.value, source_.DefaultGivenName());
    static_cast<void>(UpdateData(false));
}
void LegacyTitleNameDialog::EditChanged(int id)
{
    TemporaryString text;
    InitializeEmptyRuntimeCString(text.value, strings_);
    StringCleanup cleanup{text.value};
    const auto control = api_.Control(Window(), id);
    const auto length = api_.TextLength(control);
    auto* buffer = GetRuntimeCStringBufferSetLengthAt417260(text.value, length);
    static_cast<void>(api_.GetText(control, buffer, length + 1));
    ReleaseRuntimeCStringBufferAt486B10(text.value);
    if (text.value.GetLength() <= 6) return;
    int count = 0;
    while (count < text.value.GetLength())
    {
        int next = count;
        // 408953 sign-extend 后调用 CRT；按完整 MB 字符前进，最多6字节。
        if (api_.IsMbLead(static_cast<unsigned int>(static_cast<int>(static_cast<signed char>(text.value.GetString()[count]))))) ++next;
        if (++next > 6) break;
        count = next;
    }
    {
        TemporaryString left;
        ConstructRuntimeCStringLeftAt419D00(&left.value, text.value, count, strings_);
        StringCleanup leftCleanup{left.value};
        AssignRuntimeCStringAt410220(text.value, left.value);
    }
    // 4089CE：只写控件，不同步成员、不补 EM_SETSEL；同步 EN_CHANGE 可正常重入。
    static_cast<void>(api_.SetControlText(Window(), id, text.value.GetString()));
}
BOOL LegacyTitleNameDialog::ProcessMessage(HWND, UINT message, WPARAM w, LPARAM, LRESULT& result, DWORD map)
{
    // 49E960：按钮仅匹配 LOWORD；edit 额外要求 HIWORD==EN_CHANGE；其它 map 不消费。
    if (map != 0) return FALSE;
    if (message == WM_INITDIALOG) { result = Initialize(); return TRUE; }
    if (message != WM_COMMAND) return FALSE;
    const auto id = LOWORD(w);
    if (id == IDOK) Accept();
    else if (id == IDCANCEL) static_cast<void>(api_.End(Window(), IDCANCEL));
    else if (id == 1100) Defaults();
    else if ((id == 1401 || id == 1400) && HIWORD(w) == EN_CHANGE) EditChanged(id);
    else return FALSE;
    result = 0;
    return TRUE;
}
}
