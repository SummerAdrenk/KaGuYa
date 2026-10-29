#pragma once
#include <Runtime/SaveFileDeleteFactoryNative.h>

namespace KaGuYa::Start::Runtime
{
class SaveFileStateNativeArchive;
// 48DBD0仅jmp到48D290；ECX不是业务参数，选择结果写回Env slot6原字段。
SaveFileStateNativeArchive* RefreshSaveFileSelectionNativeAt48D290(ISaveFileDeleteFactoryNativeSource&);
}
