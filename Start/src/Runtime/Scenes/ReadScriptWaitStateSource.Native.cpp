#include <Runtime/ReadScriptWaitStateSource.h>
#include <Runtime/GameBuildProfile.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/InputRootNative.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/UnSaveDataRuntime.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
bool ReadFixedHeld(const KeyboardManagerNativeBody438h& keyboard, unsigned action)
{
    // 4C3456/4AA996：begin、end和buffer各固定一次；不能借4198D0逐项重读end。
    const auto& descriptor = keyboard.actionsAt14[action];
    auto at = reinterpret_cast<std::uintptr_t>(descriptor.begin);
    const auto end = reinterpret_cast<std::uintptr_t>(descriptor.end);
    if (at == end) return false;
    const auto buffer = keyboard.bufferAt10;
    do
    {
        if (*reinterpret_cast<const std::uint8_t*>(at + buffer + 1u) != 0) return true;
        at += 3u;
    } while (at != end);
    return false;
}
}

RootReadScriptWaitStateSource::RootReadScriptWaitStateSource(EnvironmentRootOwner& environment,
    InputRootOwner& input, RootRuntimeOwner& root, IScrVmCommandErrorSink& errors,
    ScriptNamedValueNativeTables names, IRuntimeStringManagerSource& strings) noexcept
    : environment_(environment), input_(input), root_(root), errors_(errors), names_(names), strings_(strings) {}

const SaveDataEnvironmentArchive& RootReadScriptWaitStateSource::CaptureSaveAt4180C0()
{
    return *static_cast<SaveDataEnvironmentArchive*>(
        ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 1));
}

UnSaveDataRuntime& RootReadScriptWaitStateSource::CaptureUnSaveAt4180C0()
{
    return *static_cast<UnSaveDataRuntime*>(
        ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 2));
}

bool RootReadScriptWaitStateSource::ReadPressedAt47F4E0(std::int32_t action)
{
    auto* const input = input_.CaptureAt418210();
    return ReadKeyboardPressedAt47F4E0(
        *ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(input->keyboardAt18), action);
}

bool RootReadScriptWaitStateSource::ReadHeldAt4198D0(std::int32_t action)
{
    auto* const input = input_.CaptureAt418210();
    return ReadKeyboardHeldAt4198D0(
        *ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(input->keyboardAt18), action);
}

bool RootReadScriptWaitStateSource::MousePressedEdge(std::size_t button)
{
    auto* const input = input_.CaptureAt418210();
    const auto* const mouse = ResolveEnvironmentObjectReference<MouseManagerNativeBody3Ch>(input->mouseAt00);
    // 4C507A/4B0740/4AA9D6：先previous，再current，最后countdown；latched不参与。
    return mouse->previousAt30[button] == 0 && mouse->pressedAt10[button] != 0 &&
        mouse->suppressionAt28 == 0;
}

void RootReadScriptWaitStateSource::SetMouseCountdownAt28(std::int32_t value)
{
    // 4AC526/4AC5DF：写入自身有独立的Input捕获，不沿用此前边沿查询的鼠标。
    auto* const input = input_.CaptureAt418210();
    ResolveEnvironmentObjectReference<MouseManagerNativeBody3Ch>(input->mouseAt00)->suppressionAt28 =
        static_cast<std::uint32_t>(value);
}

bool RootReadScriptWaitStateSource::SuppressEffectAt4C3430()
{
    // 4AC50D/4AC585：即使4C3430不消费ECX，也必须保留caller的slot5捕获和解引用叶。
    static_cast<void>(ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 5));
    auto* const input = input_.CaptureAt418210();
    if (ReadFixedHeld(*ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(input->keyboardAt18), 6))
        return true;
    if ((*reinterpret_cast<const volatile std::uint8_t*>(&CaptureUnSaveAt4180C0().flagsAt14_) & 2u) != 0) return true;
    return CaptureSaveAt4180C0().valueAt6F == 0;
}

bool RootReadScriptWaitStateSource::SkipEffectAt4AA970()
{
    // 4AC59E/4AC5F7：slot5在helper之前；4AA9B1仅在action5未命中时另取Input。
    static_cast<void>(ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 5));
    auto* const input = input_.CaptureAt418210();
    if (ReadFixedHeld(*ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(input->keyboardAt18), 5))
        return true;
    return MousePressedEdge(0);
}

std::int32_t RootReadScriptWaitStateSource::ReadWaitValueAfter413BE0(unsigned index)
{
    // 4C5135/4C5156/4C5178分别Ensure和47CD00；4ADF40之后原样读+00/+04/+08。
    root_.EnsureCreatedAt413BE0();
    auto* const script = root_.Published()->ScriptAt47CD00();
    const auto* const wait = ResolveEnvironmentObjectReference<ScriptWaitNative>(script->waitAt18);
    return *reinterpret_cast<const std::int32_t*>(reinterpret_cast<std::uintptr_t>(wait) + index * 4u);
}

void RootReadScriptWaitStateSource::WriteWaitSkippedAfter413BE0(std::int32_t value)
{
    // 01–04原命名表没有wait_skiped出口；等待/跳过仍由共享WaitFlow执行，不占用旧脚本槽。
    if (!CompiledGameProfile().waitSkipResultEnabled) return;
    // 4C50BE/4C51D6/4AC54A：先固定root；字符串回调之后才从该root获取当前CParams。
    root_.EnsureCreatedAt413BE0();
    auto* const root = root_.Published();
    static_cast<void>(ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 0));
    const auto index = ResolveScriptNamedValueNativeAt498020("wait_skiped", names_, strings_);
    WriteScriptValueAt4C8220(root->ParamsAt47BDB0(), index, value, errors_);
}

void RootReadScriptWaitStateSource::ClearRootByteAt1B0After413BE0()
{
    // 4AC67D..4AC689：独立Ensure之后取当前root，不复用先前调用的正文。
    root_.EnsureCreatedAt413BE0();
    root_.Published()->Frame().FrameWaitByteAt1B0() = 0;
}
}
