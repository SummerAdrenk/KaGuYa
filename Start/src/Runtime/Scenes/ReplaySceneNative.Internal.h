#pragma once
#include <Runtime/ReplaySceneNative.h>
#include <Runtime/SceneGroupReferenceNative.h>
#include "CgSceneNative.Internal.h"

namespace KaGuYa::Start::Runtime::ReplaySceneNativeDetail
{
using Body=ReplaySceneNativeBody80h;
using Slot=YtlReferenceSlotRuntime;
using CgSceneNativeDetail::Read;
using CgSceneNativeDetail::Field;
using CgSceneNativeDetail::Resolve;
using CgSceneNativeDetail::ResolveWithBase;
using CgSceneNativeDetail::Advance;
using CgSceneNativeDetail::Add;
using CgSceneNativeDetail::Sub;
using CgSceneNativeDetail::Root;
using CgSceneNativeDetail::Draw;
using CgSceneNativeDetail::Scenes;
using CgSceneNativeDetail::Registry;
using CgSceneNativeDetail::ResetOperation;
void Retire(Body&) noexcept;
void CaptureScreen(Body&,IReplaySceneNativeSource&,unsigned site);
ReplayButtonNativeBody84h* Group(Body&) noexcept;
const ReplayButtonNativeVtable15* GroupMethods(ReplayButtonNativeBody84h&) noexcept;
bool PollSelection(Body&,ReplaySceneNativeServices&);
void PrepareReplay(Body&,ReplaySceneNativeServices&);
const char* CgName() noexcept;
const char* ScriptName() noexcept;
}
