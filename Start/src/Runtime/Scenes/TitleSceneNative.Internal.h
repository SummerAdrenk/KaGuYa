#pragma once
#include <Runtime/TitleSceneNative.h>
#include <Runtime/SceneManagerView.h>

namespace KaGuYa::Start::Runtime::TitleSceneNativeDetail
{
using Body=TitleSceneNativeBody1Ch;
using Table=TitleSceneNativeVtable4;
TitleSceneNativeServices& ActiveServices();
void Retire(Body&) noexcept;
void AdoptDialog(Body&,void*,TitleSceneNativeServices&);
// 内联417990变体先end后begin；保留负差值经signed除12再unsigned判定。
void* InlineArchive(EnvironmentRootStorage*,std::uint32_t index);
inline SceneManagerView View(TitleSceneNativeManagerReceiver r)
{return {r.body,*r.services};}
}
