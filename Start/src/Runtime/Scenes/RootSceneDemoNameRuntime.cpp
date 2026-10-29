#include <Runtime/RootSceneDemoNameRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SceneManagerRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
RootSceneDemoNameRuntime::RootSceneDemoNameRuntime(RootRuntimeOwner& root) noexcept
    : root_(root)
{
}

SceneManagerView RootSceneDemoNameRuntime::CaptureSceneManager()
{
    root_.EnsureCreatedAt413BE0();
    auto* const root = root_.Published();
    if (!root)
        throw std::logic_error("Demo name runtime root is absent after 413BE0");
    const auto scene = root->PostWindow().BorrowSceneManagerAt30();
    // 原版此处解引用；安全差异在实际访问处报错，不补造场景或默认名字。
    if (!scene.IsNative()&&!scene)
        throw std::logic_error("Demo name SceneManager +30 is absent");
    return scene;
}

const RuntimeCString* RootSceneDemoNameRuntime::CaptureNameSlotAt49DA53()
{
    return &CaptureSceneManager().DemoNameSlotAt3C();
}

void RootSceneDemoNameRuntime::WriteStringAt3C(const char* const value)
{
    CaptureSceneManager().SetDemoNameAt4D6820(value);
}
}
