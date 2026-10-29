#pragma once
#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
// 目标已完成40C760的nil/空串初始化；完整函数与LINK内联副本的Left正常释放取值点不同。
void ScanArchiveTitleAt40C130(RuntimeCString&,const char*,
    IRuntimeStringManagerSource& =DefaultRuntimeStringManagerSource());
void ScanLinkTitleAt493F67(RuntimeCString&,const char*,
    IRuntimeStringManagerSource& =DefaultRuntimeStringManagerSource());
}
