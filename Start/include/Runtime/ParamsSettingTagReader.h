#pragma once

#include <Runtime/ParamsSettingTagRuntime.h>

namespace KaGuYa::Start::Runtime
{
class ParamsRuntimeReadCursor;

// 41FE80只在整节点成功返回后提交调用者游标；正文/引用的中途副作用不回滚。
void ReadParamsSettingTagAt41FE80(ParamsRuntimeReadCursor& cursor, YtlReferenceSlotRuntime& target,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
}
