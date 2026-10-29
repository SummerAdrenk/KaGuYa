#include "ModeButtonNative.Internal.h"
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace { thread_local ModeButtonNativeServices* current = nullptr; }
ModeButtonNativeScope::ModeButtonNativeScope(ModeButtonNativeServices& value) noexcept
    : previous_(current) { current = &value; }
ModeButtonNativeScope::~ModeButtonNativeScope() noexcept { current = previous_; }
ModeButtonNativeServices& ModeButtonNativeDetail::Services()
{
    if (!current) throw std::logic_error("ModeButton native call has no bound services");
    return *current;
}
InputRootNativeBody24h* ModeButtonInputNativeSource::CaptureAt418210()
{ return owner_.CaptureAt418210(); }
MouseManagerNativeBody3Ch* ModeButtonNativeDetail::CaptureMouse()
{
    // 旧导出误标成threadpool符号；原PE E8实际目标418210，每个调用点重新捕获。
    auto* root = Services().buttons.input.CaptureAt418210();
    return Resolve<MouseManagerNativeBody3Ch>(root->mouseAt00);
}
KeyboardManagerNativeBody438h* ModeButtonNativeDetail::CaptureKeyboard()
{
    auto* root = Services().buttons.input.CaptureAt418210();
    return Resolve<KeyboardManagerNativeBody438h>(root->keyboardAt18);
}
}
