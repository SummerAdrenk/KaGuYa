#pragma once
#include <Runtime/DemoSequenceNative.h>
#include <Runtime/DemoSpriteNative.h>
#include <Runtime/GuiDirtyRectNative.h>
#include <Runtime/DemoSpriteLayerRuntime.h>
#include <Runtime/GraphicsSurfaceNativeBody.h>
#include <bit>

namespace KaGuYa::Start::Runtime::DemoNativeDetail
{
template<class T> T Read(const T& value) noexcept { return *static_cast<const volatile T*>(&value); }
inline std::int32_t Signed(std::uint32_t value) noexcept { return std::bit_cast<std::int32_t>(value); }
inline YtlReferenceSlotRuntime& At(EnvironmentVectorStorage<YtlReferenceSlotRuntime>& vector,std::uint32_t offset)
{ return *reinterpret_cast<YtlReferenceSlotRuntime*>(reinterpret_cast<std::uintptr_t>(Read(vector.begin))+offset); }
inline std::uint32_t Count(const EnvironmentVectorStorage<YtlReferenceSlotRuntime>& vector)
{
    const auto end=reinterpret_cast<std::uintptr_t>(Read(vector.end));
    const auto begin=reinterpret_cast<std::uintptr_t>(Read(vector.begin));
    return std::uint32_t(Signed(std::uint32_t(end-begin))/12);
}
inline DemoSpriteNativeBodyBCh* Sprite(YtlReferenceSlotRuntime& reference)
{ return ResolveEnvironmentObjectReference<DemoSpriteNativeBodyBCh>(reference); }
inline GraphicsSurfaceNativeBody* Surface(YtlReferenceSlotRuntime& reference)
{ return ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(reference); }
inline GuiDirtyRectNativeBody80h* Dirty(DemoSequenceNativeBodyA4h& body)
{ return ResolveGuiDirtyRectReference(body.dirtyAt44); }
inline void AddDirty(DemoSequenceNativeBodyA4h& body,const RECT& rectangle)
{ auto* dirty=Dirty(body);dirty->vptrAt00->addRectAt08(dirty,&rectangle); }
inline void AddDemoDirtyAt4B4530(DemoSequenceNativeBodyA4h& body,const RECT& rectangle)
{
    // UPDATE/MOVE的原wrapper先判空；零矩形不得解析可能尚未存在的dirty Ref。
    if (!::IsRectEmpty(&rectangle)) AddDirty(body,rectangle);
}
inline void ResetPacket(TransitOperation& operation)
{ operation=TransitOperation{}; }
}
