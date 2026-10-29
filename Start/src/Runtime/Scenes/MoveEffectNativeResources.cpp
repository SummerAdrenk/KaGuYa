#include <Runtime/MoveEffectNativeResources.h>
#include <Runtime/PatternLayerNative.h>
#include <Runtime/FreeScrollNative.h>
#include <Runtime/SpriteNativeDrawing.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/PatternPixelFilterRuntime.h>
#include <Runtime/GuiControlResourceLoadRuntime.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/ScrResourceNameSource.h>

#include <bit>
#include <emmintrin.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Surface = GraphicsSurfaceNativeBody;
using Item = MoveEffectItemNativeBody3Ch;

std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b));
}
float SubFloat(float value, std::int32_t extent) noexcept
{
    // 49D142/49D1BD：有符号DWORD转单精度，随后单精度减法。
    return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(value), _mm_cvtepi32_ps(_mm_cvtsi32_si128(extent))));
}
Item* SavedItem(std::uintptr_t base, YtlReferenceControlRuntime* control) noexcept
{
    if (!base) return nullptr;
    // EDI/ESI固定；原零索引仍读取control+0C，不能把故障读优化掉。
    if (control)
        (void)*reinterpret_cast<const volatile std::uint32_t*>(
            reinterpret_cast<std::uintptr_t>(control) + 0x0C);
    return reinterpret_cast<Item*>(base);
}
std::uint32_t SpriteCount(const PatternLayerNativeBody80h& pattern) noexcept
{
    const auto* control = pattern.spritesAt34.controlAt08;
    return control ? static_cast<const EnvironmentObjectReferenceControl*>(control)->countAt10 : 0;
}
SpriteNativeBodyA0h* Sprite(PatternLayerNativeBody80h& pattern, std::uint32_t index) noexcept
{
    const auto& slot = pattern.spritesAt34;
    if (!slot.baseAt00) return nullptr;
    const auto stride = slot.controlAt08 ?
        static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C : 0;
    return reinterpret_cast<SpriteNativeBodyA0h*>(slot.baseAt00 + (slot.biasAt04 + index) * stride);
}
struct LocalReference final
{
    YtlReferenceSlotRuntime& slot;
    bool active = true;
    ~LocalReference() noexcept { if (active) ReleaseYtlReference(slot); }
    void ReleaseNormal() noexcept { active = false; ReleaseYtlReference(slot); }
};
void RenderSprites(PatternLayerNativeBody80h& pattern, YtlReferenceSlotRuntime& snapshot,
    const RECT& bounds, IGraphicsWindowSurfaceApi& drawing)
{
    for (std::uint32_t index = 0; index < SpriteCount(pattern); ++index)
    {
        if (!Sprite(pattern, index)->logicalNameAt0C.GetLength()) continue;
        // 49CEA2保留同一Sprite；回调换数组后仍恢复它，不重解新元素。
        auto* const sprite = Sprite(pattern, index);
        const auto x = sprite->xAt2C;
        const auto y = sprite->yAt30;
        sprite->xAt2C = Sub(x, bounds.left);
        sprite->yAt30 = Sub(y, bounds.top);
        (void)CompositeSpriteNativeAt46B390(*sprite,
            ResolveEnvironmentObjectReference<Surface>(snapshot), nullptr, drawing);
        // 原异常路径不恢复坐标。
        sprite->xAt2C = x;
        sprite->yAt30 = y;
    }
}
void FilterSnapshot(PatternLayerNativeBody80h& pattern, YtlReferenceSlotRuntime& snapshot,
    PatternLayerNativeRenderServices& services)
{
    const auto mode = std::bit_cast<std::int32_t>(pattern.filterAt40.modeAt04);
    if (mode <= 0 || mode == 10) return;
    if (mode == 3)
    {
        YtlReferenceSlotRuntime clone;
        CloneGraphicsSurfaceAt444470(clone, *ResolveEnvironmentObjectReference<Surface>(snapshot),
            0, services.surfaces, services.filters, services.strings);
        LocalReference cloneCleanup{clone}; // 12h只在444470完成后取得资格。
        const auto* source = ResolveEnvironmentObjectReference<Surface>(snapshot);
        auto* destination = ResolveEnvironmentObjectReference<Surface>(clone);
        (void)ApplyPatternPixelFilterNativeAt442900(services.filters, pattern.filterAt40,
            destination, source, nullptr);
        CopyAssignGraphicsSurfaceReferenceAt465870(snapshot, clone);
        cloneCleanup.ReleaseNormal(); // 40D2F0的EHFlags=5。
        return;
    }
    if (mode == 6)
    {
        // 49CF49先存参数地址；49CF5C在Params取根后、Pattern捕获前读值。
        const auto* arguments = pattern.filterAt40.argumentsAt08.begin;
        const auto root = services.names.CaptureParamsForNames();
        const auto id = std::bit_cast<std::int32_t>(*arguments);
        const auto names = root.CapturePatternNames();
        auto name = ArchiveResourceNameAt40B530(names.FindNameAt4AD1B0(id), services.strings);
        const auto* chars = name.Value().GetString();
        void* const receiver = services.resources.CaptureLoadReceiverAt413BE0();
        if (LoadGuiControlSurfaceReferenceAt480000(pattern.filterAt40.temporaryAt14,
            receiver, chars, services.resources, services.surfaces, services.strings))
        {
            (void)ApplyPatternPixelFilterNativeAt442900(services.filters, pattern.filterAt40,
                ResolveEnvironmentObjectReference<Surface>(snapshot), nullptr, nullptr);
        }
        // 真/假均解绑原Pattern+54h；加载异常不经过这里，不能使用预览的局部Filter。
        ReleaseYtlReference(pattern.filterAt40.temporaryAt14);
        name.ReleaseNormal(); // 49CFD0在解绑回调后重读CString，不使用先前chars。
        return;
    }
    (void)ApplyPatternPixelFilterNativeAt442900(services.filters, pattern.filterAt40,
        ResolveEnvironmentObjectReference<Surface>(snapshot), nullptr, nullptr);
}
}

void CaptureMoveSnapshotAt49CDE4(YtlReferenceSlotRuntime& itemSlot,
    PatternLayerNativeBody80h& sourcePattern, std::uint32_t mirror,
    PatternLayerNativeRenderServices& services)
{
    const auto itemBase = itemSlot.baseAt00;
    auto* const itemControl = itemSlot.controlAt08;
    GraphicsSurfaceNativeScope scope(services.surfaces);
    YtlReferenceSlotRuntime snapshot;
    auto* const created = NewGraphicsSurfaceAt40C2E0(services.surfaces, services.strings);
    ConstructGraphicsSurfaceReferenceAt451500(snapshot, created, 1);
    LocalReference snapshotCleanup{snapshot};
    const RECT bounds = ReadFreeScrollLocalUnionAt4ADDE0(sourcePattern);
    auto* const destination = ResolveEnvironmentObjectReference<Surface>(snapshot);
    const auto alpha = ResolveEnvironmentObjectReference<Surface>(sourcePattern.mainAt18)->alphaAt18;
    // 49CE67不检查AL；创建失败仍绘制/过滤并提交24h正文。
    (void)destination->CreateAt415010(services.surfaces, Sub(bounds.right, bounds.left),
        Sub(bounds.bottom, bounds.top), alpha, 1);
    RenderSprites(sourcePattern, snapshot, bounds, services.drawing);
    FilterSnapshot(sourcePattern, snapshot, services);

    auto* const target = &SavedItem(itemBase, itemControl)->snapshotAt28;
    CopyAssignGraphicsSurfaceReferenceAt465870(*target, snapshot);
    SavedItem(itemBase, itemControl)->offsetXAt34 = bounds.left;
    SavedItem(itemBase, itemControl)->offsetYAt38 = bounds.top;
    if (mirror & 1u)
    {
        auto* item = SavedItem(itemBase, itemControl);
        item->offsetXAt34 = Sub(0, item->offsetXAt34);
        item = SavedItem(itemBase, itemControl);
        item->currentXAt00 = SubFloat(item->currentXAt00, Sub(bounds.right, bounds.left));
        item = SavedItem(itemBase, itemControl);
        item->targetXAt08 = Sub(item->targetXAt08, Sub(bounds.right, bounds.left));
    }
    if (mirror & 2u)
    {
        auto* item = SavedItem(itemBase, itemControl);
        item->offsetYAt38 = Sub(0, item->offsetYAt38);
        item = SavedItem(itemBase, itemControl);
        item->currentYAt04 = SubFloat(item->currentYAt04, Sub(bounds.bottom, bounds.top));
        item = SavedItem(itemBase, itemControl);
        item->targetYAt0C = Sub(item->targetYAt0C, Sub(bounds.bottom, bounds.top));
    }
    // 原PE 5EEFD4[state14h] -> 541E55：正常减持失败terminate，不由state11h重试。
    snapshotCleanup.ReleaseNormal();
}
}
