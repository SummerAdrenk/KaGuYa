#pragma once
#include <Runtime/GraffitiCountMultiNative.h>
#include "GraffitiCountDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime::GraffitiCountMultiDetail
{
using namespace GraffitiCountDialogNativeDetail;
using Multi = GraffitiCountMultiNativeBody;
void __fastcall Begin(Dialog*, void*);
std::int32_t __fastcall Poll(Dialog*, void*);
std::int32_t __fastcall End(Dialog*, void*);
void __fastcall Input(Dialog*, void*);
Dialog* __fastcall Delete(Dialog*, void*, std::uint32_t) noexcept;
}
