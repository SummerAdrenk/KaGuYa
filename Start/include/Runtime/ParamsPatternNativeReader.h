#pragma once

#include <Runtime/ParamsPatternNative.h>

namespace KaGuYa::Start::Runtime
{
class ParamsRuntimeReadCursor;

// 41DFD0：按原版本读取单文件、旧文件组或>=5.7的kind项，以及两张组表。
// 四表逐段提交，失败不回滚已追加项或提前清除后表。
void ReadParamsPatternNativeAt41DFD0(ParamsRuntimeReadCursor& cursor,
    ParamsPatternNative& pattern, double version,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
