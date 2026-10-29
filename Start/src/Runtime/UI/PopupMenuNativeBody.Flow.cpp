#include "PopupMenuNativeBody.Internal.h"
#include <Runtime/PopupMenuServices.h>
#include <Runtime/ModalDialogRunnerRuntime.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
using namespace PopupNativeDetail;
namespace
{
GraphicsSurfaceNativeBody& Captured(PopupMenuNativeBodyA4h& body)
{
    auto* surface = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(body.capturedAt5C);
    if (!surface) throw std::logic_error("Popup operation requires captured Surface body");
    return *surface;
}
void AddCaptureRect(PopupMenuNativeBodyA4h& body)
{
    const auto& sizeSource = Captured(body);
    const auto width = sizeSource.widthAt08;
    const auto height = sizeSource.heightAt0C;
    const auto& originSource = Captured(body);
    const auto x = originSource.originXAt10;
    const auto y = originSource.originYAt14;
    auto* dirty = body.common.vptrAt00->dirtyAt00(&body.common);
    dirty->vptrAt00->addXYWHAt10(dirty, x, y, width, height);
}
}
std::int32_t __fastcall PopupMenuNativeEntries::Poll(GuiDialogNativeBody5Ch* common, void*)
{
    auto& body = Popup(common);
    auto& deps = RequirePopupMenuNativeDependencies();
    for (;;)
    {
        switch (body.phaseAt6C)
        {
        case 0:
        {
            AddCaptureRect(body);
            const auto* table = common->vptrAt00;
            body.phaseAt6C = 1;
            body.pollStateAt70 = 0;
            table->flushDirtyAt58(common);
            return 1;
        }
        case 1:
            if (body.pollStateAt70 == 1) { RefreshPopupMenuAt4FFBE0(body); body.phaseAt6C = 3; continue; }
            if (body.pollStateAt70 != 0) continue; // 原同一switch自旋，不发明额外阶段。
            {
                Slot header;
                common->vptrAt00->findChildAt14(common, &header, 0);
                LocalReference lifetime{header};
                if (header.baseAt00)
                {
                    const auto rect = ReadRect(header);
                    const auto offset = PopupMenuCursorOffsetSample18();
                    const POINT logical{Add(rect.left, offset.x), Add(rect.top, offset.y)};
                    static_cast<void>(deps.input.InputAt418210());
                    deps.operations.MovePointerAt4512E0(logical);
                }
                body.pollStateAt70 = Add(body.pollStateAt70, 1);
                lifetime.Finish(true);
            }
            common->vptrAt00->flushDirtyAt58(common);
            return 1;
        case 2: continue;
        case 3:
            common->vptrAt00->processInputAt54(common);
            common->vptrAt00->flushDirtyAt58(common);
            return 1;
        case 4:
            AddCaptureRect(body);
            HidePopupMenuAt4FFAB0(body);
            common->vptrAt00->flushDirtyAt58(common);
            return 1;
        default:
            common->vptrAt00->writeEnabledAt24(common, 0);
            return 0;
        }
    }
}
void __fastcall PopupMenuNativeEntries::Flush(GuiDialogNativeBody5Ch* common, void*)
{
    auto& body = Popup(common);
    auto* dirty = common->vptrAt00->dirtyAt00(common);
    dirty->vptrAt00->coalesceAt04(dirty);
    dirty = common->vptrAt00->dirtyAt00(common);
    const auto count = std::bit_cast<std::int32_t>(dirty->vptrAt00->countAt18(dirty));
    if (!count) return;
    dirty = common->vptrAt00->dirtyAt00(common);
    auto* rectangle = dirty->vptrAt00->rectsAt1C(dirty);
    for (std::int32_t index = 0; index < count; ++index, ++rectangle)
    {
        const auto handle = Captured(body).handleAt04;
        if (handle)
            static_cast<void>(RequireGuiControlGraphicsSource().CompositionAt4182F0().DrawSurface(
                handle, 0, 0, nullptr, nullptr, rectangle, 0));
        common->vptrAt00->drawAt60(common, rectangle);
        RequireGuiDialogNativeScreenSource().InvalidateRectAt52603A(rectangle);
    }
    dirty = common->vptrAt00->dirtyAt00(common);
    dirty->vptrAt00->clearAt00(dirty);
}
std::int32_t __fastcall PopupMenuNativeEntries::End(GuiDialogNativeBody5Ch* common, void*)
{
    auto& deps = RequirePopupMenuNativeDependencies();
    const auto event = Popup(common).resultAt68;
    if (event >= 2 && event <= 5)
    {
        deps.modal.SynchronizeRuntimeAt413BE0();
        deps.operations.WriteScenePendingCommandAt1C(PopupMenuSceneCommandSample18(event));
    }
    else switch (event)
    {
    case 6:
        deps.modal.SynchronizeRuntimeAt413BE0();
        deps.operations.ReplayEnvironmentVoiceBatchAt42C4F0(); break;
    case 8:
        deps.modal.SynchronizeRuntimeAt413BE0();
        deps.operations.PopSceneAt5328E0(); break;
    case 9: case 10:
        if (event == 9) deps.operations.UnSaveAt4180C0().FlagsAt18() ^= 2;
        else deps.operations.UnSaveAt4180C0().FlagsAt14() ^= 2;
        deps.modal.SynchronizeRuntimeAt413BE0();
        if (deps.operations.ToolWindowEnabledAtSlot2())
        {
            deps.modal.SynchronizeRuntimeAt413BE0();
            deps.operations.RefreshToolWindowAt434FB0(0);
        }
        break;
    case 11: deps.operations.UnSaveAt4180C0().FlagsAt20() = 2; break;
    default: break;
    }
    return 1;
}
}
