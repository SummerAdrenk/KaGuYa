#pragma once
#include <Runtime/ToolDialogNative.h>

namespace KaGuYa::Start::Runtime::BakuToolData
{
// Baku Start.exe 4E1F20：正文56E60C、回想56EA20，各五组控件。
const ToolDialogNativeProfile& Profile() noexcept;
const ToolDialogNativeRelation& Relation() noexcept;
}
