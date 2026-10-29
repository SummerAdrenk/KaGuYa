#include "CgUiButtonNative.Internal.h"
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/CgUiButtonSurfaceAppendNative.h>
#include <mbstring.h>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
using namespace CgUiButtonNativeDetail;
namespace
{
void MergeSurface(CgUiButtonNativeBody6Ch& body,const YtlReferenceSlotRuntime& reference)
{
    if(!Count(body.surfacesAt5C))
    {
        auto* surface=Resolve<GraphicsSurfaceNativeBody>(reference);
        body.extentAt38.cx=Read(surface->widthAt08);body.extentAt38.cy=Read(surface->heightAt0C);
        surface=Resolve<GraphicsSurfaceNativeBody>(reference);
        body.naturalAt30.x=Read(surface->originXAt10);body.naturalAt30.y=Read(surface->originYAt14);
    }
    else
    {
        auto* surface=Resolve<GraphicsSurfaceNativeBody>(reference);
        const auto width=Read(surface->widthAt08);const auto height=Read(surface->heightAt0C);
        surface=Resolve<GraphicsSurfaceNativeBody>(reference);
        const auto x=Read(surface->originXAt10);const auto y=Read(surface->originYAt14);
        RECT incoming{x,y,Add(x,width),Add(y,height)};
        RECT current{Read(body.naturalAt30.x),Read(body.naturalAt30.y),
            Add(Read(body.naturalAt30.x),Read(body.extentAt38.cx)),
            Add(Read(body.naturalAt30.y),Read(body.extentAt38.cy))};
        ::UnionRect(&incoming,&incoming,&current);
        body.extentAt38.cy=Sub(incoming.bottom,incoming.top);body.naturalAt30.y=incoming.top;
        body.extentAt38.cx=Sub(incoming.right,incoming.left);body.naturalAt30.x=incoming.left;
    }
}
}
std::uint8_t AttachCgUiButtonSurfaceNativeAt4C1F20(CgUiButtonNativeBody6Ch& body,std::uint32_t slot,
    const char* name,const YtlReferenceSlotRuntime& reference,CgUiButtonNativeServices& services)
{
    // PE FuncInfo5EDAFC保护state0..1，catch-all返回AL0；不外传新的现代range错误。
    try
    {
        if(slot>=7u)throw 1;
        auto identifier=ArchiveResourceNameAt40B530(name,services.strings);
        std::int32_t index=0;
        while(std::uint32_t(index)<std::uint32_t(Count(body.surfacesAt5C)))
        {
            auto* surface=ResolveCgUiButtonSurfaceNativeAt4ADED0(body,index);
            const auto* existing=*reinterpret_cast<const char* const volatile*>(&surface->nameAt1C);
            if(!existing)throw HRESULT{E_FAIL};
            if(!_mbsicmp(reinterpret_cast<const unsigned char*>(identifier.Value().GetString()),
                reinterpret_cast<const unsigned char*>(existing)))break;
            ++index;
        }
        if(std::uint32_t(index)>=std::uint32_t(Count(body.surfacesAt5C)))
        {
            auto* surface=Resolve<GraphicsSurfaceNativeBody>(reference);
            SetRuntimeCStringAt437680(surface->nameAt1C,name,name?static_cast<int>(std::strlen(name)):0);
            index=Count(body.surfacesAt5C);MergeSurface(body,reference);
            AppendCgUiButtonSurfaceReferenceAt499920(body.surfacesAt5C,reference);
        }
        body.slotsAt40[slot]=index;
        // 原normal尾先撤state1再Free，抛出后catch不能再次减同一name。
        identifier.ReleaseNormal();return 1;
    }
    catch(...){return 0;}
}
}
