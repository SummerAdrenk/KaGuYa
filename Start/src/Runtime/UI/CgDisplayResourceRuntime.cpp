#include <Runtime/CgDisplayResourceRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
[[nodiscard]] SpriteRuntime& RequireCgReceiver(const PatternSpriteReceiver receiver)
{
    if (!receiver) throw std::logic_error("CG Sprite receiver is null (safe boundary)");
    return *reinterpret_cast<SpriteRuntime*>(receiver);
}

class CgLoadAction final : public IPatternResourceLoadAction
{
public:
    CgLoadAction(const CgDisplayWindowRef& displaySlot, std::uint32_t slot,
        PatternSpriteReceiver receiver, IPatternResourceLoadCommit& commit) noexcept
        : displaySlot_(displaySlot), slot_(slot), receiver_(receiver), commit_(commit) {}

    void CaptureCgDirectOwnerAfterRead() override
    {
        // 4A8D46..73 只借当前真实槽的 owner，不复制 shared_ptr 或提前固定 Sprite。
        cachedOwner_ = dynamic_cast<CgDisplayWindowRuntime*>(displaySlot_.get());
    }
    bool ResolveDirectTargetAfterInitialization() override
    {
        // registry 回调后仍使用上述 owner，但解析它当前的 Sprite 容器。
        if (!cachedOwner_) throw std::logic_error("CG display owner is null (safe boundary)");
        if (slot_ >= cachedOwner_->SpriteSlotCount()) return false;
        receiver_ = reinterpret_cast<PatternSpriteReceiver>(&cachedOwner_->SpriteAt(slot_));
        return true;
    }
    bool LoadFromReader(const ArchiveReaderReference& reader, std::uint8_t initialization,
        const PatternResourceLoadRequest& request, const ArchiveCodecServices& codecs) override
    {
        return LoadPatternSpriteFromReaderModern(
            RequireCgReceiver(receiver_), reader, initialization, request, codecs);
    }
    void CommitWhileFactoryAlive() override { commit_.Commit(); }

private:
    const CgDisplayWindowRef& displaySlot_;
    std::uint32_t slot_;
    PatternSpriteReceiver receiver_;
    IPatternResourceLoadCommit& commit_;
    CgDisplayWindowRuntime* cachedOwner_ = nullptr;
};
}

CgDisplayArchiveSpriteResourceLoader::CgDisplayArchiveSpriteResourceLoader(
    const PatternResourceLoadServices& services) noexcept
    : services_(services)
{
}

void CgDisplayArchiveSpriteResourceLoader::ExecuteLoad(const PatternResourceLoadRequest& request,
    const IPatternResourceFailureName& failureName, IPatternResourceLoadAction& action,
    IRuntimeStringManagerSource& strings)
{
    const PatternResourceLoadServices current{services_.source, services_.transaction,
        services_.files, services_.memory, services_.key, services_.initialization, services_.codecs, strings};
    ExecutePatternResourceLoad(request, failureName, action, current);
}

CgDisplayResourceRuntime::CgDisplayResourceRuntime(
    const CgDisplayWindowRef& displayWindowSlot,
    ICgOperationWindow& operationWindow,
    ICgDisplaySpriteResourceLoader& resourceLoader,
    ICgOperationDialogResourceControl& operationDialog) noexcept
    : displayWindowSlot_(displayWindowSlot),
      operationWindow_(&operationWindow),
      resourceLoader_(&resourceLoader),
      operationDialog_(&operationDialog)
{
}

CgDisplayWindowRuntime& CgDisplayResourceRuntime::RequireCurrentDisplayWindow() const
{
    auto* const window = dynamic_cast<CgDisplayWindowRuntime*>(displayWindowSlot_.get());
    if (!window) throw std::logic_error("CG display owner is null (safe boundary)");
    return *window;
}

SpriteRuntime& CgDisplayResourceRuntime::SpriteAt(const std::size_t slotIndex)
{
    return RequireCurrentDisplayWindow().SpriteAt(slotIndex);
}

const SpriteRuntime& CgDisplayResourceRuntime::SpriteAt(
    const std::size_t slotIndex) const
{
    return RequireCurrentDisplayWindow().SpriteAt(slotIndex);
}

bool CgDisplayResourceRuntime::CgDisplayWindowActiveAt0C() const
{
    return RequireCurrentDisplayWindow().WindowActiveAt0C();
}

bool CgDisplayResourceRuntime::CgOperationWindowActiveAt0C() const noexcept
{
    return operationWindow_->ActiveAt0C();
}

std::size_t CgDisplayResourceRuntime::PrimarySpriteSlotCount() const
{
    return RequireCurrentDisplayWindow().SpriteSlotCount();
}

const char* CgDisplayResourceRuntime::PrimaryLogicalNameCharacters(
    const std::size_t slotIndex) const
{
    return RequireCurrentDisplayWindow().SpriteAt(slotIndex).LogicalName().c_str();
}

void CgDisplayResourceRuntime::ClearPrimarySprite(
    const std::size_t slotIndex)
{
    RequireCurrentDisplayWindow().SpriteAt(slotIndex).Clear();
}

PatternSpriteReceiver CgDisplayResourceRuntime::CaptureSpriteReceiver(const std::uint32_t slotIndex) const
{
    auto* const window = dynamic_cast<CgDisplayWindowRuntime*>(displayWindowSlot_.get());
    if (!window || slotIndex >= window->SpriteSlotCount()) return 0;
    return reinterpret_cast<PatternSpriteReceiver>(&window->SpriteAt(slotIndex));
}

void CgDisplayResourceRuntime::LoadResource(const std::uint32_t slotIndex,
    const PatternSpriteReceiver capturedReceiver, const PatternResourceLoadRequest& request,
    const IPatternResourceFailureName& failureName, IPatternResourceLoadCommit& commit,
    IRuntimeStringManagerSource& strings)
{
    CgLoadAction action(displayWindowSlot_, slotIndex, capturedReceiver, commit);
    resourceLoader_->ExecuteLoad(request, failureName, action, strings);
}

const char* CgDisplayResourceRuntime::ReceiverPrimaryNameCharacters(const PatternSpriteReceiver receiver) const
{
    return RequireCgReceiver(receiver).PrimaryMemberName().c_str();
}

PatternResourceNamesBorrow CgDisplayResourceRuntime::ReceiverAuxiliaryNames(
    const PatternSpriteReceiver receiver) const
{
    return PatternResourceNamesBorrow(RequireCgReceiver(receiver).AuxiliaryMemberNames());
}

void CgDisplayResourceRuntime::ClearReceiverFileGroup(const PatternSpriteReceiver receiver)
{
    RequireCgReceiver(receiver).Clear();
}

void CgDisplayResourceRuntime::SetReceiverLogicalName(
    const PatternSpriteReceiver receiver, const RuntimeCString& source)
{
    RequireCgReceiver(receiver).SetLogicalNameModernFromCString(source);
}

void CgDisplayResourceRuntime::SetReceiverActive(const PatternSpriteReceiver receiver, const bool active)
{
    RequireCgReceiver(receiver).SetActive(active);
}

void CgDisplayResourceRuntime::SetReceiverOpaqueValuesAt2CAnd30(
    const PatternSpriteReceiver receiver,
    const std::uint32_t valueAt2C,
    const std::uint32_t valueAt30)
{
    RequireCgReceiver(receiver).SetOpaqueValuesAt2CAnd30(
        valueAt2C, valueAt30);
}

void CgDisplayResourceRuntime::SetCgDisplayWindowActiveAt0C(
    const bool active)
{
    RequireCurrentDisplayWindow().SetWindowActiveAt0C(active);
}

void CgDisplayResourceRuntime::SetCgOperationWindowActiveAt0C(
    const bool active) noexcept
{
    operationWindow_->SetActiveAt0CWithoutLayerSubmission(active);
}

void CgDisplayResourceRuntime::SetCgOperationDialogSelectionValueAt7C(
    const std::int32_t value) noexcept
{
    operationDialog_->SetSelectionValueAt7C(value);
}

void CgDisplayResourceRuntime::OpenCgOperationDialogAtVslot44()
{
    operationDialog_->OpenAtVslot44();
}
}
