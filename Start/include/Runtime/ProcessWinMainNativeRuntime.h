#pragma once
#include <Windows.h>

namespace KaGuYa::Start::Runtime
{
class IWinMainNativeSource;
// 调用方先完成ProcessRoot长图；本构造只借该图，不加载params或查询环境正文。
// 正式入口仅调用一次，源/API/诊断元数据以显式存储覆盖全部退出回调。
IWinMainNativeSource& ConstructProcessWinMainNativeSourceAtEntry();
int RunProcessWinMainNativeAt43D6E0(HINSTANCE, HINSTANCE, LPSTR, int);
}
