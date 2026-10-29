#pragma once
#include <Runtime/LogDialogNative.h>
#include <Runtime/GraphicsSurfaceNativeBody.h>
#include <Runtime/RuntimeCStringVector.h>
#include <Runtime/RuntimeCStringArray.h>
#include <Runtime/SoundManagerNativeLifecycle.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <array>
#include <bit>
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime::LogDialogNativeDetail
{
using Common=GuiDialogNativeBody5Ch;
using Control=GuiControlNativeBody50h;
using Slot=YtlReferenceSlotRuntime;
using DC=LogDialogNativeBodyDCh;
using Text=LogTextDialogNativeBodyA0h;
using Scroll=LogScrollDialogNativeBody80h;
using Row=LogTextNativeBody30h;
template<class T>T Read(const T& p) noexcept{return *static_cast<const volatile T*>(&p);}
template<class T>T& Field(void* p,std::size_t n) noexcept{return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(p)+n);}
template<class T>T* ResolveWithBase(const Slot& r,std::uintptr_t base) noexcept
{
    if(!base)return nullptr;
    auto* c=Read(r.controlAt08);
    const auto stride=c?Read(static_cast<EnvironmentObjectReferenceControl*>(c)->strideAt0C):0u;
    return reinterpret_cast<T*>(base+Read(r.biasAt04)*stride);
}
template<class T>T* Resolve(const Slot& r) noexcept{return ResolveWithBase<T>(r,Read(r.baseAt00));}
inline Slot Copy(const Slot& s) noexcept
{
    Slot r{Read(s.baseAt00),Read(s.biasAt04),Read(s.controlAt08)};
    if(r.controlAt08)++r.controlAt08->referenceCountAt04;
    return r;
}
struct Fresh final{Slot slot;Fresh() noexcept{};};
void ReleaseCaptured(YtlReferenceControlRuntime*);
struct RefLife final
{
    Slot& slot;bool active=true;
    ~RefLife() noexcept{if(active)ReleaseYtlReference(slot);}
    void Normal(bool clear=true);
    void Captured(YtlReferenceControlRuntime*,bool clear=true);
    void TerminateNormal(bool clear=true) noexcept{Normal(clear);}
    void TerminateCaptured(YtlReferenceControlRuntime* c,bool clear=true) noexcept{Captured(c,clear);}
};
struct Raw final
{
    void* p;std::size_t bytes;bool active=true;
    ~Raw() noexcept{if(active)RequireLogDialogNativeServices().memory.DeleteBody(p,bytes);}
};
struct CStringLife final
{
    union{RuntimeCString value;};bool active=false;
    CStringLife() noexcept{}
    ~CStringLife() noexcept{if(active)DestroyRuntimeCStringAt40CD40(&value);}
    void Empty(){InitializeEmptyRuntimeCString(value,RequireLogDialogNativeServices().strings);active=true;}
    void Clone(const RuntimeCString&);
    void Normal(){active=false;DestroyRuntimeCStringAt40CD40(&value);}
    void NormalCaptured(ATL::CStringData* header){active=false;ReleaseRuntimeCStringHeader(header);}
};
inline auto& Slider(Control* p){return *reinterpret_cast<const GuiSliderNativeVtable31*>(Read(p->vptrAt00));}
inline std::int32_t Add(std::int32_t a,std::int32_t b) noexcept
{return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a)+static_cast<std::uint32_t>(b));}
inline std::int32_t Sub(std::int32_t a,std::int32_t b) noexcept
{return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a)-static_cast<std::uint32_t>(b));}
inline void Find(Common* p,Slot& out,int id){Read(p->vptrAt00)->findChildAt14(p,&out,id);}
inline int PollEvents(Slot& r)
{
    auto* manager=Resolve<GuiClickedEventReferences>(r);
    auto* at=Read(manager->begin);auto* end=Read(manager->end);
    for(;at!=end;++at)
    {
        const auto base=Read(at->baseAt00);
        if(!Read(ResolveWithBase<GuiClickedEventNativeBody>(*at,base)->clickedFlagAt04))continue;
        const auto event=Read(ResolveWithBase<GuiClickedEventNativeBody>(*at,base)->eventValueAt08);
        auto* selected=ResolveWithBase<GuiClickedEventNativeBody>(*at,base);
        Read(selected->vptrAt00)->clearAt04(selected);return event;
    }
    return -1;
}
void* Root(std::uint32_t site);
void* Registry(unsigned index,std::uint32_t site,bool ordinary=false);
ScriptParamsNativeBody* Parameters(std::uint32_t site);
SoundManagerNativeBody50h* Sound(std::uint32_t site);
void AdoptDialog(Slot&,Common*,std::uint32_t);
Row* NewRow();
void AdoptRow(Slot&,Row*);
void AppendRow(EnvironmentVectorStorage<Slot>&,const Slot&);
void DestroyRows(EnvironmentVectorStorage<Slot>&) noexcept;
void ResetRow(Row&);
void BindEvent(Common&,Slot& child,Slot& manager,int event,bool inlineControl);
// 动态profile借用允许缺绑定错误外传；不能沿用原静态表getter的noexcept。
const GuiStaticDescriptorNative24h& BackgroundDescriptor();
std::span<const GuiButtonDescriptorNative2Ch,2> ScrollButtons();
const GuiButtonDescriptorNative2Ch& VoiceButton();
const GuiSliderDescriptorNative6Ch& ScrollSlider();
std::span<const POINT,4> RowPositions();
POINT VoiceShift();
POINT FrameShift();
const char* FrameName();
const char* FontLiteral() noexcept;
const char* NamePrefix();
const char* NameSuffix();
const char* TrimCharacters() noexcept;

void __fastcall BeginDC(Common*,void*);
int __fastcall PollDC(Common*,void*);
int __fastcall EndDC(Common*,void*);
void __fastcall InputDC(Common*,void*);
void __fastcall FlushDC(Common*,void*);
void __fastcall BeginText(Common*,void*);
int __fastcall PollText(Common*,void*);
void __fastcall DrawText(Common*,void*,const RECT*);
void __fastcall CompositeText(Common*,void*,Slot,const RECT*);
void __fastcall BeginScroll(Common*,void*);
int __fastcall PollScroll(Common*,void*);
int __fastcall UpperScroll(Common*,void*);
void __fastcall InputScroll(Common*,void*);
int ScrollPosition(Common*);
void MoveScroll(Common*,int delta);
void UpdateRows(Text&,int position,int redraw);
void UpdateVoiceButton(Text&,int row);
void RenderRow(Row&,int format);
void RebuildVoices(Slot&,int count);
RuntimeCString* VoiceAt(const Slot&,std::uint32_t index) noexcept;
void FormatName(RuntimeCString* out,RuntimeCString& input);
bool CreateFont(const std::array<int,4>&);
void DrawSurface(GraphicsSurfaceNativeBody*,GraphicsSurfaceNativeBody*,int,int,const RECT*,bool composite=false);
}
