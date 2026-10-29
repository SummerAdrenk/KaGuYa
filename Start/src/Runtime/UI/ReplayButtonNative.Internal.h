#pragma once
#include <Runtime/ReplayButtonNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/SceneManagerNative.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <bit>
#include <exception>
#include <new>

namespace KaGuYa::Start::Runtime::ReplayButtonNativeDetail
{
using Body = ReplayButtonNativeBody84h;
using Slot = YtlReferenceSlotRuntime;
using Button = CgUiButtonNativeBody6Ch;
using Surface = GraphicsSurfaceNativeBody;
template<class T> T Read(const T& value) noexcept
{ return *static_cast<const volatile T*>(&value); }
template<class T> T& Field(void* p, unsigned offset) noexcept
{ return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(p) + offset); }
inline int Add(int a, int b) noexcept
{ return std::bit_cast<int>(std::uint32_t(a) + std::uint32_t(b)); }
inline int Sub(int a, int b) noexcept
{ return std::bit_cast<int>(std::uint32_t(a) - std::uint32_t(b)); }
inline int Mul(int a, int b) noexcept
{ return std::bit_cast<int>(std::uint32_t(a) * std::uint32_t(b)); }
template<class T> T* Advance(T* p, int n) noexcept
{ return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(p) + std::uint32_t(n) * sizeof(T)); }
template<class T> int Distance(T* first, T* last) noexcept
{ return std::bit_cast<int>(std::uint32_t(reinterpret_cast<std::uintptr_t>(last) - reinterpret_cast<std::uintptr_t>(first))) / int(sizeof(T)); }
template<class T> T* Resolve(const Slot& r, int index = 0) noexcept
{
    const auto base = Read(r.baseAt00);
    if (!base) return nullptr;
    auto* c = static_cast<EnvironmentObjectReferenceControl*>(Read(r.controlAt08));
    const auto stride = c ? Read(c->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + (Read(r.biasAt04) + std::uint32_t(index)) * stride);
}
inline int Count(const Slot& r) noexcept
{
    auto* c = static_cast<EnvironmentObjectReferenceControl*>(Read(r.controlAt08));
    return c ? std::bit_cast<int>(Read(c->countAt10)) : 0;
}
inline int Count(const GuiControlSurfaceReferences& v) noexcept
{ auto* last = Read(v.end); return Distance(Read(v.begin), last); }
inline const ReplayButtonNativeVtable15* Methods(Body& b) noexcept
{ return reinterpret_cast<const ReplayButtonNativeVtable15*>(Read(b.common.vptrAt00)); }
inline void ReleaseSurface(Slot& slot) noexcept { ReleaseYtlReference(slot); }

// 缓存control的内联正常尾声，与40D2F0/4A3460的当前槽EH释放分开。
void ReleaseCaptured(Slot&, YtlReferenceControlRuntime*, bool clearSlot);
Slot CopyCaptured(const Slot&, YtlReferenceControlRuntime*) noexcept;
void Assign(Slot&, const Slot*);
void* Root(unsigned ensureSite, unsigned readSite);
void* Draw(unsigned ensureSite, unsigned readSite);
void Blit(Surface* target, Surface* source, int x, int y);
void AppendChild(ModeButtonReferenceVector&, const Slot&);
void AdoptButton(Slot&, Button*);
void AdoptPageArray(Slot&, Button*, int count);

extern const char* const backgrounds[10];
extern const char* const pageMarks[3];
extern const char* const stamps[3];
extern const char* const percentDigits[10];
extern const char* const formats[3];
extern const CgUiButtonRecordNative28h pageRecords[2];
extern const POINT positions[9];
extern const POINT percentPositions[3];
extern const POINT buttonOffset;
extern const POINT imageOffset;
extern const int neighbors[27][4];
struct UnwindEntry final { int previous; unsigned action; };
extern const UnwindEntry constructorUnwind[35];
extern const UnwindEntry percentUnwind[7];
}
