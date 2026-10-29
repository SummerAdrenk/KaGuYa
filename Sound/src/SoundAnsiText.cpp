#include "SoundAnsiText.h"

namespace KaGuYa::Sound
{
SoundAnsiText::SoundAnsiText(const char* source)
    // 原03 10004490/10010A10与ATL14.44窄串构造一致：先查资源ID，
    // CP_THREAD_ACP query(-1)-1，再convert(-1,length)；忽略普通返回，定长补NUL。
    : text_(source)
{
}

const wchar_t* SoundAnsiText::GetString() const noexcept
{
    return text_.GetString();
}

int SoundAnsiText::GetLength() const noexcept
{
    return text_.GetLength();
}

bool SoundAnsiText::IsEmpty() const noexcept
{
    return text_.IsEmpty();
}

SoundAnsiOpenNames::SoundAnsiOpenNames(const char* logicalName, const char* physicalName)
    : logicalName_(logicalName), physicalName_(physicalName)
{
}

const wchar_t* SoundAnsiOpenNames::LogicalName() const noexcept
{
    return logicalName_.GetString();
}

const wchar_t* SoundAnsiOpenNames::PhysicalName() const noexcept
{
    // 原03 100033A7..100033B3检查CString声明长度，不是输入指针是否为null。
    return physicalName_.IsEmpty() ? nullptr : physicalName_.GetString();
}
}
