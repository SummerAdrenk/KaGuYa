#pragma once
#include <Runtime/SceneManagerNative.h>
#include "../UI/CgUiButtonNative.Internal.h"

namespace KaGuYa::Start::Runtime::SceneManagerNativeDetail
{
using namespace CgUiButtonNativeDetail;
enum class OwnedBody{Scheduler,Controller,Mode,Dialog,Manager};
void Replace(YtlReferenceSlotRuntime&,void*,OwnedBody,std::uint32_t,SceneManagerNativeServices&);
}
