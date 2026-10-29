#pragma once
#include <Runtime/SceneManagerNative.h>
#include <Runtime/SceneManagerPublicNativeReferences.h>
#include <Runtime/RuntimeCStringResult.h>
#include <cstddef>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
class SceneManagerRuntime;
// 显式原生/既有受控域，只借一次选中的receiver，不拥有或复制业务状态。
// 没有失败换域。原生body为null也不转换成受控空对象。
class SceneManagerView final
{
public:
    explicit SceneManagerView(SceneManagerRuntime* controlled) noexcept:controlled_(controlled){}
    SceneManagerView(SceneManagerRuntime& controlled) noexcept:controlled_(&controlled){}
    SceneManagerView(SceneManagerNativeBody48h* body,SceneManagerNativeServices& services) noexcept
        :body_(body),services_(&services){}
    bool IsNative() const noexcept{return services_!=nullptr;}
    explicit operator bool() const noexcept{return IsNative()?body_!=nullptr:controlled_!=nullptr;}
    const void* Address() const noexcept{return IsNative()?static_cast<const void*>(body_):controlled_;}
    SceneManagerNativeBody48h* NativeBody() const noexcept{return body_;}
    SceneManagerRuntime* ControlledBody() const noexcept{return controlled_;}
    SceneManagerNativeServices& NativeServices() const noexcept{return *services_;}
    void QueueScene(const char*) const;
    void PushActiveScene(const char*) const;
    void ClearScenes() const;
    void PopThenActivatePendingAt4C1760() const;
    bool HasPendingSceneWork() const;
    bool RunSceneTasks(std::int32_t) const;
    std::size_t ActiveCount() const;
    std::size_t PendingCount() const;
    const RuntimeCString& PendingCommandSlotAt1C() const;
    RuntimeCStringResult ClonePendingCommandAt414540() const;
    std::string_view ReadPendingCommandAt1C() const;
    void WritePendingCommandAt1C(std::string_view) const;
    const RuntimeCString& DemoNameSlotAt3C() const;
    void SetDemoNameAt4D6820(const char*) const;
    std::int32_t ReadSceneMarkerAt38() const;
    void WriteSceneMarkerAt38(std::int32_t) const;
    void ConfigurePublicAt4703D0(std::int32_t) const;
    void ClearPublicAt46DB90() const;
    SceneManagerModeButtonsNativeReference AcquireNativeModeButtonsAt47B7B0() const;
    SceneManagerOptionCharNativeReference AcquireNativeSelectorAt478A30() const;
private:
    SceneManagerRuntime* controlled_=nullptr;
    SceneManagerNativeBody48h* body_=nullptr;
    SceneManagerNativeServices* services_=nullptr;
};
}
