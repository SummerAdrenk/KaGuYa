#include "GraffitiPageNative.Internal.h"
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/LayerWindowNative.h>
#include <Runtime/SceneManagerNative.h>

namespace KaGuYa::Start::Runtime
{
using namespace GraffitiPageNativeDetail;
std::uint8_t RunGraffitiPageNativeAt4C3630(GraffitiPageNativeBody20h& body,
    std::int32_t argument, GraffitiPageNativeServices& services)
{
    static_cast<void>(argument); // 原ret4，只承诺AL=1，不借此实参提供任何Source。
    GraffitiPageNativeScope scope(services);
    Read(body.vptrAt00)->updateAt08(&body);
    Read(body.vptrAt00)->drawAt0C(&body); // 4C3638：Update可换表，必须重新读取。
    return 1;
}
void UpdateGraffitiPageNativeAt4B6130(GraffitiPageNativeBody20h& body, GraffitiPageNativeServices& services)
{
    GraffitiPageNativeScope scope(services);
    switch (Read(body.phaseAt04))
    {
    case 0:
    {
        // 4B620A外层取根仍发生；46A980内部再逐槽取七次根，复用已证叶。
        static_cast<void>(services.source.CaptureDrawManagerAfter413BE0());
        static_cast<void>(PrepareDrawLayersAt46A980(services.source));
        auto* dialog = Dialog(body);
        dialog->vptrAt00->beginAt44(dialog);
        body.phaseAt04 = 1;
        break;
    }
    case 1:
    {
        auto* dialog = Dialog(body);
        if (dialog->vptrAt00->pollAt48(dialog) == 0)
        {
            dialog = Dialog(body); // 4B61C9：Poll回调可换base/bias/control/vptr。
            static_cast<void>(dialog->vptrAt00->endAt4C(dialog));
            body.phaseAt04 = 2;
        }
        break;
    }
    case 2:
    {
        auto* manager = services.source.CaptureSceneManagerAfter413BE0();
        auto* controller = ResolveEnvironmentObjectReference<SceneControllerNativeBody28h>(manager->controllerAt10);
        ActivatePendingSceneNativeAt4C1782(*controller, services.controller);
        // 4B6185在推进成功后才取Environment；不缓存恢复目标，不把phase改成完成态。
        auto* archive = ResolveEnvironmentArchiveAt417990(*services.source.CaptureEnvironmentAt4180C0(), 2);
        auto* field = reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(archive) + 4u);
        *field = Read(body.savedSlot2At18);
        break;
    }
    default: break;
    }
}
void DrawGraffitiPageNativeAt4B5860(GraffitiPageNativeBody20h& body, GraffitiPageNativeServices& services)
{
    GraffitiPageNativeScope scope(services);
    if (!Read(body.drawGateAt1C)) return;
    AdvanceDrawLayersAt46B240(*services.source.CaptureDrawManagerAfter413BE0());
    auto* manager = services.source.CaptureDrawManagerAfter413BE0();
    auto* zero = reinterpret_cast<LayerWindowNativeBody4Ch*>(DrawLayerAt418C10(*manager, 0));
    PropagateLayerWindowDirtyNativeAt4C4250(*zero);
    MergeGraffitiPageDirtyNativeAt4AA9F0(body, services);
    auto* list = List(services); // 4B58EF：不能沿用前两次root或Merge中的root。
    list->vptrAt00->drawDirtyAt18(list);
    auto* dialog = Dialog(body);
    dialog->vptrAt00->flushDirtyAt58(dialog);
}
void MergeGraffitiPageDirtyNativeAt4AA9F0(GraffitiPageNativeBody20h& body, GraffitiPageNativeServices& services)
{
    GraffitiPageNativeScope scope(services);
    // 原两只栈上dirty80，只写vptr/count，不复制或拥有两端Ref。
    GuiDirtyRectNativeBody80h layerSnapshot;
    GuiDirtyRectNativeBody80h dialogSnapshot;
    auto* list = List(services);
    auto* dirty = list->vptrAt00->dirtyAt14(list);
    const auto layerCount = std::bit_cast<std::int32_t>(dirty->vptrAt00->countAt18(dirty));
    if (layerCount != 0)
    {
        list = List(services);
        dirty = list->vptrAt00->dirtyAt14(list);
        auto* rectangles = dirty->vptrAt00->rectsAt1C(dirty);
        for (std::int32_t i = 0; i < layerCount; ++i)
        {
            const auto rectangle = rectangles[i];
            layerSnapshot.vptrAt00->addLTRBAt0C(&layerSnapshot,
                rectangle.left, rectangle.top, rectangle.right, rectangle.bottom);
        }
    }
    auto* dialog = Dialog(body);
    dirty = dialog->vptrAt00->dirtyAt00(dialog);
    const auto dialogCount = std::bit_cast<std::int32_t>(dirty->vptrAt00->countAt18(dirty));
    if (dialogCount != 0)
    {
        dialog = Dialog(body);
        dirty = dialog->vptrAt00->dirtyAt00(dialog);
        auto* rectangles = dirty->vptrAt00->rectsAt1C(dirty);
        for (std::int32_t i = 0; i < dialogCount; ++i)
        {
            const auto rectangle = rectangles[i];
            dialogSnapshot.vptrAt00->addLTRBAt0C(&dialogSnapshot,
                rectangle.left, rectangle.top, rectangle.right, rectangle.bottom);
        }
    }
    // 4AABF5每个dialog矩形重新捕获root；4AAC72每个layer矩形重解当前Ref。
    const auto toLayers = std::bit_cast<std::int32_t>(dialogSnapshot.countAt7C);
    for (std::int32_t i = 0; i < toLayers; ++i)
    {
        list = List(services);
        dirty = list->vptrAt00->dirtyAt14(list);
        dirty->vptrAt00->addRectAt08(dirty, &dialogSnapshot.rectsAt04[i]);
    }
    const auto toDialog = std::bit_cast<std::int32_t>(layerSnapshot.countAt7C);
    for (std::int32_t i = 0; i < toDialog; ++i)
    {
        dialog = Dialog(body);
        dirty = dialog->vptrAt00->dirtyAt00(dialog);
        dirty->vptrAt00->addRectAt08(dirty, &layerSnapshot.rectsAt04[i]);
    }
}
}
