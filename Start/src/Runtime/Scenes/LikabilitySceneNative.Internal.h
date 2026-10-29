#pragma once
#include <Runtime/LikabilitySceneNative.h>
#include "CgSceneNative.Internal.h"

namespace KaGuYa::Start::Runtime::LikabilitySceneNativeDetail
{
using Body=LikabilitySceneNativeBody20h;
using Slot=YtlReferenceSlotRuntime;
using CgSceneNativeDetail::Read;
using CgSceneNativeDetail::Resolve;
using CgSceneNativeDetail::Field;
void Retire(Body&)noexcept;
inline void* Mode(ILikabilitySceneNativeSource& source,unsigned site)
{auto* env=source.CaptureEnvironmentAt4180C0(site);return ResolveEnvironmentArchiveAt417990(*env,2);}
inline void* Scenes(ILikabilitySceneNativeSource& source,unsigned site)
{source.EnsureRootAt413BE0(site);auto* root=source.RootAt6198C4(site+5u);return Resolve<void>(source.RootScenesSlotAt30(root));}
}
