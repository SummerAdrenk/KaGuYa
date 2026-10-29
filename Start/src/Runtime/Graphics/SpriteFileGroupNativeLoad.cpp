#include <Runtime/SpriteFileGroupNativeLoad.h>
#include <Runtime/AnimationFormatNativePictures.h>
#include <Runtime/PictureLayerFormatNative.h>
#include <Runtime/ArchiveReaderFactoryRuntime.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/PatternLayerNative.h>
#include <Runtime/SpriteAnimationNativeExecution.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
using Format = AnimationFormatNativeBody44h;
std::int32_t Signed(std::uint32_t value) noexcept { return std::bit_cast<std::int32_t>(value); }
std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return Signed(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); }
std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{ return Signed(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b)); }
std::int32_t Count(const Slot& slot) noexcept
{ return slot.controlAt08 ? Signed(static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10) : 0; }
template<class T> T* Element(const Slot& slot, std::uint32_t index = 0) noexcept
{
    if (!slot.baseAt00) return nullptr;
    const auto stride = slot.controlAt08 ?
        static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C : 0u;
    return reinterpret_cast<T*>(slot.baseAt00 + (slot.biasAt04 + index) * stride);
}
Format* CapturedFormat(std::uintptr_t base, YtlReferenceControlRuntime* control, std::uint32_t bias) noexcept
{
    if (!base) return nullptr;
    const auto stride = control ? static_cast<EnvironmentObjectReferenceControl*>(control)->strideAt0C : 0u;
    return reinterpret_cast<Format*>(base + stride * bias);
}
void Share(Slot& target, const Slot& source)
{
    if (&target == &source) return;
    ReleaseYtlReference(target);
    target.baseAt00 = source.baseAt00;
    target.biasAt04 = source.biasAt04;
    target.controlAt08 = source.controlAt08;
    if (target.controlAt08) ++target.controlAt08->referenceCountAt04;
}
void ReleaseCached(YtlReferenceControlRuntime* control) noexcept
{
    if (!control || --control->referenceCountAt04) return;
    if (control->owningAt08) control->DestroyPayloadAtVslot00();
    control->DeleteControlAtVslot04(1);
}
struct RefCleanup
{
    Slot& slot;
    bool active = true;
    ~RefCleanup() noexcept { if (active) ReleaseYtlReference(slot); }
    void CachedNormal() noexcept
    {
        auto* const control = slot.controlAt08;
        active = false;
        ReleaseCached(control);
    }
    void CurrentNormal() noexcept { active = false; ReleaseYtlReference(slot); }
};
void AppendSurfaceAt499920(GuiControlSurfaceReferences& array, const Slot& source)
{
    Slot local{};
    RefCleanup cleanup{local};
    // 499972先增持局部Ref，扩容回调不能改变这次追加的source；非直接Append(source)。
    Share(local, source);
    AppendGuiControlSurfaceReference(array, local);
    cleanup.CachedNormal();
}
std::int32_t NameCount(const EnvironmentVectorStorage<RuntimeCString>& names) noexcept
{
    return Signed(reinterpret_cast<std::uintptr_t>(names.end) -
        reinterpret_cast<std::uintptr_t>(names.begin)) >> 2;
}
void AssignName(RuntimeCString& target, const char* text)
{
    int length = 0;
    if (text) while (text[length]) ++length;
    SetRuntimeCStringAt437680(target, text, length);
}
RECT SpriteRect(const SpriteNativeBodyA0h& sprite) noexcept
{
    return {sprite.xAt2C, sprite.yAt30, Add(sprite.xAt2C, sprite.widthAt24),
        Add(sprite.yAt30, sprite.heightAt28)};
}
void ShiftOldAuxiliary(SpriteNativeBodyA0h& sprite, const RECT& combined)
{
    if (combined.left == sprite.xAt2C && combined.top == sprite.yAt30) return;
    for (auto* slot = sprite.auxiliarySurfacesAt94.begin; slot != sprite.auxiliarySurfacesAt94.end; ++slot)
        for (std::int32_t i = 0; i < Count(*slot); ++i)
        {
            auto* const before = Element<GraphicsSurfaceNativeBody>(*slot, static_cast<std::uint32_t>(i));
            const auto x = Add(before->originXAt10, Sub(sprite.xAt2C, combined.left));
            const auto y = Add(before->originYAt14, Sub(sprite.yAt30, combined.top));
            auto* const after = Element<GraphicsSurfaceNativeBody>(*slot, static_cast<std::uint32_t>(i));
            after->originXAt10 = x;
            after->originYAt14 = y;
        }
}
void CommitBounds(SpriteNativeBodyA0h& sprite, const RECT& combined) noexcept
{
    sprite.xAt2C = combined.left;
    sprite.widthAt24 = Sub(combined.right, combined.left);
    sprite.heightAt28 = Sub(combined.bottom, combined.top);
    sprite.yAt30 = combined.top;
}
void CommitAuxiliary(SpriteNativeBodyA0h& sprite, std::int32_t index, const Slot& source)
{
    if (index < NameCount(sprite.memberNamesAt18))
    {
        auto* const target = reinterpret_cast<Slot*>(reinterpret_cast<std::uintptr_t>(
            sprite.auxiliarySurfacesAt94.begin) + static_cast<std::uint32_t>(index) * 12u);
        CopyAssignGraphicsSurfaceReferenceAt465870(*target, source);
    }
    else AppendSurfaceAt499920(sprite.auxiliarySurfacesAt94, source);
}
void CommitMemberName(SpriteNativeBodyA0h& sprite, std::int32_t index,
    const ArchiveReaderReference& reader, IRuntimeStringManagerSource& strings)
{
    auto& names = sprite.memberNamesAt18;
    auto* const begin = names.begin;
    if (index < NameCount(names))
    {
        // 4B103D固定begin，Name回调之后不可改用新的vector.begin。
        auto* const target = reinterpret_cast<RuntimeCString*>(reinterpret_cast<std::uintptr_t>(begin) +
            static_cast<std::uint32_t>(index) * 4u);
        const char* const name = reader.Get()->NameAt4179F0();
        AssignName(*target, name);
    }
    else
    {
        union Name { Name() noexcept {} ~Name() noexcept {} RuntimeCString value; } temporary;
        ConstructArchiveResourceNameAt40B530(&temporary.value, reader.Get()->NameAt4179F0(), strings);
        struct Cleanup
        {
            RuntimeCString& value;
            bool active = true;
            ~Cleanup() noexcept { if (active) DestroyRuntimeCStringAt40CD40(&value); }
        } cleanup{temporary.value};
        AppendRuntimeCStringAt41D5E9(names, temporary.value);
        // 4B10B2未提前撤state7；正常Free若抛，仍按EH销毁当前临时槽。
        DestroyRuntimeCStringAt40CD40(&temporary.value);
        cleanup.active = false;
    }
}
}

bool LoadSpriteFileGroupNativeAt4B0840(SpriteNativeBodyA0h& sprite, std::int32_t member,
    const ArchiveReaderReference& reader, std::int32_t playFromStart,
    IAnimationFormatNativePictures& pictures, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings, ArchiveCodecInputMode* inputMode)
{
    GraphicsSurfaceNativeScope scope(surfaces);
    auto* const allocated = NewAnimationFormatNativeAt47EECB(strings);
    Slot local;
    ConstructAnimationFormatReferenceAt4987F0(local, allocated, 1);
    RefCleanup cleanup{local};
    if (!member)
    {
        // 4B08C0先解reader，再固定格式base/control，Bytes回调后仍使用已固定receiver。
        auto* const source = reader.Get();
        auto base = local.baseAt00;
        auto* const control = local.controlAt08;
        auto* const format = CapturedFormat(base, control, local.biasAt04);
        auto* const bytes = source->BytesAt4172C0();
        std::int32_t status;
        {
            // 4B090C：沿用Bytes实际返回的Ref，不重取reader，也不提前读count。
            AnimationFormatNativeRawInputScope inputScope(inputMode);
            status = LoadAnimationFormatNativeAt448010(*format, bytes->RawSlot(), pictures);
        }
        if (!status)
        {
            sprite.animationAt34.repeatAt10 = CapturedFormat(base, control, local.biasAt04)->repeatAt18;
            sprite.animationAt34.mappingCountAt14 = CapturedFormat(base, control, local.biasAt04)->mappingCountAt1C;
            Share(sprite.animationAt34.commandsAt3C, CapturedFormat(base, control, local.biasAt04)->commandsAt20);
            Share(sprite.animationAt34.branchesAt48, CapturedFormat(base, control, local.biasAt04)->branchesAt2C);
            if (Count(CapturedFormat(base, control, local.biasAt04)->surfacesAt38) > 0)
            {
                ClearSpriteNameVectorAt43F240(sprite.memberNamesAt18);
                ReleaseGuiControlSurfaceRangeAt43EA10(sprite.auxiliarySurfacesAt94.begin, sprite.auxiliarySurfacesAt94.end);
                base = local.baseAt00;
                sprite.auxiliarySurfacesAt94.end = sprite.auxiliarySurfacesAt94.begin;
                auto* current = CapturedFormat(base, control, local.biasAt04);
                const auto y = current->yAt0C;
                const auto x = current->xAt08;
                sprite.xAt2C = x;
                sprite.yAt30 = y;
                current = CapturedFormat(base, control, local.biasAt04);
                const auto height = current->heightAt14;
                const auto width = current->widthAt10;
                sprite.widthAt24 = width;
                sprite.heightAt28 = height;
                CopyAssignGraphicsSurfaceReferenceAt465870(sprite.primarySurfacesAt88,
                    CapturedFormat(base, control, local.biasAt04)->surfacesAt38);
            }
            // 原新AN无帧仍成功并初始化；不清旧Surface/aux/画布。
            sprite.modeAt04 = 2;
            static_cast<void>(InitializeSpriteAnimationNativeAt47E980(sprite.animationAt34, playFromStart));
        }
        else if (status == 3)
        {
            AdoptGraphicsSurfaceAt407A80(sprite.primarySurfacesAt88, NewGraphicsSurfaceAt40C2E0(surfaces, strings));
            if (!LoadGraphicsSurfaceArchiveReaderAt419D70(*ResolveSpriteSurfaceNativeAt443580(sprite, 0),
                reader, surfaces, 0)) return false;
            auto* current = ResolveSpriteSurfaceNativeAt443580(sprite, 0);
            sprite.widthAt24 = current->widthAt08;
            sprite.heightAt28 = current->heightAt0C;
            current = ResolveSpriteSurfaceNativeAt443580(sprite, 0);
            sprite.xAt2C = current->originXAt10;
            sprite.yAt30 = current->originYAt14;
            sprite.modeAt04 = ResolveSpriteSurfaceNativeAt443580(sprite, 0)->alphaAt18 ? 1u : 0u;
        }
        else return false;
        AssignName(sprite.primaryNameAt14, reader.Get()->NameAt4179F0());
    }
    else
    {
        if (sprite.modeAt04 != 2) { cleanup.CachedNormal(); return false; }
        auto* const source = reader.Get();
        auto base = local.baseAt00;
        auto* control = local.controlAt08;
        auto* const format = CapturedFormat(base, control, local.biasAt04);
        auto* const bytes = source->BytesAt4172C0();
        std::int32_t status;
        {
            // 4B0C2B：PL同样显式选Raw，不改变格式catch或AL分支。
            AnimationFormatNativeRawInputScope inputScope(inputMode);
            status = LoadPictureLayerFormatNativeAt4482A0(*format, bytes->RawSlot(), pictures);
        }
        const auto index = Sub(member, 1);
        if (!status)
        {
            RECT combined = SpriteRect(sprite);
            // 4B0C50仅重读base；incoming与首轮count仍用Bytes前固定的control，stride晚读。
            base = local.baseAt00;
            auto* current = CapturedFormat(base, control, local.biasAt04);
            RECT incoming{current->xAt08, current->yAt0C, Add(current->xAt08, current->widthAt10),
                Add(current->yAt0C, current->heightAt14)};
            ::UnionRect(&combined, &combined, &incoming);
            if (combined.left != incoming.left || combined.top != incoming.top)
                for (std::int32_t i = 0;
                     i < Count(CapturedFormat(base, control, local.biasAt04)->surfacesAt38); ++i)
                {
                    // 4B0D21才重新解析完整Ref；不得将循环正文也固定到旧control。
                    current = ResolveAnimationFormatReference(local);
                    auto* const before = Element<GraphicsSurfaceNativeBody>(current->surfacesAt38, static_cast<std::uint32_t>(i));
                    const auto x = Add(before->originXAt10, Sub(current->xAt08, combined.left));
                    const auto y = Add(before->originYAt14, Sub(current->yAt0C, combined.top));
                    auto* const after = Element<GraphicsSurfaceNativeBody>(current->surfacesAt38, static_cast<std::uint32_t>(i));
                    base = local.baseAt00; // 4B0D55在写origin之前刷新。
                    after->originYAt14 = y;
                    after->originXAt10 = x;
                    control = local.controlAt08; // 4B0D62在写入后刷新，供下一轮count。
                }
            ShiftOldAuxiliary(sprite, combined);
            CommitBounds(sprite, combined);
            CommitAuxiliary(sprite, index, ResolveAnimationFormatReference(local)->surfacesAt38);
        }
        else if (status == 3)
        {
            auto* const created = NewGraphicsSurfaceAt40C2E0(surfaces, strings);
            Slot temporary;
            ConstructGraphicsSurfaceReferenceAt451500(temporary, created, 1);
            RefCleanup surfaceCleanup{temporary};
            if (!LoadGraphicsSurfaceArchiveReaderAt419D70(*Element<GraphicsSurfaceNativeBody>(temporary),
                reader, surfaces, 0)) return false;
            RECT combined = SpriteRect(sprite);
            auto* current = Element<GraphicsSurfaceNativeBody>(temporary);
            RECT incoming{current->originXAt10, current->originYAt14,
                Add(current->originXAt10, current->widthAt08), Add(current->originYAt14, current->heightAt0C)};
            ::UnionRect(&combined, &combined, &incoming);
            current = Element<GraphicsSurfaceNativeBody>(temporary);
            current->originXAt10 = Sub(incoming.left, combined.left);
            current->originYAt14 = Sub(incoming.top, combined.top);
            ShiftOldAuxiliary(sprite, combined);
            CommitBounds(sprite, combined);
            CommitAuxiliary(sprite, index, temporary);
            surfaceCleanup.CurrentNormal();
        }
        else return false;
        CommitMemberName(sprite, index, reader, strings);
    }
    cleanup.CachedNormal();
    return true;
}
}
