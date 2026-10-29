#include "MusicDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime::MusicDialogNativeDetail
{
std::int32_t FirstClickedAt50FDB0(GuiClickedEventReferences& refs, std::int32_t clear)
{
    auto* position = Read(refs.begin);
    auto* const end = Read(refs.end);
    for (; position != end; ++position)
    {
        if (!Read(Resolve<GuiClickedEventNativeBody>(*position)->clickedFlagAt04)) continue;
        const auto result = Read(Resolve<GuiClickedEventNativeBody>(*position)->eventValueAt08);
        if (clear)
        {
            auto* event = Resolve<GuiClickedEventNativeBody>(*position);
            Read(event->vptrAt00)->clearAt04(event);
        }
        return result;
    }
    return -1;
}
}
namespace KaGuYa::Start::Runtime
{
using namespace MusicDialogNativeDetail;
void ClearMusicDialogChecksAt532340(Body& body, std::int32_t invalidate)
{
    auto& common = body.common;
    for (std::int32_t i = 0; i < ItemCount(); ++i)
    {
        Fresh selected;
        Read(common.vptrAt00)->findChildAt14(&common, &selected.slot, i + CheckTemplate().eventAt00);
        RefLife life{selected.slot};
        if (Read(Resolve<GuiCheckBoxNativeBody60h>(selected.slot)->checkedAt5C))
        {
            Resolve<GuiCheckBoxNativeBody60h>(selected.slot)->checkedAt5C = 0;
            if (invalidate)
            {
                auto* control = Resolve<Control>(selected.slot);
                const auto rectFunction = Read(control->vptrAt00)->rectAt3C;
                const auto* table = Read(common.vptrAt00);
                RECT rect;
                auto* result = rectFunction(control, &rect);
                table->invalidateAt64(&common, *result);
            }
        }
        life.Reload();
    }
}
void BeginMusicDialogNativeAt50D090(Body& body)
{
    ClearMusicDialogChecksAt532340(body, 0);
    auto& common = body.common;
    Read(common.vptrAt00)->writeEnabledAt24(&common, 1);
    Read(common.vptrAt00)->resetInputAt3C(&common);
    common.activeAt4C = nullptr;
    body.phaseAt5C = 3;
}
std::int32_t PollMusicDialogNativeAt52DD40(Body& body)
{
    auto& common = body.common;
    const auto phase = Read(body.phaseAt5C);
    if (phase != 0 && phase != 3 && phase != 4) return 0;
    if (phase == 0 || phase == 4)
    {
        auto* dirty = Read(common.vptrAt00)->dirtyAt00(&common);
        Read(dirty->vptrAt00)->addRectAt08(dirty, &body.boundsAt64);
        if (phase == 4)
        {
            Read(common.vptrAt00)->writeEnabledAt24(&common, 0);
            body.phaseAt5C = -1;
        }
        else body.phaseAt5C = 3;
    }
    if (phase != 4)
    {
        body.resultAt80 = -1;
        Read(common.vptrAt00)->processInputAt54(&common);
        const auto clicked = FirstClickedAt50FDB0(*Resolve<GuiClickedEventReferences>(body.eventsAt74), 1);
        if (clicked >= 0)
        {
            Fresh selected;
            Read(common.vptrAt00)->findChildAt14(&common, &selected.slot, clicked);
            RefLife life{selected.slot};
            if (!Read(Resolve<GuiCheckBoxNativeBody60h>(selected.slot)->checkedAt5C))
            {
                ClearMusicDialogChecksAt532340(body, 1);
                Resolve<GuiCheckBoxNativeBody60h>(selected.slot)->checkedAt5C = 1;
            }
            else Resolve<GuiCheckBoxNativeBody60h>(selected.slot)->checkedAt5C = 0;
            body.resultAt80 = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(clicked)
                - static_cast<std::uint32_t>(CheckTemplate().eventAt00));
            life.Reload();
        }
    }
    Read(common.vptrAt00)->flushDirtyAt58(&common);
    return 1;
}
std::int32_t MusicDialogNativeResultAt50AB80(const Body& body) noexcept { return Read(body.resultAt80); }
}
