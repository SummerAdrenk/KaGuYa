#include <Runtime/RootSaveLoadDialogNativeSource.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/ReadScriptInputNative.h>
#include <Runtime/NativeInputRuntime.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>

namespace KaGuYa::Start::Runtime
{
std::int32_t RootSaveLoadDialogNativeSource::ReadStepAt51CC28()
{
    auto* env = CaptureEnvironmentAt4180C0();
    auto* save = static_cast<SaveDataEnvironmentArchive*>(ResolveEnvironmentArchiveAt417990(*env, 1));
    // 51CC39的DWORD在第二次Capture之前已push；回调改旧Save也不能改变本次index。
    const auto index = *static_cast<const volatile std::uint32_t*>(&save->valueAt70);
    env = CaptureEnvironmentAt4180C0();
    static_cast<void>(ResolveEnvironmentArchiveAt417990(*env, 0));
    // 497903..14不读ECX；无符号cmp/sbb/and选择原PE6018A0，不是缺依赖时补默认值。
    constexpr std::int32_t steps[]{4, 8, 16, 32};
    return steps[index < 4 ? index : 0];
}

std::uint8_t RootSaveLoadDialogNativeSource::AccelerateAt4C3430()
{
    // 51CAD4..E4的slot5捕获是caller副作用；4C3430自身不读传入ECX。
    auto* env = CaptureEnvironmentAt4180C0();
    static_cast<void>(ResolveEnvironmentArchiveAt417990(*env, 5));
    return ReadScriptSuppressInputAt4C3430(dependencies_.environment, dependencies_.input.RootSource());
}

std::uint8_t RootSaveLoadDialogNativeSource::AccelerateAt4AA970()
{
    // 51CAED..FD是第二个独立slot5捕获，外层Advance保留短路，不提前两次取源。
    auto* env = CaptureEnvironmentAt4180C0();
    static_cast<void>(ResolveEnvironmentArchiveAt417990(*env, 5));
    return ReadScriptAccelerateInputAt4AA970(dependencies_.input.RootSource());
}

TransitRuntime* RootSaveLoadDialogNativeSource::CaptureTransitAfter413BE0()
{
    // 51CB24..75每帧先选当前Root+18，再按同一个Draw+20 Ref解Transit，不增持。
    auto* draw = static_cast<DrawManagerNativeBody2Ch*>(CaptureDrawAfter413BE0());
    return ResolveDrawTransit(*draw);
}

void RootSaveLoadDialogNativeSource::CommitScreenAt4B02E0()
{
    // 51CB9B push0在4182F0之前；4B02E0只用本次Graphics根，不重取或RenderScreen。
    auto* graphics = dependencies_.graphics.CaptureAt4182F0();
    dependencies_.graphics.InvalidatedRectCapturedAt4B02E0(*graphics, nullptr);
}
}
