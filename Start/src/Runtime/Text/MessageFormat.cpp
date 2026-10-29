#include <Runtime/MessageFormat.h>

namespace KaGuYa::Start::Runtime
{
void ConstructMessageRawFormatAt47AB80(YtlReferenceSlotRuntime& output)
{
    // 47ABB2..47ABBF先清返回槽；raw16失败时不能保留调用前的字段。
    output = {};
    void* storage = ::operator new(sizeof(MessageRawFormat));
    auto* body = storage ? ::new (storage) MessageRawFormat : nullptr;
    // 47AC07重新检查槽：正文分配回调可以改变它。control失败保留孤立正文。
    ReplaceEnvironmentObjectReference(output, body, 16);
}

void ConstructMessage2FormatAt451570(YtlReferenceSlotRuntime& output,
    Message2Format* body, std::uint8_t owning)
{
    output = {};
    if (!body) return;
    output.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    using Control = EnvironmentObjectControl<Message2Format>;
    static_assert(sizeof(Control) == 24);
    // 451594 / 4515AF：先发布正文，分配control，发布后再填refs/owning/stride/count。
    void* storage = ::operator new(sizeof(Control));
    auto* control = storage ? ::new (storage) Control(body) : nullptr;
    output.controlAt08 = control;
    control->referenceCountAt04 = 1;
    control->owningAt08 = owning;
    control->strideAt0C = 24;
    control->countAt10 = 1;
}

MessageRawFormatRef MakeMessageRawFormat()
{
    MessageRawFormatRef result;
    ConstructMessageRawFormatAt47AB80(result.RawSlot());
    return result;
}

Message2FormatRef MakeMessage2Format()
{
    // 47A2B5及47A5B0的构造次序：裸正文完成后才进入451570；不能make_shared合并分配。
    void* storage = ::operator new(sizeof(Message2Format));
    auto* body = storage ? ::new (storage) Message2Format : nullptr;
    Message2FormatRef result;
    ConstructMessage2FormatAt451570(result.RawSlot(), body);
    return result;
}
}
