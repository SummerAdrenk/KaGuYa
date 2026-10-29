#include <Runtime/ReadScriptMenuServicesRuntime.h>
#include <Runtime/SelectWindowLayerRuntime.h>
#include <Runtime/ToolWindowLayerRuntime.h>
#include <Runtime/ScrVmScriptCommandState.h>
#include <Runtime/ScrVmExecutableProfile.h>
#include <Runtime/ScrVmValueTable.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
ReadScriptMenuServicesRuntime::ReadScriptMenuServicesRuntime(DrawManagerLayerRuntime& layers,
    ISelectWindowStateServices& choices, IGraphicsTextApi& text, IGraphicsTextGlyphApi& glyphs,
    const ScrVmScriptCommandState& script, ScrVmValueTable& values,
    const ScrVmExecutableProfile& profile) noexcept
    : layers_(layers), controlled_{&choices,&text,&glyphs}, script_(script),
      values_(values), profile_(profile) {}
ReadScriptMenuServicesRuntime::ReadScriptMenuServicesRuntime(DrawLayersView layers,SelectWindowNativeWorkflowServices& choices,
    const ScrVmScriptCommandState& script,ScrVmValueTable& values,const ScrVmExecutableProfile& profile) noexcept
    :layers_(layers),native_(&choices),script_(script),values_(values),profile_(profile){}
ReadScriptMenuServicesRuntime::ReadScriptMenuServicesRuntime(IDrawLayersViewSource& source,SelectWindowNativeWorkflowServices& choices,
    const ScrVmScriptCommandState& script,ScrVmValueTable& values,const ScrVmExecutableProfile& profile) noexcept
    :source_(&source),native_(&choices),script_(script),values_(values),profile_(profile){}
DrawLayersView ReadScriptMenuServicesRuntime::CaptureLayers() const
{
    // 构造入口明确选择固定图或原逐点来源；Source失败直接外传，不退回固定图。
    return source_?source_->CaptureDrawLayersAt413BE0():layers_;
}

DrawSelectionView ReadScriptMenuServicesRuntime::Window() const
{
    return CaptureLayers().SelectionAtSlotFive(native_,controlled_);
}

void ReadScriptMenuServicesRuntime::RebuildChoicesAt4369E0()
{
    Window().RebuildAt4369E0();
}

void ReadScriptMenuServicesRuntime::ClearToolByteAt38()
{
    CaptureLayers().ToolAtSlotTwo().WriteLatchByteAt38(0); // 4B12B2：写 byte，而非调用工具条 Reset。
}

std::uint8_t ReadScriptMenuServicesRuntime::ReadScriptByteAt24() { return script_.ReadStateByteAt24(); }
std::int32_t ReadScriptMenuServicesRuntime::TickChoicesAt4BBFD0(const std::uint8_t blocked)
{
    return Window().TickAt4BBFD0(blocked);
}
void ReadScriptMenuServicesRuntime::WriteMenuResult(const std::int32_t ordinal)
{
    // 4B16E7..4B170B：498020 的名字索引交给当前 CParams，值不是 message ID。
    values_.WriteSigned(profile_.ResolveNamedValue("menu_result"), ordinal);
}
}
