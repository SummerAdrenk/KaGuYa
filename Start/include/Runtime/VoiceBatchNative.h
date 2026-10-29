#pragma once
#include <Runtime/RuntimeCStringArray.h>

namespace KaGuYa::Start::Runtime
{
class IScrVmVoiceRuntimeBoundary;
// 42C4F0：真实CString数组借用；返回值被VOICE忽略，重播/读档调用者保留它。
bool PlayVoiceBatchAt42C4F0(IScrVmVoiceRuntimeBoundary& voice, const RuntimeCStringArrayReference& names);
}
