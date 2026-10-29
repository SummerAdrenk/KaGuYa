#include <Runtime/NativeFileProgressDialogRuntime.h>

#include <Platform/PendingWindowCreation.h>
#include <Platform/WindowHost.h>
#include <Platform/WindowCreation.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
struct NativeFileProgressDialogRuntime::State
{
    HWND window = nullptr;
    Platform::ExecutableWindowThunk thunk;
    Platform::PendingWindowCreateRecord record{};
    struct MessageContext { MessageContext* previous; };
    MessageContext* current = nullptr;
    bool destroyPending = false;
    [[maybe_unused]] std::uint8_t reservedAt20 = 0;
    bool cancelRequestAt21 = false;

    ~State() noexcept
    {
        // 468FF0：先销毁窗口并处理同步消息，再由成员析构释放thunk。
        if (window != nullptr) DestroyWindow(window);
        if (record.object != nullptr)
            Platform::PendingWindowCreateRecords().Remove(record);
    }

    void Create(HINSTANCE resourceInstance)
    {
        if (!thunk.TryInitialize(nullptr, 0))
        {
            SetLastError(ERROR_OUTOFMEMORY);
            return;
        }
        record.object = this;
        record.threadId = GetCurrentThreadId();
        Platform::PendingWindowCreateRecords().Push(record);
        // 458AAE：不赋返回HWND、不抛创建失败、不改可见性，失败记录留到对象析构。
        static_cast<void>(CreateDialogParamA(resourceInstance, MAKEINTRESOURCEA(101),
            nullptr, &Initial, 0));
    }

    static INT_PTR CALLBACK Initial(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        auto* state = static_cast<State*>(
            Platform::PendingWindowCreateRecords().PopForCurrentThread());
        if (state == nullptr) return 0;
        state->window = hwnd;
        static_cast<void>(state->thunk.TryInitialize(state,
            reinterpret_cast<std::uintptr_t>(&Bound)));
        const auto procedure = reinterpret_cast<DLGPROC>(state->thunk.Procedure());
        SetWindowLongA(hwnd, DWL_DLGPROC,
            static_cast<LONG>(reinterpret_cast<LONG_PTR>(procedure)));
        return procedure(hwnd, message, wParam, lParam);
    }

    static INT_PTR CALLBACK Bound(State* state, UINT message, WPARAM wParam, LPARAM lParam)
    {
        return state->Dispatch(message, wParam, lParam);
    }

    bool ProcessMessage(UINT message, WPARAM wParam, LRESULT& result)
    {
        if (message == WM_INITDIALOG)
        {
            static_cast<void>(Platform::CenterWindowAt413990(window, GetParent(window)));
            result = 1;
            return true;
        }
        if (message == WM_COMMAND && LOWORD(wParam) == IDCANCEL)
        {
            cancelRequestAt21 = true;
            result = 0;
            return true;
        }
        return false;
    }

    INT_PTR Dispatch(UINT message, WPARAM wParam, LPARAM /*lParam*/)
    {
        MessageContext context{current};
        current = &context;
        LRESULT result = 0;
        const bool handled = ProcessMessage(message, wParam, result);
        current = context.previous;
        INT_PTR returned = handled ? 1 : 0;
        if (handled)
        {
            // 415885..4158B6：这些特殊对话框消息直接返回LRESULT。
            if (message == WM_VKEYTOITEM || message == WM_CHARTOITEM ||
                message == WM_QUERYDRAGICON || message == WM_COMPAREITEM ||
                message == WM_INITDIALOG ||
                (message >= WM_CTLCOLORMSGBOX && message <= WM_CTLCOLORSTATIC))
                returned = result;
            else if (!destroyPending)
                SetWindowLongA(window, DWL_MSGRESULT, static_cast<LONG>(result));
        }
        else if (message == WM_NCDESTROY)
        {
            destroyPending = true;
        }
        if (destroyPending && current == nullptr)
        {
            destroyPending = false;
            window = nullptr;
            // 424DC0的最终消息回调为空；窗口失效不删除owner。
        }
        // 未处理消息交给dialog manager；不可自行调用DefWindowProcA。
        return returned;
    }
};

NativeFileProgressDialogRuntime::NativeFileProgressDialogRuntime(HINSTANCE resourceInstance)
    : state_(std::make_unique<State>())
{
    state_->Create(resourceInstance);
}

NativeFileProgressDialogRuntime::~NativeFileProgressDialogRuntime() noexcept = default;

HWND NativeFileProgressDialogRuntime::Window() const noexcept
{
    return state_->window;
}

void NativeFileProgressDialogRuntime::PumpAt485C70()
{
    MSG message{};
    while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE))
    {
        if (!IsDialogMessageA(state_->window, &message))
        {
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }
    }
}

bool NativeFileProgressDialogRuntime::ConsumeCancelRequest() noexcept
{
    const bool requested = state_->cancelRequestAt21;
    state_->cancelRequestAt21 = false;
    return requested;
}
}
