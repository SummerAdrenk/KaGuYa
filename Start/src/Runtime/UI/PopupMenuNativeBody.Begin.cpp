#include "PopupMenuNativeBody.Internal.h"
#include <Runtime/PopupMenuServices.h>
#include <Runtime/ModalDialogRunnerRuntime.h>

namespace KaGuYa::Start::Runtime
{
using namespace PopupNativeDetail;
void __fastcall PopupMenuNativeEntries::Begin(GuiDialogNativeBody5Ch* common, void*)
{
    auto& body = Popup(common);
    auto& deps = RequirePopupMenuNativeDependencies();
    auto& source = deps.root.CaptureFrameSlotAt413BE0();
    Slot argument;
    TransferGraphicsSurfaceReferenceAt50DCA0(body.capturedAt5C, source, argument);
    common->vptrAt00->writeEnabledAt24(common, 1);
    ClearGuiClickedEventFlagsAt532200(body.eventsAt80);
    for (auto* it = common->childrenAt0C.begin; it != common->childrenAt0C.end; ++it)
    {
        auto& child = RequireGuiControl(*it);
        child.vptrAt00->resetAt08(&child);
        WriteVisible(*it, 0);
        WriteState(*it, 0);
    }
    body.resultAt68 = -1;
    common->activeAt4C = nullptr;
    ClearGuiNavigationReferencesAt4D80B0(common->navigationAt24);
    body.recordsAt8C.end = body.recordsAt8C.begin;
    body.recordsAt98.end = body.recordsAt98.begin;
    body.modeAt7C = deps.operations.UnSaveAt4180C0().ModeAt04();
    if (deps.operations.UnSaveAt4180C0().ModeAt04() != 1 &&
        deps.operations.UnSaveAt4180C0().ModeAt04() != 2) return;
    body.phaseAt6C = 0;
    const auto mode = deps.operations.UnSaveAt4180C0().ModeAt04();
    const auto layout = PopupMenuLayoutEventsSample18(mode);
    const auto navigation = PopupMenuNavigationSample18(mode);
    std::int32_t x = 0, y = 0;
    for (std::size_t index = 0; index != layout.size(); ++index)
    {
        Slot child;
        common->vptrAt00->findChildAt14(common, &child, layout[index]);
        LocalReference childLifetime{child};
        if (child.baseAt00)
        {
            const auto rect = ReadRect(child);
            if (index == 0)
            {
                x = Sub(Add(deps.operations.ReadGuiWidthAt18(), rect.left), rect.right);
                y = Sub(Add(rect.top, deps.operations.ReadGuiHeightAt1C()), rect.bottom);
            }
            else y = Add(y, Sub(rect.top, rect.bottom));
            auto& selected = RequireGuiControl(child);
            selected.xAt34 = x;
            selected.yAt38 = y;
            WriteVisible(child, 1);
            WriteChecked(child, 0);
            const auto hintId = HintFor(layout[index]);
            if (hintId >= 0)
            {
                Slot hint;
                common->vptrAt00->findChildAt14(common, &hint, hintId);
                LocalReference hintLifetime{hint};
                if (hint.baseAt00)
                {
                    const auto hintRect = ReadRect(hint);
                    const auto offset = PopupMenuHintOffsetSample18();
                    auto& target = RequireGuiControl(hint);
                    target.xAt34 = Sub(Add(Add(offset.x, x), hintRect.left), hintRect.right);
                    target.yAt38 = Sub(Sub(Add(Add(Add(offset.y, y), rect.bottom), hintRect.top),
                        hintRect.bottom), rect.top);
                    WriteState(hint, 1);
                }
                hintLifetime.Finish(true);
            }
        }
        childLifetime.Finish(true);
    }
    {
        Slot header;
        common->vptrAt00->findChildAt14(common, &header, 0);
        LocalReference headerLifetime{header};
        if (header.baseAt00)
        {
            const auto rect = ReadRect(header);
            auto& target = RequireGuiControl(header);
            target.xAt34 = x;
            target.yAt38 = Add(Sub(rect.top, rect.bottom), y);
            WriteVisible(header, 1);
            WriteState(header, 1);
        }
        headerLifetime.Finish(false); // 50E600..50E629正常尾不清本地Ref。
    }
    // 50E63D..50E8D7有独立factory1h/control18h，不能省略其失败点与正常释放。
    GuiFactoryConstructionLifetime factory;
    for (const auto& descriptor : navigation)
        CreateAndAppendGuiNavigation(common->navigationAt24, descriptor.input, descriptor.mappings.data());
    factory.Finish();
}
void HidePopupMenuAt4FFAB0(PopupMenuNativeBodyA4h& body)
{
    auto& common = body.common;
    for (auto* it = common.childrenAt0C.begin; it != common.childrenAt0C.end; ++it)
    {
        auto& child = RequireGuiControl(*it);
        child.vptrAt00->resetAt08(&child);
        WriteVisible(*it, 0);
    }
    Slot hint;
    common.vptrAt00->findChildAt14(&common, &hint, 1);
    LocalReference lifetime{hint};
    if (hint.baseAt00) { WriteVisible(hint, 1); WriteState(hint, 1); }
    body.phaseAt6C = -1;
    lifetime.Finish(false);
}
void RefreshPopupMenuAt4FFBE0(PopupMenuNativeBodyA4h& body)
{
    auto& deps = RequirePopupMenuNativeDependencies();
    auto& common = body.common;
    bool available = true;
    if (body.modeAt7C == 1)
    {
        deps.modal.SynchronizeRuntimeAt413BE0();
        available = deps.operations.TextGlyphCursorAt50() < 0;
    }
    for (const auto id : {2, 3, 4, 5, 9, 10, 6, 11, 8})
    {
        Slot child;
        common.vptrAt00->findChildAt14(&common, &child, id);
        LocalReference lifetime{child};
        if (child.baseAt00)
        {
            auto& selected = RequireGuiControl(child);
            if (selected.vptrAt00->readEnabledAt50(&selected))
            {
                switch (id)
                {
                case 2: case 3: case 4: case 5: WriteState(child, available); break;
                case 9:
                    WriteState(child, (deps.operations.UnSaveAt4180C0().FlagsAt18() & 1) != 0);
                    WriteChecked(child, (deps.operations.UnSaveAt4180C0().FlagsAt18() >> 1) & 1);
                    break;
                case 10:
                    WriteState(child, std::uint8_t(deps.operations.UnSaveAt4180C0().DwordAt0C()) != 0 &&
                        (deps.operations.UnSaveAt4180C0().FlagsAt14() & 1) != 0);
                    WriteChecked(child, (deps.operations.UnSaveAt4180C0().FlagsAt14() >> 1) & 1);
                    break;
                case 6:
                    WriteState(child, std::uint8_t(deps.operations.UnSaveAt4180C0().DwordAt0C()) != 0 &&
                        (deps.operations.UnSaveAt4180C0().FlagsAt18() & 2) == 0 &&
                        deps.operations.UnSaveAt4180C0().ArrayReferenceAt3C().StorageAt3C() != nullptr);
                    break;
                case 11:
                {
                    bool enabled = false;
                    if (std::uint8_t(deps.operations.UnSaveAt4180C0().DwordAt0C()) != 0 &&
                        (deps.operations.UnSaveAt4180C0().FlagsAt20() & 1) != 0)
                    {
                        deps.modal.SynchronizeRuntimeAt413BE0();
                        enabled = deps.operations.TextGlyphCursorAt50() < 0;
                    }
                    WriteState(child, enabled);
                    break;
                }
                case 8: WriteState(child, 1); break;
                }
                InvalidateChild(common, child);
            }
        }
        lifetime.Finish(false);
    }
}
}
