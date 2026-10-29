#include <Runtime/ReadScriptLayerServicesRuntime.h>
#include <Runtime/DrawManagerLayerRuntime.h>
#include <Runtime/FreeScrollRuntime.h>
#include <Runtime/OpacityWindowRuntime.h>
#include <Runtime/GameBuildProfile.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
ReadScriptLayerServicesRuntime::ReadScriptLayerServicesRuntime(DrawLayersView manager,
    IFreeScrollOperations& scroll, Platform::MouseView mouse,
    Platform::KeyboardView keyboard, IOpacityDialogUpdateServices& opacity) noexcept
    : manager_(manager), scroll_(scroll), controlled_{mouse,keyboard,&opacity} {}
ReadScriptLayerServicesRuntime::ReadScriptLayerServicesRuntime(IDrawLayersViewSource& manager,IFreeScrollOperations& scroll,
    IOpacityWindowInputSource& opacity) noexcept:source_(&manager),scroll_(scroll),native_(&opacity){}
DrawLayersView ReadScriptLayerServicesRuntime::CaptureLayers() const
{return source_?source_->CaptureDrawLayersAt413BE0():manager_;}
namespace
{
class FixedSource final:public IDrawLayersViewSource
{public:explicit FixedSource(DrawLayersView view):view_(view){}DrawLayersView CaptureDrawLayersAt413BE0() override{return view_;}
private:DrawLayersView view_;};
}

bool ReadScriptLayerServicesRuntime::PrepareLayersAt46AD20(const std::int32_t mode)
{
    // 正文 4B2846 push 0；选择 4B132A push 1，不能把两条 caller 统一为 0。
    // 4B283B/4B131F先捕获外层manager；46AD20内七槽再各自取根。
    if(source_){static_cast<void>(CaptureLayers());return PrepareDrawLayersViewAt46AD20(*source_,mode)!=0;}
    FixedSource fixed(manager_);return PrepareDrawLayersViewAt46AD20(fixed,mode)!=0;
}

bool ReadScriptLayerServicesRuntime::PrepareLayersAt46A980()
{
    if(source_){static_cast<void>(CaptureLayers());return PrepareDrawLayersViewAt46A980(*source_)!=0;}
    FixedSource fixed(manager_);return PrepareDrawLayersViewAt46A980(fixed)!=0;
}

void ReadScriptLayerServicesRuntime::SetLayerVisible(const std::uint32_t slot,
    const std::uint8_t requested, const std::int32_t argument)
{
    const auto physicalSlot = slot == 5 ? StartupSelectionLayerIndex()
        : slot == 6 ? StartupOpacityLayerIndex() : slot;
    (void)CaptureLayers().RequestOwnedLayerAtVslot10(physicalSlot,requested,argument);
}

DrawOpacityView ReadScriptLayerServicesRuntime::OpacityAtSlotSix() const
{
    return CaptureLayers().OpacityAtSlotSix(native_,controlled_);
}

void ReadScriptLayerServicesRuntime::ResetOpacityAt47EE40()
{
    OpacityAtSlotSix().ResetAtVslot18();
}

void ReadScriptLayerServicesRuntime::BeginHiddenWindowAt4ADF70()
{
    // 4B3A33 的 caller 不检查 AL；false 仍继续执行随后七层隐藏链。
    (void)scroll_.BeginAt4ADF70();
}

void ReadScriptLayerServicesRuntime::EndHiddenWindowAt4AC970() { scroll_.EndAt4AC970(); }
bool ReadScriptLayerServicesRuntime::UpdateHiddenWindowAt4BDD00() { return scroll_.UpdateAt4BDD00(); }
bool ReadScriptLayerServicesRuntime::UpdateLayerSixAt4BE530()
{
    return OpacityAtSlotSix().TickAt4BE530();
}
}
