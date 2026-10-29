#pragma once
#include <Runtime/EnvironmentPathRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 46CC90不消费路径对象ECX；复制CString、删除一个非盘根尾斜杠后查询物理属性。
bool EnvironmentPathExistsNativeAt46CC90(const char*,IEnvironmentPathApi&,
    IRuntimeStringManagerSource& =DefaultRuntimeStringManagerSource());
}
