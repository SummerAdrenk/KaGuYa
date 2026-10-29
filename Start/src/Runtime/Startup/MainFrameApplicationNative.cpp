#include <Runtime/MainFrameApplicationNative.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Logging/Logger.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T> T Read(const T& value) noexcept { return *static_cast<const volatile T*>(&value); }
LONG Signed(DWORD value) noexcept { return std::bit_cast<LONG>(value); }
const char separator[] = "\x81\x40"; // 原PE5AEA60，显式追加2字节。
const char caption[] = "\x83\x47\x83\x89\x81\x5B";
const char creationError[] =
    "\x83\x81\x83\x43\x83\x93\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45"
    "\x82\xCC\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC"
    "\x82\xB5\x82\xBD\x81\x49";
union Title final { RuntimeCString value; Title() noexcept {} ~Title() noexcept {} };
// 原L=[ebp-108h]、F=[ebp-D0h]：两正文相邻，未写区仍由各raw构造保留。
struct StackBodies final { IdleLoopNativeBody38h loop; MainFrameNativeBody90h frame; };
static_assert(sizeof(StackBodies) == 0xC8 && offsetof(StackBodies, frame) == 0x38);
static_assert(std::is_trivially_destructible_v<StackBodies>);
struct Unwind final
{
    IdleLoopNativeBody38h& loop;
    MainFrameNativeBody90h& frame;
    MainFrameApplicationNativeServices& services;
    int state = 0;
    ~Unwind() noexcept
    {
        // 原593880/59388B/593896。展开动作再抛出必须终止，不吞掉首次异常。
        // 本地catch已在state4->3释放标题；state2不补477620或432800。
        if (state < 0) return;
        if (state >= 2) ReleaseMainFrameNativeAt40EA80(frame, services.window.update, services.window.creation.thunks);
        else if (state == 1) ReleaseMainFrameBaseNativeAt40CC70(frame, services.window.creation.thunks);
        ReleaseIdleLoopArraysNativeAt40E940(loop, services.arrays);
    }
};
void CreateFailure(IMainFrameApplicationNativeApi& api)
{
    static_cast<void>(api.MessageBoxA(nullptr, creationError, caption, MB_ICONERROR));
    throw 2; // 原catch不读取这个整数。
}
}
int RunMainFrameApplicationNativeAt4342A0(LPSTR, int show, IMainFrameApplicationNativeSource& source,
    MainFrameApplicationNativeServices& services)
{
    MainFrameWindowNativeScope metadata(services.window);
    StackBodies bodies;
    auto& loop = bodies.loop;
    auto& frame = bodies.frame;
    InitializeIdleLoopNativeAt4342D1(loop);
    Unwind unwind{loop, frame, services};
    auto& dispatch = services.window.dispatch;
    RegisterAppModuleMessageLoopNativeAt434310(dispatch.module, dispatch.publication, &loop, dispatch.loops);
    InitializeMainFrameBaseNativeAt434379(frame);
    unwind.state = 1;
    InitializeUpdateUiNativeAt40B600(frame.updateAt3C, services.window.update);
    InitializeMainFrameTailNativeAt4343C6(frame, MainFrameWindowNativeConstructorTables());
    int catchResult = 0;
    int result;
    Title title;
    const char* phase = "pre-window";
    unwind.state = 3;
    try
    {
        source.EnsureRootAt413BE0();
        if (!source.InitializeBeforeWindowAt4778B0(source.RootAt6198C4())) throw 1;
        auto& api = services.api;
        const auto screenWidth = static_cast<DWORD>(api.GetSystemMetrics(0));
        const auto screenHeight = static_cast<DWORD>(api.GetSystemMetrics(1));
        const auto height = Read(source.EnvironmentAt4180C0()->heightAt1C);
        const auto width = Read(source.EnvironmentAt4180C0()->widthAt18);
        RECT rectangle{0, 0, Signed(width), Signed(height)};
        const auto adjustedExStyle = Read(source.EnvironmentAt4180C0()->exStyleAt24);
        const auto adjustedStyle = Read(source.EnvironmentAt4180C0()->styleAt20);
        static_cast<void>(api.AdjustWindowRectEx(&rectangle, adjustedStyle, FALSE, adjustedExStyle));
        const auto x = Signed(static_cast<DWORD>(rectangle.left) - static_cast<DWORD>(rectangle.right) + screenWidth) / 2;
        const auto outerHeight = static_cast<DWORD>(rectangle.bottom) - static_cast<DWORD>(rectangle.top);
        const auto y = Signed(screenHeight) <= Signed(outerHeight) ? Signed(0u - static_cast<DWORD>(rectangle.top)) :
            Signed(static_cast<DWORD>(rectangle.top) - static_cast<DWORD>(rectangle.bottom) + screenHeight) / 2;
        static_cast<void>(api.OffsetRect(&rectangle, x, y));
        auto* environment = source.EnvironmentAt4180C0();
        const auto sourceChars = Read(*reinterpret_cast<const char* const*>(&environment->titleAt10));
        auto* copied = CloneRuntimeCStringDataAt414540(reinterpret_cast<ATL::CStringData*>(const_cast<char*>(sourceChars)) - 1);
        const char* chars = reinterpret_cast<const char*>(copied + 1);
        std::memcpy(&title.value, &chars, 4);
        unwind.state = 4;
        environment = source.EnvironmentAt4180C0();
        auto* append = Read(*reinterpret_cast<const char* const*>(&environment->appendTitleAt14));
        if (Read(reinterpret_cast<const ATL::CStringData*>(append)[-1].nDataLength))
        {
            AppendRuntimeCStringAt413550(title.value, separator, 2);
            environment = source.EnvironmentAt4180C0();
            append = Read(*reinterpret_cast<const char* const*>(&environment->appendTitleAt14));
            const auto length = Read(reinterpret_cast<const ATL::CStringData*>(append)[-1].nDataLength);
            AppendRuntimeCStringAt413550(title.value, append, length);
            chars = Read(*reinterpret_cast<const char* const*>(&title.value));
        }
        const auto exStyleValue = Read(source.EnvironmentAt4180C0()->exStyleAt24);
        const auto styleValue = Read(source.EnvironmentAt4180C0()->styleAt20);
        const auto atom = RegisterMainFrameWindowClassNativeAt4316F0(services.windowClass,
            &frame.windowAt00.previousAt20, services.classes);
        const auto style = styleValue ? styleValue : 0x06CF0000u;
        const auto exStyle = exStyleValue ? exStyleValue : 0x00040100u;
        auto* thunk = Read(frame.windowAt00.thunkAt14);
        if (!thunk)
        {
            thunk = AllocateWindowThunkNativeAt53F0E4(services.window.creation.thunks);
            frame.windowAt00.thunkAt14 = thunk;
            if (!thunk) { api.SetLastError(14); CreateFailure(api); }
        }
        InitializeWindowThunkNativeAt53F1E6(thunk, 0, nullptr, services.window.creation.thunks);
        if (!atom) CreateFailure(api);
        PushMainFrameWindowCreationNativeAt434592(frame, services.window);
        phase = "CreateWindowExA";
        const auto created = api.CreateWindowExA(exStyle, MAKEINTRESOURCEA(atom), chars, style,
            rectangle.left, rectangle.top,
            Signed(static_cast<DWORD>(rectangle.right) - static_cast<DWORD>(rectangle.left)),
            Signed(static_cast<DWORD>(rectangle.bottom) - static_cast<DWORD>(rectangle.top)), nullptr,
            style & WS_CHILD ? reinterpret_cast<HMENU>(&frame) : nullptr,
            Read(services.classes.imageInstance), nullptr);
        if (!created) CreateFailure(api);
        // 原434618先缓存frame HWND，随后捕获Env；绝不发布CreateWindowEx的返回值。
        const auto window = Read(frame.windowAt00.windowAt04);
        source.EnvironmentAt4180C0()->windowAt2C = reinterpret_cast<DWORD>(window);
        source.EnsureRootAt413BE0();
        phase = "post-window";
        if (!source.InitializeAfterWindowAt489C90(source.RootAt6198C4()))
        {
            phase = "failed-post-loop";
            static_cast<void>(services.window.callbacks.PostMessageA(Read(frame.windowAt00.windowAt04), WM_CLOSE, 0, 0));
            catchResult = RunIdleLoopNativeAt43E980(loop, Read(frame.windowAt00.windowAt04), services.idleRoot, services.messages, services.arrays);
            throw 0;
        }
        static_cast<void>(api.ShowWindow(Read(frame.windowAt00.windowAt04), show));
        phase = "message-loop";
        result = RunIdleLoopNativeAt43E980(loop, Read(frame.windowAt00.windowAt04), services.idleRoot, services.messages, services.arrays);
        phase = "title-release";
        unwind.state = 3;
        ReleaseRuntimeCStringHeader(reinterpret_cast<ATL::CStringData*>(const_cast<char*>(chars)) - 1);
    }
    catch (...)
    {
        // 不改变原catch的返回/展开资格；保留Win32 last-error，只记录落入handler的阶段。
        const auto lastError = ::GetLastError();
        Logging::WriteFormat(Logging::Level::Error,
            "MainFrame 4342A0 original catch: phase=%s state=%d result=%d.", phase, unwind.state, catchResult);
        ::SetLastError(lastError);
        if (unwind.state == 4)
        {
            unwind.state = 3;
            [](RuntimeCString* value) noexcept { DestroyRuntimeCStringAt40CD40(value); }(&title.value);
        }
        unwind.state = 5; // 原catchHigh；无独立action，退出后退至完整frame资格2。
        result = catchResult;
    }
    // 原434691先退到state2：以下异常必须外传，不能重新进入上面的运行catch。
    unwind.state = 2;
    source.EnsureRootAt413BE0();
    source.ReleaseSubsystemsAt477620(source.RootAt6198C4());
    static_cast<void>(RemoveAppModuleMessageLoopNativeAt432800(dispatch.module, dispatch.publication, dispatch.loops));
    ReleaseMainFrameNativeAt40EA80(frame, services.window.update, services.window.creation.thunks);
    ReleaseIdleLoopArraysNativeAt4346DA(loop, services.arrays);
    unwind.state = -1;
    return result;
}
}
