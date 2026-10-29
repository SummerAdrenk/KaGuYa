#include <Platform/WindowHost.h>

#include <array>
#include <cstdint>
#include <cstring>

namespace
{
void ReportCreateFailure()
{
    // 4342A0 使用 MessageBoxA，因此这里保存原 CP932 字节，不做 Unicode 转换。
    constexpr char title[] = "\x83\x47\x83\x89\x81\x5B";
    constexpr char message[] =
        "\x83\x81\x83\x43\x83\x93\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45"
        "\x82\xCC\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC"
        "\x82\xB5\x82\xBD\x81\x49";
    MessageBoxA(nullptr, message, title, MB_ICONERROR);
}
}

namespace KaGuYa::Start::Platform
{
ExecutableWindowThunk::~ExecutableWindowThunk()
{
    if (code_ != nullptr)
    {
        VirtualFree(code_, 0, MEM_RELEASE);
    }
}

void ExecutableWindowThunk::Initialize(
    void* const object,
    const std::uintptr_t targetProcedure)
{
    if (!TryInitialize(object, targetProcedure))
        RaiseException(0xC0000017u, EXCEPTION_NONCONTINUABLE, 0, nullptr);
}

bool ExecutableWindowThunk::TryInitialize(
    void* const object,
    const std::uintptr_t targetProcedure)
{
    static_assert(sizeof(void*) == 4, "窗口 thunk 只实现原版 x86 指令布局");
    if (code_ == nullptr)
    {
        code_ = VirtualAlloc(nullptr, 13, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (code_ == nullptr)
        {
            return false;
        }
    }

    auto* const bytes = static_cast<std::uint8_t*>(code_);
    // C7 44 24 04 imm32 : 把 WndProc 栈上的 HWND 参数替换成当前 CMainFrame 对象。
    constexpr std::array<std::uint8_t, 4> moveObject{0xC7, 0x44, 0x24, 0x04};
    std::memcpy(bytes, moveObject.data(), moveObject.size());
    const std::uint32_t objectAddress = reinterpret_cast<std::uint32_t>(object);
    std::memcpy(bytes + 4, &objectAddress, sizeof(objectAddress));
    bytes[8] = 0xE9;
    // E9位移取低32位；跨越有符号地址边界时不能执行C++有符号减法溢出。
    const auto target = static_cast<std::uint32_t>(targetProcedure);
    const auto nextInstruction = reinterpret_cast<std::uint32_t>(bytes + 13);
    const std::uint32_t displacement = target - nextInstruction;
    std::memcpy(bytes + 9, &displacement, sizeof(displacement));
    FlushInstructionCache(GetCurrentProcess(), code_, 13);
    return true;
}

WNDPROC ExecutableWindowThunk::Procedure() const noexcept
{
    return reinterpret_cast<WNDPROC>(code_);
}

WindowHost::WindowHost(MainWindow& mainWindow)
    : mainWindow_(mainWindow)
{
}

WindowHost::~WindowHost()
{
    if (createRecord_.object != nullptr)
        PendingWindowCreateRecords().Remove(createRecord_);
}

HWND WindowHost::Create(
    WindowClassRegistrar& classRegistrar,
    const HINSTANCE moduleInstance,
    const HINSTANCE resourceInstance,
    const WindowCreationPlan& plan,
    const HMENU childWindowIdentifier)
{
    WNDPROC superclassProcedure = nullptr;
    const ATOM classAtom = classRegistrar.Register(
        moduleInstance,
        resourceInstance,
        &superclassProcedure,
        WindowClassRegistrationKind::MainFrame);
    // 434565..434582：注册返回后先准备 thunk，再检查 AX；失败时不进入窗口回调。
    // 原 53F0E4 分配失败设置 ERROR_OUTOFMEMORY，随后显示同一窗口错误并抛整数 2。
    if (!thunk_.TryInitialize(nullptr, 0))
    {
        SetLastError(ERROR_OUTOFMEMORY);
        ReportCreateFailure();
        throw 2;
    }
    if (classAtom == 0)
    {
        ReportCreateFailure();
        throw 2;
    }
    previousProcedure_ = superclassProcedure != nullptr ? superclassProcedure : DefWindowProcA;

    createRecord_.object = this;
    createRecord_.threadId = GetCurrentThreadId();
    PendingWindowCreateRecords().Push(createRecord_);

    const HMENU menu = (plan.style & WS_CHILD) != 0 ? childWindowIdentifier : nullptr;
    const HWND createdWindow = CreateWindowExA(
        plan.exStyle,
        MAKEINTATOM(classAtom),
        plan.title.c_str(),
        plan.style,
        plan.x,
        plan.y,
        plan.outerWidth,
        plan.outerHeight,
        nullptr,
        menu,
        moduleInstance,
        nullptr);
    if (createdWindow == nullptr)
    {
        ReportCreateFailure();
        throw 2;
    }

    // 原版只把 CreateWindowExA 的返回值用于判空，正式 HWND 由首次类过程写回对象。
    return window_;
}

LRESULT CALLBACK WindowHost::InitialWindowProcedure(
    const HWND window,
    const UINT message,
    const WPARAM wParam,
    const LPARAM lParam)
{
    auto* const host = static_cast<WindowHost*>(PendingWindowCreateRecords().PopForCurrentThread());
    if (host == nullptr)
    {
        return 0;
    }

    host->window_ = window;
    host->mainWindow_.Attach(window, host->previousProcedure_);
    host->thunk_.Initialize(host, reinterpret_cast<std::uintptr_t>(&BoundWindowProcedure));
    const WNDPROC thunkProcedure = host->thunk_.Procedure();
    SetWindowLongA(window, GWL_WNDPROC, static_cast<LONG>(reinterpret_cast<LONG_PTR>(thunkProcedure)));
    return thunkProcedure(window, message, wParam, lParam);
}

LRESULT CALLBACK WindowHost::BoundWindowProcedure(
    WindowHost* const host,
    const UINT message,
    const WPARAM wParam,
    const LPARAM lParam)
{
    return host->DispatchBound(message, wParam, lParam);
}

LRESULT WindowHost::DispatchBound(
    const UINT message,
    const WPARAM wParam,
    const LPARAM lParam)
{
    MessageContext context{currentMessage_};
    currentMessage_ = &context;

    LRESULT result = 0;
    const bool handled = mainWindow_.Dispatch(0, message, wParam, lParam, result);
    if (!handled && message != WM_NCDESTROY)
    {
        result = CallWindowProcA(previousProcedure_, window_, message, wParam, lParam);
    }

    if (message == WM_NCDESTROY && !handled)
    {
        // 43A11A/43A149 比较转发前后的过程，不是与本对象 thunk 常量比较。
        const LONG procedureBefore = GetWindowLongA(window_, GWL_WNDPROC);
        result = CallWindowProcA(previousProcedure_, window_, message, wParam, lParam);
        if (previousProcedure_ != DefWindowProcA &&
            GetWindowLongA(window_, GWL_WNDPROC) == procedureBefore)
        {
            SetWindowLongA(
                window_,
                GWL_WNDPROC,
                static_cast<LONG>(reinterpret_cast<LONG_PTR>(previousProcedure_)));
        }
        destroyPending_ = true;
    }

    currentMessage_ = context.previous;
    if (destroyPending_ && currentMessage_ == nullptr)
    {
        // 43A170/43A173 先清销毁位；否则同一 host 再建窗口会在首条消息结束时清掉新 HWND。
        destroyPending_ = false;
        window_ = nullptr;
        mainWindow_.Attach(nullptr, previousProcedure_);
    }
    return result;
}
}
