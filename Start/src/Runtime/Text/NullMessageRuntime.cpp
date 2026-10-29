#include <Runtime/NullMessageRuntime.h>

namespace KaGuYa::Start::Runtime
{
bool NullMessageRuntime::Load(const char* path)
{
    // 484E30..484E34：只清 AL，不检查路径，不打开文件，也不更新任何状态。
    return native_.Load(path);
}

float NullMessageRuntime::ReadVersionAt47DDE0() const
{
    // 47DDF0..47DDF2：返回正零；不能套用 Message/Message2 的 +04 optional 读取。
    return native_.ReadVersionAt47DDE0();
}

std::int32_t NullMessageRuntime::GetFormatCount() const noexcept
{
    return native_.GetFormatCount();
}

Message2FormatRef NullMessageRuntime::GetFormat(std::int32_t index)
{
    // 47AB66/47AB6C/47AB73：返回 Ref 的三个字段清零，不构造空格式对象。
    return native_.GetFormat(index);
}

std::int32_t NullMessageRuntime::GetChoiceCount() const noexcept
{
    return native_.GetChoiceCount();
}

MessageTextCommandReference NullMessageRuntime::GetChoice(std::int32_t index)
{
    // 47D0F6/47D0FC/47D103：忽略索引，返回空 Ref，不构造空字符串。
    return native_.GetChoice(index);
}

std::int32_t NullMessageRuntime::GetMessageCount() const noexcept
{
    return native_.GetMessageCount();
}

MessageVoiceAndTextRef NullMessageRuntime::GetMessage(std::int32_t index)
{
    // 47B356/47B35C/47B363：忽略索引，不分配正文、语音或文本对象。
    return native_.GetMessage(index);
}
}
