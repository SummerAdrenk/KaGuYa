#include "CgDisplayWindowNative.Internal.h"
#include <Runtime/LayerListNative.h>

namespace KaGuYa::Start::Runtime
{
std::uint32_t CgDisplayWindowSpriteCountAt4ACF90(const CgDisplayWindowNativeBody60h& body) noexcept
{
    const auto* const control = static_cast<const EnvironmentObjectReferenceControl*>(body.spritesAt24.controlAt08);
    return control ? control->countAt10 : 0u;
}
SpriteNativeBodyA0h* ResolveCgDisplayWindowSpriteNative(
    const CgDisplayWindowNativeBody60h& body, std::uint32_t index) noexcept
{
    const auto& slot = body.spritesAt24;
    if (!slot.baseAt00) return nullptr;
    const auto* const control = static_cast<const EnvironmentObjectReferenceControl*>(slot.controlAt08);
    const auto stride = control ? control->strideAt0C : 0u;
    return reinterpret_cast<SpriteNativeBodyA0h*>(slot.baseAt00 + (slot.biasAt04 + index) * stride);
}
RECT* ReadCgDisplayWindowBoundsNativeAt479DB0(CgDisplayWindowNativeBody60h& body, RECT* out) noexcept
{ *out = body.window.boundsAt14; return out; }
std::uint8_t QueryCgDisplayWindowNativeAt4C1240(std::uint32_t, std::uint32_t) noexcept
{ return 0; }
void ResetCgDisplayWindowNativeAt4AF1E0(CgDisplayWindowNativeBody60h& body) noexcept
{ body.window.layer.enabledAt0C = 0; }
namespace CgDisplayWindowNativeDetail
{
RECT SpriteBounds(const SpriteNativeBodyA0h& sprite)
{
    RECT result{};
    // 原JA：空逻辑名、负模式及unsigned mode>2均贡献空矩形；不检查active。
    if (sprite.logicalNameAt0C.GetLength() && sprite.modeAt04 <= 2u)
    {
        result = {0, 0, sprite.widthAt24, sprite.heightAt28};
        ::OffsetRect(&result, sprite.xAt2C, sprite.yAt30);
    }
    return result;
}
void AddParentDirty(Body& body, const RECT* rect)
{
    auto* const parent = body.window.layer.parentAt10;
    auto* const dirty = parent->vptrAt00->dirtyAt14(parent);
    dirty->vptrAt00->addRectAt08(dirty, rect);
}
}
}
