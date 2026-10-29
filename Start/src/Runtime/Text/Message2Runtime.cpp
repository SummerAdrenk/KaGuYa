#include <Runtime/Message2Runtime.h>

namespace KaGuYa::Start::Runtime
{
Message2Runtime::Message2Runtime(IStartFileApi& api, const FileTransactionContext* transaction)
    : context_{api, transaction} {}
bool Message2Runtime::Load(const char* path) { return native_.LoadWithContext(path, context_); }
float Message2Runtime::ReadVersionAt47DDE0() const { return native_.ReadVersionAt47DDE0(); }
MessageVoiceAndTextRef Message2Runtime::GetMessage(std::int32_t index)
{ return native_.GetMessageWithContext(index, context_); }
MessageTextCommandReference Message2Runtime::GetName(std::int32_t index)
{ return native_.GetNameWithContext(index, context_); }
MessageTextCommandReference Message2Runtime::GetChoice(std::int32_t index)
{ return native_.GetChoiceWithContext(index, context_); }
Message2FormatRef Message2Runtime::GetFormat(std::int32_t index)
{ return native_.GetFormatWithContext(index, context_); }
std::int32_t Message2Runtime::GetNameCount() const noexcept { return native_.GetNameCount(); }
std::int32_t Message2Runtime::GetChoiceCount() const noexcept { return native_.GetChoiceCount(); }
std::int32_t Message2Runtime::GetMessageCount() const noexcept { return native_.GetMessageCount(); }
std::int32_t Message2Runtime::GetFormatCount() const noexcept { return native_.GetFormatCount(); }
}
