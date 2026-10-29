#include <Runtime/PatternLayerNativeResourceTarget.h>
#include <Runtime/AnimationFormatNativePictures.h>
#include <Runtime/GuiDirtyRectNative.h>
#include <Runtime/SpriteArchiveResourceNativeLoader.h>
#include <Runtime/SpriteNativeResourceState.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
SpriteNativeBodyA0h& Sprite(PatternSpriteReceiver receiver) noexcept
{ return *reinterpret_cast<SpriteNativeBodyA0h*>(receiver); }

class NativeLoadAction final : public IPatternResourceLoadAction
{
public:
    NativeLoadAction(PatternLayerNativeResourceTarget& target, std::uint32_t slot,
        PatternSpriteReceiver receiver, IPatternResourceLoadCommit& commit,
        PatternLayerNativeResourceServices& services, IRuntimeStringManagerSource& strings) noexcept
        : target_(target), slot_(slot), receiver_(receiver), commit_(commit), services_(services), strings_(strings) {}

    void CaptureCgDirectOwnerAfterRead() override {}
    bool ResolveDirectTargetAfterInitialization() override
    {
        // 4CCBE3先读当前control.count；只有Direct有此无符号gate。
        if (slot_ >= target_.SpriteSlotCount()) return false;
        receiver_ = target_.CaptureSpriteReceiver(slot_);
        return true;
    }
    bool LoadFromReader(const ArchiveReaderReference& reader, std::uint8_t initialization,
        const PatternResourceLoadRequest& request, const ArchiveCodecServices& codecs) override
    {
        AnimationFormatNativePictureServices pictures{services_.surfaces, strings_,
            services_.graphics, services_.win32, codecs, {}};
        // bridge在原AN/PL wrapper调用区间显式选Raw；不额外读Ref元数据或调用reader getter。
        return LoadPatternSpriteFromReaderNative(Sprite(receiver_), reader, initialization, request, pictures);
    }
    void CommitWhileFactoryAlive() override { commit_.Commit(); }
private:
    PatternLayerNativeResourceTarget& target_;
    std::uint32_t slot_;
    PatternSpriteReceiver receiver_;
    IPatternResourceLoadCommit& commit_;
    PatternLayerNativeResourceServices& services_;
    IRuntimeStringManagerSource& strings_;
};
}

void InvalidatePatternLayerNativeAt443650(PatternLayerNativeBody80h& body, const RECT* rect)
{
    RECT whole;
    if (!rect)
    {
        const auto* const surface = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(body.mainAt18);
        whole = {0, 0, surface->widthAt08, surface->heightAt0C};
        rect = &whole;
    }
    auto* const dirty = ResolveGuiDirtyRectReference(body.dirtyAt70);
    dirty->vptrAt00->addRectAt08(dirty, rect);
}

PatternLayerNativeResourceTarget::PatternLayerNativeResourceTarget(PatternLayerNativeBody80h& body,
    PatternLayerNativeResourceServices& services) noexcept : body_(body), services_(services) {}

std::size_t PatternLayerNativeResourceTarget::SpriteSlotCount() const noexcept
{
    const auto* const control = static_cast<const EnvironmentObjectReferenceControl*>(body_.spritesAt34.controlAt08);
    return control ? control->countAt10 : 0u;
}
std::int32_t PatternLayerNativeResourceTarget::LayerX() const noexcept
{ return std::bit_cast<std::int32_t>(body_.layer.valueAt04); }
std::int32_t PatternLayerNativeResourceTarget::LayerY() const noexcept
{ return std::bit_cast<std::int32_t>(body_.layer.valueAt08); }
PatternSpriteReceiver PatternLayerNativeResourceTarget::CaptureSpriteReceiver(std::uint32_t slot) const
{
    // Exc/Convert/Group和Direct提交重解析都无count门控；不得复用Direct装载gate。
    const auto& source = body_.spritesAt34;
    if (!source.baseAt00) return 0;
    const auto stride = source.controlAt08 ?
        static_cast<const EnvironmentObjectReferenceControl*>(source.controlAt08)->strideAt0C : 0u;
    return source.baseAt00 + (source.biasAt04 + slot) * stride;
}
const char* PatternLayerNativeResourceTarget::LogicalNameCharacters(std::size_t slot) const
{ return Sprite(CaptureSpriteReceiver(static_cast<std::uint32_t>(slot))).logicalNameAt0C.GetString(); }
std::int32_t PatternLayerNativeResourceTarget::LogicalNameLength(std::size_t slot) const
{ return Sprite(CaptureSpriteReceiver(static_cast<std::uint32_t>(slot))).logicalNameAt0C.GetLength(); }
PatternSpriteRectResult PatternLayerNativeResourceTarget::LocalDrawableRect(std::size_t slot) const
{ return ReceiverDrawableRect(CaptureSpriteReceiver(static_cast<std::uint32_t>(slot))); }
void PatternLayerNativeResourceTarget::ClearSlot(std::size_t slot)
{
    GraphicsSurfaceNativeScope scope(services_.surfaces);
    static_cast<void>(ClearPatternSpritesNativeAt442700(body_,
        std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(slot))));
}
void PatternLayerNativeResourceTarget::LoadResource(std::uint32_t slot, PatternSpriteReceiver receiver,
    const PatternResourceLoadRequest& request, const IPatternResourceFailureName& failureName,
    IPatternResourceLoadCommit& commit, IRuntimeStringManagerSource& strings)
{
    NativeLoadAction action(*this, slot, receiver, commit, services_, strings);
    const auto& base = services_.loads;
    const PatternResourceLoadServices current{base.source, base.transaction, base.files, base.memory,
        base.key, base.initialization, base.codecs, strings};
    ExecutePatternResourceLoad(request, failureName, action, current);
}
const char* PatternLayerNativeResourceTarget::ReceiverPrimaryNameCharacters(PatternSpriteReceiver receiver) const
{ return Sprite(receiver).primaryNameAt14.GetString(); }
PatternResourceNamesBorrow PatternLayerNativeResourceTarget::ReceiverAuxiliaryNames(PatternSpriteReceiver receiver) const
{ return PatternResourceNamesBorrow(Sprite(receiver).memberNamesAt18); }
void PatternLayerNativeResourceTarget::PrepareReceiverFileGroup(PatternSpriteReceiver receiver, FileGroupChange changes)
{
    // Prepare早于loader的scope，旧Surface在此即可被释放。
    GraphicsSurfaceNativeScope scope(services_.surfaces);
    PrepareSpriteFileGroupNativeAt4CBD71(Sprite(receiver), changes);
}
void PatternLayerNativeResourceTarget::SetReceiverLogicalName(PatternSpriteReceiver receiver, const RuntimeCString& source)
{ AssignRuntimeCStringAt410220(Sprite(receiver).logicalNameAt0C, source); }
void PatternLayerNativeResourceTarget::SetReceiverOrigin(PatternSpriteReceiver receiver, std::int32_t x, std::int32_t y)
{
    auto& sprite = Sprite(receiver);
    sprite.xAt2C = x;
    sprite.yAt30 = y;
}
void PatternLayerNativeResourceTarget::SetReceiverActive(PatternSpriteReceiver receiver, bool active)
{ Sprite(receiver).activeAt08 = static_cast<std::uint8_t>(active); }
PatternSpriteRectResult PatternLayerNativeResourceTarget::ReceiverDrawableRect(PatternSpriteReceiver receiver) const
{ return ReadSpriteDrawableRectNativeAt4AD080(Sprite(receiver)); }
void PatternLayerNativeResourceTarget::Invalidate(const RECT& rect)
{ InvalidatePatternLayerNativeAt443650(body_, &rect); }
}
