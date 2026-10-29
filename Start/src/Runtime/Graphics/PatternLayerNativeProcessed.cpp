#include <Runtime/PatternLayerNative.h>
#include <Runtime/PatternPixelFilterRuntime.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/ScrResourceNameSource.h>
#include <Runtime/GuiDirtyRectNative.h>
#include <Runtime/LayerListNative.h>

#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = PatternLayerNativeBody80h;
GraphicsSurfaceNativeBody& Surface(const YtlReferenceSlotRuntime& slot)
{
    auto* const value = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(slot);
    return *value;
}
GuiDirtyRectNativeBody80h& Dirty(Body& body)
{
    auto* const value = ResolveGuiDirtyRectReference(body.dirtyAt70);
    return *value;
}
std::int32_t Mode(const Body& body) noexcept
    { return std::bit_cast<std::int32_t>(body.filterAt40.modeAt04); }
bool PixelMode(const Body& body) noexcept { return Mode(body) > 0 && Mode(body) != 10; }
std::uint32_t FirstArgument(const std::uint32_t* data)
{
    return *data;
}
LONG Subtract(LONG a, LONG b) noexcept
    { return std::bit_cast<LONG>(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b)); }
struct CloneRefCleanup final
{
    YtlReferenceSlotRuntime& slot;
    // 正常出口state1及EH均终止二次异常，但均重新读取局部Ref的control。
    ~CloneRefCleanup() noexcept { ReleaseYtlReference(slot); }
};
void Apply(Body& body, const GraphicsSurfaceNativeBody* source, const RECT* clip,
    IGraphicsPatternFilterCalls& filters)
{
    (void)ApplyPatternPixelFilterNativeAt442900(filters, body.filterAt40,
        ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(body.processedAt64), source, clip);
}
void CopyMain(Body& body, const RECT* clip, IGraphicsPatternFilterCalls& filters)
{
    const auto source = Surface(body.mainAt18).handleAt04;
    const auto target = Surface(body.processedAt64).handleAt04;
    filters.SurfaceCopyBlitRaw(target, source, 0, 0, nullptr, nullptr, clip, 0);
}
void LoadTemporary(Body& body, const RECT* clip, PatternLayerNativeRenderServices& services)
{
    // 443E80保存原参数首地址；CParams/Pattern两次取源之后443F52才读取DWORD。
    const auto* const arguments = body.filterAt40.argumentsAt08.begin;
    const auto root = services.names.CaptureParamsForNames();
    const auto names = root.CapturePatternNames();
    const auto id = std::bit_cast<std::int32_t>(FirstArgument(arguments));
    auto name = ArchiveResourceNameAt40B530(names.FindNameAt4AD1B0(id), services.strings);
    const char* const chars = name.Value().GetString();
    void* const receiver = services.resources.CaptureLoadReceiverAt413BE0();
    auto& temporary = body.filterAt40.temporaryAt14;
    if (!temporary.baseAt00)
    {
        auto* const created = NewGraphicsSurfaceAt40C2E0(services.surfaces, services.strings);
        ReleaseYtlReference(temporary);
        // 444063清cached ECX；不能套用另一次动态检查的407A80。
        if (created) ConstructGraphicsSurfaceReferenceAt451500(temporary, created, 1);
    }
    if (services.resources.LoadIntoAt4800A0(receiver, Surface(temporary), chars))
        Apply(body, nullptr, clip, services.filters);
    // 44411E只在正常路径释放成员；加载/错误展示抛出保留temp，只有name进入EH。
    ReleaseYtlReference(temporary);
    name.ReleaseNormal();
}
void ProcessRect(Body& body, RECT& affected, const RECT* original, PatternLayerNativeRenderServices& services)
{
    if (!body.processedAt64.baseAt00)
    {
        const auto initialMode = Mode(body);
        {
            YtlReferenceSlotRuntime result;
            CloneGraphicsSurfaceAt444470(result, Surface(body.mainAt18), initialMode != 11,
                services.surfaces, services.filters, services.strings);
            CloneRefCleanup cleanup{result};
            CopyAssignGraphicsSurfaceReferenceAt465870(body.processedAt64, result);
        }
        if (initialMode == 11) affected = body.boundsAt24;
    }
    // 443D27重读mode；克隆、旧processed释放、局部减持均可产生回调。
    switch (Mode(body))
    {
    case 3:
        Apply(body, ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(body.mainAt18),
            original, services.filters);
        break;
    case 6:
        CopyMain(body, original, services.filters);
        LoadTemporary(body, original, services);
        break;
    case 11:
    {
        const auto flags = FirstArgument(body.filterAt40.argumentsAt08.begin);
        RECT mirrored = affected;
        if (flags & 1u)
        {
            mirrored.right = Subtract(body.boundsAt24.right, affected.left);
            mirrored.left = Subtract(body.boundsAt24.right, affected.right);
        }
        if (flags & 2u)
        {
            mirrored.bottom = Subtract(body.boundsAt24.bottom, affected.top);
            mirrored.top = Subtract(body.boundsAt24.bottom, affected.bottom);
        }
        const auto source = Surface(body.mainAt18).handleAt04;
        const auto target = Surface(body.processedAt64).handleAt04;
        services.filters.SurfaceCopyBlitRaw(target, source, mirrored.left, mirrored.top,
            nullptr, &affected, &mirrored, 0);
        Apply(body, nullptr, &mirrored, services.filters);
        affected = mirrored;
        break;
    }
    default:
        CopyMain(body, original, services.filters);
        Apply(body, nullptr, original, services.filters);
        break;
    }
}
}

void RefreshPatternProcessedNativeAt443B00(Body& body, PatternLayerNativeRenderServices& services)
{
    GraphicsSurfaceNativeScope scope(services.surfaces);
    auto* dirty = &Dirty(body);
    dirty->vptrAt00->coalesceAt04(dirty);
    dirty = &Dirty(body);
    const auto count = dirty->vptrAt00->countAt18(dirty);
    if (count)
    {
        if (body.pendingAt60)
        {
            ReleaseYtlReference(body.processedAt64);
            body.pendingAt60 = 0;
        }
        dirty = &Dirty(body);
        const auto* const rects = dirty->vptrAt00->rectsAt1C(dirty);
        for (std::int32_t index = 0; index < std::bit_cast<std::int32_t>(count); ++index)
        {
            RECT affected;
            ::CopyRect(&affected, &rects[index]);
            if (body.layer.enabledAt0C)
            {
                RefreshPatternMainNativeAt444230(body, &rects[index], services.drawing);
                if (PixelMode(body)) ProcessRect(body, affected, &rects[index], services);
            }
            ::IntersectRect(&affected, &affected, &body.boundsAt24);
            auto* const parent = body.layer.parentAt10;
            // 444198空交集也提交，让当前parent虚槽自行处理；不加IsRectEmpty早退。
            parent->vptrAt00->addDirtyAt20(parent, &affected);
        }
        dirty = &Dirty(body);
        dirty->vptrAt00->clearAt00(dirty);
    }
    if (!PixelMode(body)) ReleaseYtlReference(body.processedAt64);
}
}
