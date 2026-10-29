#include <Runtime/LegacyMessageRuntime.h>

namespace KaGuYa::Start::Runtime
{
LegacyMessageRuntime::LegacyMessageRuntime(IStartFileApi& files, const FileTransactionContext* transaction)
    : context_{files, transaction} {}
bool LegacyMessageRuntime::Load(const char* path) { return native_.LoadWithContext(path, context_); }
float LegacyMessageRuntime::ReadVersionAt47DDE0() const { return native_.ReadVersionAt47DDE0(); }
Message2FormatRef LegacyMessageRuntime::GetFormat(std::int32_t index)
{ return native_.GetFormatWithContext(index, context_); }
MessageTextCommandReference LegacyMessageRuntime::GetChoice(std::int32_t index)
{ return native_.GetChoice(index); }
MessageVoiceAndTextRef LegacyMessageRuntime::GetMessage(std::int32_t index)
{ return native_.GetMessage(index); }
std::int32_t LegacyMessageRuntime::GetFormatCount() const noexcept { return native_.GetFormatCount(); }
std::int32_t LegacyMessageRuntime::GetChoiceCount() const noexcept { return native_.GetChoiceCount(); }
std::int32_t LegacyMessageRuntime::GetMessageCount() const noexcept { return native_.GetMessageCount(); }
}
