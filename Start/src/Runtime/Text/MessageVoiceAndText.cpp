#include <Runtime/MessageVoiceAndText.h>
#include <Runtime/EnvironmentObjectReference.h>

#include <utility>

namespace KaGuYa::Start::Runtime
{
MessageVoiceAndTextRef::MessageVoiceAndTextRef(const YtlReferenceSlotRuntime& source) noexcept
    : reference_(source)
{
    if (reference_.controlAt08) ++reference_.controlAt08->referenceCountAt04;
}
MessageVoiceAndTextRef::MessageVoiceAndTextRef(const MessageVoiceAndTextRef& source) noexcept
    : MessageVoiceAndTextRef(source.reference_) {}
MessageVoiceAndTextRef& MessageVoiceAndTextRef::operator=(const MessageVoiceAndTextRef& source)
{
    if (this != &source)
    {
        ReleaseYtlReference(reference_);
        reference_ = source.reference_;
        if (reference_.controlAt08) ++reference_.controlAt08->referenceCountAt04;
    }
    return *this;
}
MessageVoiceAndTextRef::MessageVoiceAndTextRef(MessageVoiceAndTextRef&& source) noexcept
    : reference_(std::exchange(source.reference_, {})) {}
MessageVoiceAndTextRef& MessageVoiceAndTextRef::operator=(MessageVoiceAndTextRef&& source)
{
    if (this != &source)
    {
        ReleaseYtlReference(reference_);
        reference_ = std::exchange(source.reference_, {});
    }
    return *this;
}
MessageVoiceAndTextRef::~MessageVoiceAndTextRef() noexcept { Reset(); }
void MessageVoiceAndTextRef::Reset() { ReleaseYtlReference(reference_); }
MessageVoiceAndText* MessageVoiceAndTextRef::Get() const noexcept
{ return ResolveEnvironmentObjectReference<MessageVoiceAndText>(reference_); }

void ConstructMessageVoiceAndTextAt451650(YtlReferenceSlotRuntime& output,
    MessageVoiceAndText* body, std::uint8_t owning)
{
    output = {};
    if (!body) return;
    output.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    // 451674/45168F：先发布正文，再分配 control；发布 control 后才填写业务字段。
    using Control = EnvironmentObjectControl<MessageVoiceAndText>;
    static_assert(sizeof(Control) == 24);
    void* storage = ::operator new(sizeof(Control));
    auto* control = storage ? ::new (storage) Control(body) : nullptr;
    output.controlAt08 = control;
    control->referenceCountAt04 = 1;
    control->owningAt08 = owning;
    control->strideAt0C = 24;
    control->countAt10 = 1;
}

MessageVoiceAndTextRef MakeMessageVoiceAndText()
{
    // 47B43D / 47A784：两个 Ref 构造只清零，不引入字符串管理器或隐式分配。
    void* storage = ::operator new(sizeof(MessageVoiceAndText));
    auto* body = storage ? ::new (storage) MessageVoiceAndText : nullptr;
    MessageVoiceAndTextRef result;
    ConstructMessageVoiceAndTextAt451650(result.RawSlot(), body);
    return result;
}
}
