#pragma once
#include <Runtime/CgButtonNative.h>
#include <Runtime/ICgScButtonNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/SceneManagerNative.h>
#include <Runtime/SecretCgNativeArchive.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <bit>
#include <exception>
#include <new>

namespace KaGuYa::Start::Runtime::CgButtonNativeDetail
{
using Body = CgButtonNativeBody84h;
using Slot = YtlReferenceSlotRuntime;
using Button = CgUiButtonNativeBody6Ch;
using Surface = GraphicsSurfaceNativeBody;
// 仅借用同类型字段，不读入镜像；地址计算不把Replay转换成CgButton的C++对象类型。
struct Fields84 final
{
    void* address;
    ModeButtonNativeBody50h& common;
    GuiControlSurfaceReferences& pagesAt50;
    Slot& pageButtonsAt5C;
    std::int32_t& pageAt6C;
    std::int32_t& perPageAt70;
    std::int32_t& categoryAt74;
    Slot& selectorAt78;
    template<class T>static T& At(void* p,unsigned offset)noexcept
    {return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(p)+offset);}
    explicit Fields84(void* p)noexcept:address(p),common(At<ModeButtonNativeBody50h>(p,0)),
        pagesAt50(At<GuiControlSurfaceReferences>(p,0x50)),pageButtonsAt5C(At<Slot>(p,0x5C)),
        pageAt6C(At<std::int32_t>(p,0x6C)),perPageAt70(At<std::int32_t>(p,0x70)),
        categoryAt74(At<std::int32_t>(p,0x74)),selectorAt78(At<Slot>(p,0x78)){}
};
template<class T> T Read(const T& value) noexcept
{ return *static_cast<const volatile T*>(&value); }
template<class T> T& Field(void* p, std::uint32_t offset) noexcept
{ return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(p) + offset); }
inline int Add(int a, int b) noexcept
{ return std::bit_cast<int>(std::uint32_t(a) + std::uint32_t(b)); }
inline int Sub(int a, int b) noexcept
{ return std::bit_cast<int>(std::uint32_t(a) - std::uint32_t(b)); }
inline int Mul(int a, int b) noexcept
{ return std::bit_cast<int>(std::uint32_t(a) * std::uint32_t(b)); }
template<class T> T* Advance(T* p, int i) noexcept
{ return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(p) + std::uint32_t(i) * sizeof(T)); }
template<class T> int Distance(T* first, T* last) noexcept
{ return std::bit_cast<int>(std::uint32_t(reinterpret_cast<std::uintptr_t>(last) - reinterpret_cast<std::uintptr_t>(first))) / int(sizeof(T)); }
template<class T> T* Resolve(const Slot& r, int i = 0) noexcept
{
    const auto base = Read(r.baseAt00);
    if (!base) return nullptr;
    auto* c = static_cast<EnvironmentObjectReferenceControl*>(Read(r.controlAt08));
    const auto stride = c ? Read(c->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + (Read(r.biasAt04) + std::uint32_t(i)) * stride);
}
inline int Count(const Slot& r) noexcept
{
    auto* c = static_cast<EnvironmentObjectReferenceControl*>(Read(r.controlAt08));
    return c ? std::bit_cast<int>(Read(c->countAt10)) : 0;
}
inline int Count(const GuiControlSurfaceReferences& r) noexcept
{ auto* end = Read(r.end); return Distance(Read(r.begin), end); }
inline const CgButtonNativeVtable15* Methods(Body& b) noexcept
{ return reinterpret_cast<const CgButtonNativeVtable15*>(Read(b.common.vptrAt00)); }
inline const ICgScButtonNativeVtable15* Methods(Fields84& b)noexcept
{return reinterpret_cast<const ICgScButtonNativeVtable15*>(Read(b.common.vptrAt00));}
template<class B>inline Surface* Page(B& b) noexcept
{ return Resolve<Surface>(*Advance(Read(b.pagesAt50.begin), Read(b.pageAt6C))); }
template<class B>inline Button* Child(B& b, int i) noexcept
{ return Resolve<Button>(*Advance(Read(b.common.childrenAt14.begin), i)); }
template<class B>inline bool OnPage(B& b, int i) noexcept
{ return i >= Mul(Read(b.pageAt6C), Read(b.perPageAt70)) && i < Mul(Add(Read(b.pageAt6C), 1), Read(b.perPageAt70)); }
template<class B>inline bool PageVisible(B& b, int i) noexcept
{
    if (i == 0 && Read(b.pageAt6C) == 0) return false;
    return i != 1 || Read(b.pageAt6C) != Sub(Count(b.pagesAt50), 1);
}
template<class B>inline SceneManagerOptionCharNativeView Selector(B& b) noexcept
{ return SceneManagerOptionCharNativeView(&b.selectorAt78, RequireCgButtonNativeServices().selector); }
inline void ReleaseSurface(Slot& r) noexcept { ReleaseYtlReference(r); } // 40D2F0, EHFlags=5
inline Slot Copy(const Slot& r) noexcept
{
    Slot out{Read(r.baseAt00), Read(r.biasAt04), Read(r.controlAt08)};
    if (out.controlAt08) ++out.controlAt08->referenceCountAt04;
    return out;
}
void Assign(Slot&, const Slot*);
void* Root(unsigned ensureSite, unsigned readSite);
void* Draw(unsigned ensureSite, unsigned readSite);
void* System(unsigned site);
void Cue(unsigned globalAddress, int channel, bool wrapped, unsigned environmentSite, unsigned ensureSite, unsigned readSite);
RECT ButtonRect(Button&);
void Blit(Surface* target, Surface* source, int x, int y, const RECT* clip = nullptr);
void DrawButton(Button&, const RECT*);
void CompositeButton(Button&, Surface*, const RECT*);
void AppendChild(ModeButtonReferenceVector&, const Slot&);
void AdoptButton(Slot&, Button*);
void AdoptPageArray(Slot&, Button*, int);
void DestroyDerived(Body&) noexcept;
const CgButtonNativeVtable15& IntermediateMethods() noexcept;

// 表均直接核验原PE；字符串是CP932字节，不依赖宿主源文件编码。
extern const char* const backgrounds[10];
extern const char* const pageMarks[3];
extern const char* const stamps[4];
extern const char* const digits[10];
extern const char* const percentDigits[10];
extern const char* const formats[3];
extern const CgUiButtonRecordNative28h pageRecords[2];
extern const POINT positions[9];
extern const POINT slashes[9];
extern const int oneX[9][2];
extern const int twoX[9][4];
extern const int threeX[9][6];
extern const int numberY[9][2];
extern const POINT percentPositions[3];
extern const int neighbors[27][4];
struct UnwindEntry final { int previous; unsigned action; };
extern const UnwindEntry constructorUnwind[38];
}
