#include <Runtime/PatternLayerNative.h>
#include <Runtime/SpriteAnimationNativeExecution.h>
#include <Runtime/GuiDirtyRectNative.h>

#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = PatternLayerNativeBody80h;
std::uint32_t Count(const Body& body) noexcept
{
    return body.spritesAt34.controlAt08 ?
        static_cast<EnvironmentObjectReferenceControl*>(body.spritesAt34.controlAt08)->countAt10 : 0;
}
SpriteNativeBodyA0h& Sprite(Body& body, std::uint32_t index)
{
    auto& slot = body.spritesAt34;
    if (!slot.baseAt00) return *static_cast<SpriteNativeBodyA0h*>(nullptr);
    const auto stride = slot.controlAt08 ?
        static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C : 0;
    return *reinterpret_cast<SpriteNativeBodyA0h*>(slot.baseAt00 + (slot.biasAt04 + index) * stride);
}
GuiDirtyRectNativeBody80h& Dirty(Body& body)
{
    auto* const dirty = ResolveGuiDirtyRectReference(body.dirtyAt70);
    return *dirty;
}
RECT Bounds(const SpriteNativeBodyA0h& sprite)
{
    RECT result{};
    // 442437/44251E是无符号JA；未知高位模式也没有包围矩形。
    if (sprite.logicalNameAt0C.GetLength() && sprite.modeAt04 <= 2)
    {
        result = {0, 0, sprite.widthAt24, sprite.heightAt28};
        ::OffsetRect(&result, sprite.xAt2C, sprite.yAt30);
    }
    return result;
}
void AddDirty(Body& body, RECT& rect)
{
    ::OffsetRect(&rect, std::bit_cast<LONG>(body.layer.valueAt04), std::bit_cast<LONG>(body.layer.valueAt08));
    auto& dirty = Dirty(body);
    dirty.vptrAt00->addRectAt08(&dirty, &rect);
}
}
std::int32_t ClearPatternSpritesNativeAt442700(Body& body, std::int32_t index)
{
    if (index >= std::bit_cast<std::int32_t>(Count(body))) return 1;
    if (index < 0)
    {
        // 442795..442899是43F140的内联路径，包含相同的局部terminate保护。
        for (std::uint32_t i = 0; i < Count(body); ++i) ClearSpriteNativeAt43F140(Sprite(body, i));
        body.filterAt40.modeAt04 = 0;
        body.filterAt40.argumentsAt08.end = body.filterAt40.argumentsAt08.begin;
    }
    else
        ClearSpriteNativeAt43F140(Sprite(body, static_cast<std::uint32_t>(index)));
    return 0;
}
void ResetPatternLayerNativeAt47EC71(Body& body, IGraphicsWindowSurfaceApi& drawing)
{
    body.layer.valueAt04 = 0;
    body.layer.valueAt08 = 0;
    body.itemIndexAt14 = -1;
    body.hasItemAt7C = 0;
    body.resultAt7D = 0;
    (void)ClearPatternSpritesNativeAt442700(body, -1);
    auto* const main = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(body.mainAt18);
    if (const auto handle = main->handleAt04)
        drawing.SurfaceClearRaw(handle, main->clearColorAt20, nullptr);
    auto& dirty = Dirty(body);
    dirty.vptrAt00->clearAt00(&dirty);
    body.filterAt40.modeAt04 = 0;
    body.filterAt40.argumentsAt08.end = body.filterAt40.argumentsAt08.begin;
    body.pendingAt60 = 0;
    ReleaseYtlReference(body.processedAt64);
    body.layer.enabledAt0C = 0;
}
void AdvancePatternLayerNativeAt4423C0(Body& body, PatternLayerNativeRenderServices& services)
{
    if (!body.layer.enabledAt0C) return;
    for (std::uint32_t i = 0; i < Count(body); ++i)
    {
        auto before = Bounds(Sprite(body, i));
        auto& sprite = Sprite(body, i);
        if (!sprite.logicalNameAt0C.GetLength() || sprite.modeAt04 != 2) continue;
        if (AdvanceSpriteAnimationNativeAt443740(sprite.animationAt34) != 0) continue;
        AddDirty(body, before);
        // dirty虚槽可能替换Sprite数组，4424EB必须重新解当前Ref，不能沿用旧receiver。
        auto after = Bounds(Sprite(body, i));
        AddDirty(body, after);
    }
    RefreshPatternProcessedNativeAt443B00(body, services);
}

RECT* ReadPatternBoundsNativeAt4D3490(Body& body, RECT* out)
{
    *out = {};
    if (Count(body))
    {
        std::uint32_t index = 0;
        do
        {
            const auto rect = Bounds(Sprite(body, index));
            ::UnionRect(out, out, &rect);
            ++index;
        } while (index < Count(body));
    }
    if (out->left || out->right || out->top || out->bottom)
    {
        const auto y = std::bit_cast<LONG>(body.layer.valueAt08);
        const auto x = std::bit_cast<LONG>(body.layer.valueAt04);
        ::OffsetRect(out, x, y);
    }
    return out;
}

void ApplyPatternAnimationCommandsNativeAt4CAF10(Body& body)
{
    if (body.filterAt40.modeAt04 != 10u) return;
    auto end = reinterpret_cast<std::uintptr_t>(body.filterAt40.argumentsAt08.end);
    auto begin = reinterpret_cast<std::uintptr_t>(body.filterAt40.argumentsAt08.begin);
    // 4CAF30/35用TEST清低两位再作signed门控，不要求参数成对或索引在count内。
    if (std::bit_cast<std::int32_t>((end - begin) & 0xFFFFFFFCu) <= 0) return;
    std::uint32_t index = 0;
    do
    {
        const auto argument = *reinterpret_cast<const std::int32_t*>(begin + index * 4u + 4u);
        const auto slot = *reinterpret_cast<const std::uint32_t*>(begin + index * 4u);
        auto& sprite = Sprite(body, slot);
        auto& animation = sprite.animationAt34;
        if (!argument)
        {
            // 0命令只测mode；初始化的false结果原样忽略，不能强制检查logical或active。
            if (sprite.modeAt04 == 2u)
                static_cast<void>(InitializeSpriteAnimationNativeAt47E980(animation, 1));
        }
        else
        {
            if (!sprite.logicalNameAt0C.GetLength() || sprite.modeAt04 != 2u) throw 10;
            bool finalBranch = false;
            if (argument < 0)
            {
                const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(animation.commandsAt3C.controlAt08);
                const auto count = control ? control->countAt10 : 0u;
                if (count)
                {
                    auto last = animation.commandsAt3C;
                    last.biasAt04 += count - 1u;
                    const auto* command = ResolveEnvironmentObjectReference<SpriteCommandNativeBody10h>(last);
                    finalBranch = command->opcodeAt00 == 4;
                }
            }
            if (finalBranch)
            {
                AdvanceSpriteAnimationToFinalNativeAt4C1D20(animation);
                animation.flagsAt00 |= 2u;
                CopySpriteFinalMappingsNativeAt4C23A0(animation);
            }
            else
                AdvanceSpriteAnimationSegmentsNativeAt4D5790(animation, argument);
        }
        // 初始化可能分配/释放并换表；4CAFF6重新读end/begin，保留DWORD回绕与SAR。
        end = reinterpret_cast<std::uintptr_t>(body.filterAt40.argumentsAt08.end);
        begin = reinterpret_cast<std::uintptr_t>(body.filterAt40.argumentsAt08.begin);
        index += 2u;
    } while (std::bit_cast<std::int32_t>(index) < std::bit_cast<std::int32_t>(end - begin) >> 2);
}
}
