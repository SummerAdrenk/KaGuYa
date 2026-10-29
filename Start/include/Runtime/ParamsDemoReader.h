#pragma once

#include <Runtime/ParamsDemoStorage.h>

namespace KaGuYa::Start::Runtime
{
class ParamsRuntimeReadCursor;

void ReadParamsDemoDataAt419EB0(ParamsRuntimeReadCursor& cursor, ParamsDemoDataRuntime* target,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void ReadParamsDemoEntriesAt41CDB8(ParamsRuntimeReadCursor& cursor, ParamsDemoStorage& target,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
}
