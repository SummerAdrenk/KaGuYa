#include <Runtime/SpriteResourceNativeLoad.h>
#include <Runtime/AnimationFormatNativePictures.h>
#include <Runtime/ArchiveReaderFactoryRuntime.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/PatternLayerNative.h>
#include <Runtime/SpriteAnimationNativeExecution.h>

#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
void ShareAfterRelease(YtlReferenceSlotRuntime& target, const YtlReferenceSlotRuntime& source)
{
    if (&target == &source) return;
    ReleaseYtlReference(target);
    // 47EFDC/47F038：先完成旧释放，再逐字段重读源，不在释放前保存一个Ref快照。
    target.baseAt00 = source.baseAt00;
    target.biasAt04 = source.biasAt04;
    target.controlAt08 = source.controlAt08;
    if (target.controlAt08) ++target.controlAt08->referenceCountAt04;
}
void ReleaseCachedFormatControl(YtlReferenceControlRuntime* control) noexcept
{
    // 47F063正常尾为state2终止边界；payload回调后仍使用缓存ESI，而非重读局部Ref。
    if (!control || --control->referenceCountAt04 != 0) return;
    if (control->owningAt08) control->DestroyPayloadAtVslot00();
    control->DeleteControlAtVslot04(1);
}
}

std::int32_t LoadSpriteAnimationNativeAt47EEA0(SpriteNativeBodyA0h& sprite,
    const YtlReferenceSlotRuntime& bytes, IAnimationFormatNativePictures& pictures,
    IGraphicsSurfaceApi& surfaces, IRuntimeStringManagerSource& strings,
    ArchiveCodecInputMode* inputMode)
{
    GraphicsSurfaceNativeScope surfaceScope(surfaces);
    auto* const format = NewAnimationFormatNativeAt47EECB(strings);
    YtlReferenceSlotRuntime local;
    ConstructAnimationFormatReferenceAt47EF04(local, format);
    auto* const normalControl = local.controlAt08;
    struct Cleanup final
    {
        YtlReferenceSlotRuntime& reference;
        bool active = true;
        ~Cleanup() noexcept { if (active) ReleaseYtlReference(reference); }
    } cleanup{local};
    std::int32_t status;
    {
        // 47EF5C：格式正文/Ref已经完成；bridge显式选Raw，wrapper返回即恢复模式。
        AnimationFormatNativeRawInputScope inputScope(inputMode);
        status = LoadAnimationFormatNativeAt448010(*format, bytes, pictures);
    }
    std::int32_t result = status == 3 ? 1 : 2;
    if (!status)
    {
        const auto y = format->yAt0C;
        const auto x = format->xAt08;
        sprite.xAt2C = x;
        sprite.yAt30 = y;
        const auto width = format->widthAt10;
        const auto height = format->heightAt14;
        sprite.widthAt24 = width;
        sprite.heightAt28 = height;
        sprite.animationAt34.repeatAt10 = format->repeatAt18;
        sprite.animationAt34.mappingCountAt14 = format->mappingCountAt1C;
        ShareAfterRelease(sprite.animationAt34.commandsAt3C, format->commandsAt20);
        ShareAfterRelease(sprite.animationAt34.branchesAt48, format->branchesAt2C);
        CopyAssignGraphicsSurfaceReferenceAt465870(sprite.primarySurfacesAt88, format->surfacesAt38);
        result = 0;
    }
    cleanup.active = false;
    ReleaseCachedFormatControl(normalControl);
    return result;
}

bool LoadSpriteArchiveNativeAt480650(SpriteNativeBodyA0h& sprite,
    const ArchiveReaderReference& reader, std::int32_t playFromStart, std::uint32_t argument,
    IAnimationFormatNativePictures& pictures, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings, ArchiveCodecInputMode* inputMode)
{
    GraphicsSurfaceNativeScope surfaceScope(surfaces);
    const char* const firstName = reader.Get()->NameAt4179F0();
    if (!firstName) throw ATL::CAtlException(E_FAIL); // 480821 -> 413880。
    if (_mbsicmp(reinterpret_cast<const unsigned char*>(sprite.logicalNameAt0C.GetString()),
        reinterpret_cast<const unsigned char*>(firstName)) == 0 && sprite.argumentAt10 == argument)
        return true;

    ClearSpriteNativeAt43F140(sprite);
    auto* const bytes = reader.Get()->BytesAt4172C0();
    const auto status = LoadSpriteAnimationNativeAt47EEA0(
        sprite, bytes->RawSlot(), pictures, surfaces, strings, inputMode);
    if (status == 1)
    {
        // 480709..48073A：裸24h构造完成后才撤保护/交407A80；发布失败不额外回收。
        auto* const created = NewGraphicsSurfaceAt40C2E0(surfaces, strings);
        AdoptGraphicsSurfaceAt407A80(sprite.primarySurfacesAt88, created);
        if (!LoadGraphicsSurfaceArchiveReaderAt419D70(*ResolveSpriteSurfaceNativeAt443580(sprite, 0),
            reader, surfaces, argument)) return false;
        auto* current = ResolveSpriteSurfaceNativeAt443580(sprite, 0);
        sprite.widthAt24 = current->widthAt08;
        sprite.heightAt28 = current->heightAt0C;
        current = ResolveSpriteSurfaceNativeAt443580(sprite, 0);
        sprite.xAt2C = current->originXAt10;
        sprite.yAt30 = current->originYAt14;
        current = ResolveSpriteSurfaceNativeAt443580(sprite, 0);
        sprite.modeAt04 = current->alphaAt18 != 0 ? 1u : 0u;
    }
    else if (!status)
    {
        sprite.modeAt04 = 2;
        static_cast<void>(InitializeSpriteAnimationNativeAt47E980(sprite.animationAt34, playFromStart));
    }
    else return false;

    const char* const finalName = reader.Get()->NameAt4179F0();
    int length = 0;
    if (finalName) while (finalName[length]) ++length;
    SetRuntimeCStringAt437680(sprite.logicalNameAt0C, finalName, length);
    sprite.activeAt08 = 1;
    sprite.argumentAt10 = argument;
    return true;
}
}
