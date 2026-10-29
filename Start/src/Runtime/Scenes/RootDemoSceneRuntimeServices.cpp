#include <Runtime/RootDemoSceneRuntimeServices.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/InputRootNative.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SceneManagerRuntime.h>
#include <Runtime/ScriptSoundLeafSource.h>
#include <Runtime/DemoSoundManagerReceiver.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/UnSaveDataRuntime.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
bool MousePressedEdge(const MouseManagerNativeBody3Ch* mouse, unsigned button)
{
    const auto address = reinterpret_cast<std::uintptr_t>(mouse);
    return *reinterpret_cast<const volatile std::uint8_t*>(address + 0x30u + button) == 0 &&
        *reinterpret_cast<const volatile std::uint8_t*>(address + 0x10u + button) != 0 &&
        *reinterpret_cast<const volatile std::uint32_t*>(address + 0x28u) == 0;
}
}
RootDemoSceneRuntimeServices::RootDemoSceneRuntimeServices(EnvironmentRootOwner& environment,
    InputRootOwner& input, RootRuntimeOwner& root, IGraphicsScreenCalls& graphics) noexcept
    : environment_(environment), input_(input), root_(root), graphics_(graphics) {}

bool RootDemoSceneRuntimeServices::ShouldRequestCompletion()
{
    // 4B5FD6/4B6007/4B6025分别取Input；键盘成功时不能捕获鼠标。
    auto* const keyboard = ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(
        input_.CaptureAt418210()->keyboardAt18);
    if (ReadKeyboardPressedAt47F4E0(*keyboard, 0)) return true;
    const auto* const left = ResolveEnvironmentObjectReference<MouseManagerNativeBody3Ch>(
        input_.CaptureAt418210()->mouseAt00);
    if (MousePressedEdge(left, 0)) return true;
    // 4B0740只读previous/current/suppression；不读latched或secondary倒计时。
    const auto* const right = ResolveEnvironmentObjectReference<MouseManagerNativeBody3Ch>(
        input_.CaptureAt418210()->mouseAt00);
    return MousePressedEdge(right, 1);
}
void RootDemoSceneRuntimeServices::ClearGlobalDescriptorTarget(std::uint32_t index)
{
    // 4AB099..4AB116：原缺项返回null后仍在+04写入，不转成现代异常或伪成功。
    auto* const archive = ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), index);
    *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(archive) + 4u) = 0;
}
UnSaveDataRuntime& RootDemoSceneRuntimeServices::CaptureUnSave()
{
    return *static_cast<UnSaveDataRuntime*>(
        ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 2));
}
void RootDemoSceneRuntimeServices::ClearRootObjectAt24Binding()
{
    // 4B5EF9 -> 4B5F28：413270(nullptr)只停止音效，保留环境映射，不释放manager。
    root_.EnsureCreatedAt413BE0();
    root_.Published()->PostWindow().SoundLeafReceiverAt24().StopEffectsKeepingMappingsAt413270();
}
SceneManagerView RootDemoSceneRuntimeServices::CaptureSceneManager()
{
    root_.EnsureCreatedAt413BE0();
    return root_.Published()->PostWindow().BorrowSceneManagerAt30();
}
void RootDemoSceneRuntimeServices::QueueScene(const char* name)
{
    // 4B5F32/4B60E0各自重取根；Queue先释放active尾项，再追加借用的静态名字。
    CaptureSceneManager().QueueScene(name);
}
const RuntimeCString* RootDemoSceneRuntimeServices::CaptureDemoNameSlotAt49DA53()
{ return &CaptureSceneManager().DemoNameSlotAt3C(); }
void RootDemoSceneRuntimeServices::StopRootSlot24Sounds()
{
    root_.EnsureCreatedAt413BE0();
    root_.Published()->PostWindow().DemoSoundReceiverAt24().StopAndClearAllVoiceSlots();
}
void RootDemoSceneRuntimeServices::StopRootSlot24SoundsUsingDescriptor2ListAt30()
{
    // 49DB5C先固定Env2正文，49DBAC才Ensure根；不能把这两次捕获交换。
    auto& descriptor = CaptureUnSave();
    root_.EnsureCreatedAt413BE0();
    const auto sound = root_.Published()->PostWindow().SoundLeafReceiverAt24();
    sound.StopEffectsAt413270(&descriptor.mappingsAt30_);
}
void RootDemoSceneRuntimeServices::WriteDescriptor2DwordAt04(std::uint32_t value)
{ CaptureUnSave().ModeAt04() = value; }
void RootDemoSceneRuntimeServices::WriteDescriptor2DwordAt14(std::uint32_t value)
{ CaptureUnSave().FlagsAt14() = value; }
void RootDemoSceneRuntimeServices::ClearScreen(std::int32_t rect)
{ graphics_.ClearScreenIfLoaded(reinterpret_cast<const RECT*>(static_cast<std::uintptr_t>(rect))); }
void RootDemoSceneRuntimeServices::InvalidateScreenRect(const void* rect)
{ graphics_.InvalidatedRectIfLoaded(static_cast<const RECT*>(rect)); }
void RootDemoSceneRuntimeServices::AdvanceActiveSceneFromPending()
{ CaptureSceneManager().PopThenActivatePendingAt4C1760(); }
}
