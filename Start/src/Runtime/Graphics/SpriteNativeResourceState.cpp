#include <Runtime/SpriteNativeResourceState.h>

namespace KaGuYa::Start::Runtime
{
bool IsSpriteLogicalNameEmptyNativeAt4CB9E9(const SpriteNativeBodyA0h& sprite) noexcept
{
    return sprite.logicalNameAt0C.GetLength() == 0;
}

PatternSpriteRectResult ReadSpriteDrawableRectNativeAt4AD080(const SpriteNativeBodyA0h& sprite) noexcept
{
    // 资源target使用独立局部RECT；不承诺任意out别名Sprite的原始ABI调用域。
    PatternSpriteRectResult result{};
    if (IsSpriteLogicalNameEmptyNativeAt4CB9E9(sprite)) return result;
    if (sprite.modeAt04 > 2u)
    {
        result.state = PatternSpriteRectState::InvalidLoadMode;
        return result;
    }
    result.rect = {0, 0, sprite.widthAt24, sprite.heightAt28};
    ::OffsetRect(&result.rect, sprite.xAt2C, sprite.yAt30);
    result.state = PatternSpriteRectState::Valid;
    return result;
}

void PrepareSpriteFileGroupNativeAt4CBD71(SpriteNativeBodyA0h& sprite, FileGroupChange changes)
{
    const auto mask = static_cast<std::uint8_t>(changes);
    sprite.activeAt08 = 0;
    SetRuntimeCStringAt437680(sprite.logicalNameAt0C, "", 0);
    if (mask & 1u)
    {
        sprite.modeAt04 = 0;
        SetRuntimeCStringAt437680(sprite.primaryNameAt14, "", 0);
        ReleaseYtlReference(sprite.primarySurfacesAt88);
        ClearSpriteAnimationAt43EF10(sprite.animationAt34);
    }
    if (mask & 6u)
    {
        sprite.widthAt24 = 0;
        sprite.heightAt28 = 0;
        sprite.xAt2C = 0;
        sprite.yAt30 = 0;
        if (mask & 2u)
        {
            ClearSpriteNameVectorAt43F240(sprite.memberNamesAt18);
            // 4CBDF7先选end，再选begin；退域回调可改向量，成功尾必须重读begin。
            auto& auxiliary = sprite.auxiliarySurfacesAt94;
            auto* const end = auxiliary.end;
            auto* const begin = auxiliary.begin;
            ReleaseGuiControlSurfaceRangeAt43EA10(begin, end);
            auxiliary.end = auxiliary.begin;
        }
    }
}
}
