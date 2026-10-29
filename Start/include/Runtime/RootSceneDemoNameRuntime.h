#pragma once
#include <Runtime/SceneManagerView.h>

#include <Runtime/ProgramRuntimeServices.h>
#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class SceneManagerRuntime;

// 49DA53/4CFE6A 的运行根入口；不缓存 SceneManager 或名字副本。
class RootSceneDemoNameRuntime final : public IProgramCurrentEnvironment
{
public:
    explicit RootSceneDemoNameRuntime(RootRuntimeOwner& root) noexcept;
    [[nodiscard]] const RuntimeCString* CaptureNameSlotAt49DA53();
    void WriteStringAt3C(const char* value) override;

private:
    [[nodiscard]] SceneManagerView CaptureSceneManager();
    RootRuntimeOwner& root_;
};
}
