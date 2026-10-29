#pragma once
#include <Runtime/PropertyPageTextNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/RuntimeCStringInputNative.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime::PropertyPageTextDetail
{
template<class T>T Read(const T& v)noexcept{return *static_cast<const volatile T*>(&v);}
template<class T>T& Field(void* p,DWORD offset)noexcept
{return *reinterpret_cast<T*>(reinterpret_cast<DWORD>(p)+offset);}
enum class RefOrder { StrideBias,BiasStride };
template<class T>bool ResolvePresent(const YtlReferenceSlotRuntime& slot,T*& output,RefOrder order=RefOrder::StrideBias)noexcept
{
    const auto base=Read(slot.baseAt00);if(!base){output=nullptr;return false;}
    const auto* control=Read(slot.controlAt08);DWORD bias,stride=0;
    if(order==RefOrder::BiasStride)
    {
        bias=Read(slot.biasAt04);
        if(control)stride=Read(Field<DWORD>(const_cast<YtlReferenceControlRuntime*>(control),0x0C));
    }
    else
    {
        if(control)stride=Read(Field<DWORD>(const_cast<YtlReferenceControlRuntime*>(control),0x0C));
        bias=Read(slot.biasAt04);
    }
    output=reinterpret_cast<T*>(base+bias*stride);return true;
}
template<class T>T* Resolve(const YtlReferenceSlotRuntime& slot,RefOrder order=RefOrder::StrideBias)noexcept
{T* output;ResolvePresent(slot,output,order);return output;}
// inline站点大多end-first；42805E/4280E5各读begin两次，不能套417990。
enum class EnvOrder { EndBegin,BeginEndBegin };
void* InlineArchive(PropertyPageTextNativeServices&,DWORD,RefOrder=RefOrder::StrideBias,EnvOrder=EnvOrder::EndBegin);
void* LookupArchive(PropertyPageTextNativeServices&,DWORD);
LRESULT SendItem(const HWND*,int,UINT,WPARAM,LPARAM,PropertyPageTextNativeServices&);
void Invalidate(const HWND*,HWND,PropertyPageTextNativeServices&);
void SetNumber(const HWND*,int,int,PropertyPageTextNativeServices&);
DWORD TextColor(const HWND*,PropertyPageTextNativeServices&);
DWORD MessColor(const HWND*,PropertyPageTextNativeServices&);
void LoadTextColor(PropertyPageTextNativeBody68h&,DWORD,BOOL,PropertyPageTextNativeServices&);
void InitText(PropertyPageTextNativeBody68h&,PropertyPageTextNativeServices&);
void InitMess(PropertyPageMessWinNativeBody9Ch&,PropertyPageTextNativeServices&);
void EditColor(const HWND*,WORD,HWND,PropertyPageImageStaticNativeBody48h*,PropertyPageTextNativeServices&);
void DefaultMess(PropertyPageMessWinNativeBody9Ch&,PropertyPageTextNativeServices&);
void RefreshMess(PropertyPageMessWinNativeBody9Ch&,PropertyPageTextNativeServices&);
void SheetButtons(HWND,PropertyPageTextNativeServices&);
FARPROC GraphicsExport(const char*,int,PropertyPageTextNativeServices&,bool&);
void ClearSurface(GraphicsSurfaceNativeBody*,PropertyPageTextNativeServices&);
// 40E990在payload回调后重读slot.control；逐站点保留原读取，不缓存control到删除槽。
void ReleaseImageReference(YtlReferenceSlotRuntime&);
}
