#pragma once
#include <Runtime/SoundParameterNative.h>

namespace KaGuYa::Start::Runtime
{
// 476A80/+A0：设置页试听映射。out为未构造返回槽，不是全局声音打开的前置变换。
// Ref借用保持到函数返回；每层按原位置重解，不预存人物/注册/Key的值快照。
void ConstructSoundResourcePathAt476A80(RuntimeCString* out, const char* resourceName,
    const YtlReferenceSlotRuntime& characters, const YtlReferenceSlotRuntime& types,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
