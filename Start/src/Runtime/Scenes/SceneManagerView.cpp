#include <Runtime/SceneManagerView.h>
#include <Runtime/SceneManagerRuntime.h>
#include <cstring>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
SceneControllerNativeBody28h* Controller(SceneManagerNativeBody48h* body)
{return ResolveEnvironmentObjectReference<SceneControllerNativeBody28h>(body->controllerAt10);}
}
void SceneManagerView::QueueScene(const char* name) const
{
    if(!IsNative()){controlled_->Dispatch().QueueScene(name);return;}
    SceneManagerNativeScope scope(*services_);
    QueueSceneControllerNativeAt47F966(*Controller(body_),name,services_->controller);
}
void SceneManagerView::PushActiveScene(const char* name) const
{
    if(!IsNative()){controlled_->Dispatch().PushActiveScene(name);return;}
    SceneManagerNativeScope scope(*services_);auto* controller=Controller(body_);
    auto* factory=ResolveEnvironmentObjectReference<SceneFactoryNativeBody4h>(controller->factoryAt1C);
    PushSceneTaskNativeAt40B050(controller->activeAt10,factory->vptrAt00->createAt00(factory,name),services_->controller);
}
void SceneManagerView::ClearScenes() const
{
    if(!IsNative()){controlled_->Dispatch().ClearScenes();return;}
    SceneManagerNativeScope scope(*services_);ClearSceneControllerNativeAt4C14A5(*Controller(body_));
}
void SceneManagerView::PopThenActivatePendingAt4C1760() const
{
    if(!IsNative()){controlled_->PopThenActivatePendingAt4C1760();return;}
    SceneManagerNativeScope scope(*services_);ActivatePendingSceneNativeAt4C1782(*Controller(body_),services_->controller);
}
bool SceneManagerView::HasPendingSceneWork() const
{
    if(!IsNative())return controlled_->Dispatch().HasPendingSceneWork();
    return HasSceneControllerWorkNativeAt4836AA(*Controller(body_));
}
bool SceneManagerView::RunSceneTasks(std::int32_t argument) const
{
    if(!IsNative())return controlled_->Dispatch().RunSceneTasks(argument);
    SceneManagerNativeScope scope(*services_);
    // 原4836AA工作资格由caller检查，4836CD沿固定manager直接进调度，不重复读两队列。
    auto* scheduler=ResolveEnvironmentObjectReference<TaskControllerNativeBody18h>(body_->schedulerAt04);
    RunTaskControllerNativeAt46BC90(*scheduler,argument,services_->scheduler);return true;
}
std::size_t SceneManagerView::ActiveCount() const
{return IsNative()?std::uint32_t(ActiveSceneCountNative(*Controller(body_))):controlled_->Dispatch().Controller().ActiveCount();}
std::size_t SceneManagerView::PendingCount() const
{return IsNative()?PendingSceneCountNative(*Controller(body_)):controlled_->Dispatch().Controller().PendingCount();}
const RuntimeCString& SceneManagerView::PendingCommandSlotAt1C() const
{return IsNative()?body_->commandAt1C:controlled_->PendingCommandSlotAt1C();}
RuntimeCStringResult SceneManagerView::ClonePendingCommandAt414540() const
{
    return RuntimeCStringResult([&](RuntimeCString* out)
    {
        const auto& value=PendingCommandSlotAt1C();
        auto* header=reinterpret_cast<ATL::CStringData*>(const_cast<char*>(value.GetString()))-1;
        auto* cloned=CloneRuntimeCStringDataAt414540(header);
        // Clone引用交给隐藏返回槽，不经CString赋值或额外析构。
        const auto* characters=reinterpret_cast<const char*>(cloned+1);std::memcpy(static_cast<void*>(out),&characters,sizeof(characters));
    });
}
std::string_view SceneManagerView::ReadPendingCommandAt1C() const{return RuntimeStringView(PendingCommandSlotAt1C());}
void SceneManagerView::WritePendingCommandAt1C(std::string_view value) const
{
    if(!IsNative()){controlled_->WritePendingCommandAt1C(value);return;}
    SetRuntimeCStringAt437680(body_->commandAt1C,value.empty()?"":value.data(),
        std::bit_cast<std::int32_t>(std::uint32_t(value.size())));
}
const RuntimeCString& SceneManagerView::DemoNameSlotAt3C() const
{return IsNative()?body_->demoNameAt3C:controlled_->DemoNameSlotAt3C();}
void SceneManagerView::SetDemoNameAt4D6820(const char* name) const
{
    if(!IsNative()){controlled_->SetDemoNameAt4D6820(name);return;}
    SetRuntimeCStringAt437680(body_->demoNameAt3C,name,name?static_cast<int>(std::strlen(name)):0);
}
std::int32_t SceneManagerView::ReadSceneMarkerAt38() const
{return IsNative()?Read(body_->unwrittenAt38):controlled_->ReadSceneMarkerAt38();}
void SceneManagerView::WriteSceneMarkerAt38(std::int32_t value) const
{if(IsNative())body_->unwrittenAt38=value;else controlled_->WriteSceneMarkerAt38(value);}
void SceneManagerView::ConfigurePublicAt4703D0(std::int32_t mode) const
{if(IsNative())ConfigureSceneManagerNativeAt4703D0(*body_,mode,*services_);else controlled_->PublicDialogs().ConfigureAt4703D0(mode);}
void SceneManagerView::ClearPublicAt46DB90() const
{
    if(!IsNative()){controlled_->PublicDialogs().ClearAt46DB90();return;}
    SceneManagerNativeScope scope(*services_);ClearSceneManagerPublicNativeAt46DB90(*body_);
}
SceneManagerModeButtonsNativeReference SceneManagerView::AcquireNativeModeButtonsAt47B7B0() const
{
    if(!IsNative())throw std::logic_error("Native mode reference requested from explicit controlled domain");
    return SceneManagerModeButtonsNativeReference(BorrowModeButtonsNativeAt47B7B0(*body_),services_->mode);
}
SceneManagerOptionCharNativeReference SceneManagerView::AcquireNativeSelectorAt478A30() const
{
    if(!IsNative())throw std::logic_error("Native selector reference requested from explicit controlled domain");
    return SceneManagerOptionCharNativeReference(BorrowOptionCharDialogNativeAt478A30(*body_),services_->dialog);
}
}
