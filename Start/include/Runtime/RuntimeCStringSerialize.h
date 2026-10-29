#pragma once
#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
class SerializeBuffer;
// 465ED0的CString叶；serializer仍是显式现代适配器，不冒充原18h对象。
void SerializeRuntimeCStringAt465ED0(SerializeBuffer& serializer, RuntimeCString& value);
}
