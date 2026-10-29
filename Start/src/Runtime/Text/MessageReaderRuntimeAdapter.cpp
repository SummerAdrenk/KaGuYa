#include <Runtime/MessageReaderRuntimeAdapter.h>

namespace KaGuYa::Start::Runtime
{
// 已知的两种磁盘reader显式传入外部服务，其余reader履行自己的原生虚接口。
// 按正文实际类型派发，不根据version、control或stride猜类型；无control的已发布正文也有效。
bool MessageReaderRuntimeAdapter::Load(const char* path)
{
    auto* current = reader_.Get();
    if (auto* message = dynamic_cast<NativeMessage2Runtime*>(current))
        return message->LoadWithContext(path, context_);
    if (auto* legacy = dynamic_cast<NativeLegacyMessageRuntime*>(current))
        return legacy->LoadWithContext(path, context_);
    return current->Load(path);
}
Message2FormatRef MessageReaderRuntimeAdapter::GetFormat(std::int32_t index)
{
    auto* current = reader_.Get();
    if (auto* message = dynamic_cast<NativeMessage2Runtime*>(current))
        return message->GetFormatWithContext(index, context_);
    if (auto* legacy = dynamic_cast<NativeLegacyMessageRuntime*>(current))
        return legacy->GetFormatWithContext(index, context_);
    return current->GetFormat(index);
}
MessageTextCommandReference MessageReaderRuntimeAdapter::GetChoice(std::int32_t index)
{
    auto* current = reader_.Get();
    if (auto* message = dynamic_cast<NativeMessage2Runtime*>(current))
        return message->GetChoiceWithContext(index, context_);
    return current->GetChoice(index);
}
MessageVoiceAndTextRef MessageReaderRuntimeAdapter::GetMessage(std::int32_t index)
{
    auto* current = reader_.Get();
    if (auto* message = dynamic_cast<NativeMessage2Runtime*>(current))
        return message->GetMessageWithContext(index, context_);
    return current->GetMessage(index);
}
float MessageReaderRuntimeAdapter::ReadVersionAt47DDE0() const
{ return reader_->ReadVersionAt47DDE0(); }
std::int32_t MessageReaderRuntimeAdapter::GetFormatCount() const noexcept
{ return reader_->GetFormatCount(); }
std::int32_t MessageReaderRuntimeAdapter::GetChoiceCount() const noexcept
{ return reader_->GetChoiceCount(); }
std::int32_t MessageReaderRuntimeAdapter::GetMessageCount() const noexcept
{ return reader_->GetMessageCount(); }
}
