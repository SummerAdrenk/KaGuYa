#include <Runtime/MessageReaderReference.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class CreateControl>
void Publish(YtlReferenceSlotRuntime& slot, NativeMessageReader* created, std::uint32_t stride,
    CreateControl createControl)
{
    // 45A84A/45A955/451A2F：先发布 bias/base，再分配控制块；/sdl 不得清零未知 padding。
    if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(created);
    auto* control = createControl(created);
    slot.controlAt08 = control;
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = stride;
    control->countAt10 = 1;
}
}

MessageReaderReference::MessageReaderReference(const YtlReferenceSlotRuntime& source) noexcept : reference_(source)
{
    if (reference_.controlAt08) ++reference_.controlAt08->referenceCountAt04;
}
MessageReaderReference::MessageReaderReference(const MessageReaderReference& source) noexcept
    : MessageReaderReference(source.reference_) {}
MessageReaderReference& MessageReaderReference::operator=(const MessageReaderReference& source)
{
    if (this != &source)
    {
        ReleaseYtlReference(reference_);
        reference_ = source.reference_;
        if (reference_.controlAt08) ++reference_.controlAt08->referenceCountAt04;
    }
    return *this;
}
MessageReaderReference::MessageReaderReference(MessageReaderReference&& source) noexcept
    : reference_(std::exchange(source.reference_, {})) {}
MessageReaderReference& MessageReaderReference::operator=(MessageReaderReference&& source)
{
    if (this != &source)
    {
        ReleaseYtlReference(reference_);
        reference_ = std::exchange(source.reference_, {});
    }
    return *this;
}
MessageReaderReference::~MessageReaderReference() noexcept { ReleaseYtlReference(reference_); }

void MessageReaderReference::ReplaceOwnedImpl(NativeMessageReader* created, MessageReaderKind kind,
    CreateControl createControl)
{
    // 45A805/45A910/4519E7：新正文构造以后才释放旧槽；空正文也执行释放。
    ReleaseYtlReference(reference_);
    if (!created) return;
    switch (kind)
    {
    case MessageReaderKind::Message2: Publish(reference_, created, 0x90, createControl); break;
    case MessageReaderKind::Legacy: Publish(reference_, created, 0x1C, createControl); break;
    case MessageReaderKind::Null: Publish(reference_, created, 0x10, createControl); break;
    default: throw std::logic_error("invalid CMessage reader control kind");
    }
}
void MessageReaderReference::Reset() { ReleaseYtlReference(reference_); }
NativeMessageReader* MessageReaderReference::Get() const noexcept
{
    return ResolveEnvironmentObjectReference<NativeMessageReader>(reference_);
}
}
