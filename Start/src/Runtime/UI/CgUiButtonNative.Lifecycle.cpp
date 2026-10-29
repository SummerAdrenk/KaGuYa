#include "CgUiButtonNative.Internal.h"
#include <new>
#include <stdexcept>
#include <cstring>

extern "C" int __cdecl _purecall();
namespace KaGuYa::Start::Runtime
{
namespace
{
using Body=CgUiButtonNativeBody6Ch;
using namespace CgUiButtonNativeDetail;
thread_local CgUiButtonNativeServices* current=nullptr;
Body* __fastcall Deleting(Body* body,void*,std::uint32_t flags) noexcept
{return DeleteCgUiButtonNativeAt4669E0(body,flags);}
Body* __fastcall BaseDeleting(Body* body,void*,std::uint32_t flags) noexcept
{
    // 469840只有scalar delete68，不套派生4669E0的flags&2数组分支。
    DestroyCgUiButtonNativeAt463160(*body);
    if(flags&1u)::operator delete(body,std::size_t{0x68});return body;
}
std::uint8_t __fastcall PureByte(Body*,void*){return std::uint8_t(_purecall());}
POINT* __fastcall PurePoint(Body*,void*,POINT*){return reinterpret_cast<POINT*>(_purecall());}
std::uint8_t __fastcall Timeline(Body* body,void*)
{
    if(Read(body->timelineAt68)==-1)return 0;
    auto* root=Services().input.CaptureAt418210();
    auto* keyboard=Resolve<KeyboardManagerNativeBody438h>(root->keyboardAt18);
    const auto index=Read(body->timelineAt68);
    const auto address=reinterpret_cast<std::uintptr_t>(keyboard)+0x14u+std::uint32_t(index)*12u;
    auto* descriptor=reinterpret_cast<EnvironmentVectorStorage<KeyboardBindingNative3>*>(address);
    auto* at=Read(descriptor->begin);
    if(at==Read(descriptor->end))return 0;
    const auto buffer=Read(keyboard->bufferAt10);
    while(at!=Read(descriptor->end))
    {
        if(Read(*reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uintptr_t>(at)+buffer+1u)))return 1;
        at=reinterpret_cast<KeyboardBindingNative3*>(reinterpret_cast<std::uintptr_t>(at)+3u);
    }
    return 0;
}
POINT* __fastcall Pointer(Body*,void*,POINT* out)
{
    auto* root=Services().input.CaptureAt418210();auto* mouse=Resolve<MouseManagerNativeBody3Ch>(root->mouseAt00);
    out->x=Read(mouse->logicalAt18.x);out->y=Read(mouse->logicalAt18.y);return out;
}
std::uint8_t __fastcall Press(Body*,void*)
{
    auto* root=Services().input.CaptureAt418210();auto* mouse=Resolve<MouseManagerNativeBody3Ch>(root->mouseAt00);
    return !Read(mouse->previousAt30[0])&&Read(mouse->pressedAt10[0])&&!Read(mouse->suppressionAt28);
}
std::uint8_t __fastcall Release(Body*,void*)
{
    auto* root=Services().input.CaptureAt418210();auto* mouse=Resolve<MouseManagerNativeBody3Ch>(root->mouseAt00);
    return !Read(mouse->pressedAt10[0])||Read(mouse->suppressionAt28)!=0;
}
const CgUiButtonNativeVtable5 baseMethods{
    reinterpret_cast<decltype(CgUiButtonNativeVtable5::deleteAt00)>(&BaseDeleting),
    reinterpret_cast<decltype(CgUiButtonNativeVtable5::timelineAt04)>(&PureByte),
    reinterpret_cast<decltype(CgUiButtonNativeVtable5::pointerAt08)>(&PurePoint),
    reinterpret_cast<decltype(CgUiButtonNativeVtable5::pressAt0C)>(&PureByte),
    reinterpret_cast<decltype(CgUiButtonNativeVtable5::releaseAt10)>(&PureByte)};
const CgUiButtonNativeVtable5 derived{
    reinterpret_cast<decltype(CgUiButtonNativeVtable5::deleteAt00)>(&Deleting),
    reinterpret_cast<decltype(CgUiButtonNativeVtable5::timelineAt04)>(&Timeline),
    reinterpret_cast<decltype(CgUiButtonNativeVtable5::pointerAt08)>(&Pointer),
    reinterpret_cast<decltype(CgUiButtonNativeVtable5::pressAt0C)>(&Press),
    reinterpret_cast<decltype(CgUiButtonNativeVtable5::releaseAt10)>(&Release)};
}
CgUiButtonNativeServices& CgUiButtonNativeDetail::Services()
{if(!current)throw std::logic_error("CGUIButton native call has no bound services");return *current;}
CgUiButtonNativeScope::CgUiButtonNativeScope(CgUiButtonNativeServices& services) noexcept:previous_(current){current=&services;}
CgUiButtonNativeScope::~CgUiButtonNativeScope() noexcept{current=previous_;}
const CgUiButtonNativeVtable5& CgUiButtonNativeVtable() noexcept{return derived;}
const CgUiButtonNativeVtable5& GuiButtonGraphicsBaseNativeVtable() noexcept{return baseMethods;}
Body* ConstructCgUiButtonNativeAt459530(Body& body)
{
    body.vptrAt00=&baseMethods;body.hitRectAt0C={};body.positionAt24={};body.naturalAt30={};body.extentAt38={};
    body.surfacesAt5C={};body.pixelHitAt08=1;body.stateAt04=-1;body.inputModeAt1C=0;
    body.latchAt20=0;body.useNaturalAt2C=1;
    for(auto& slot:body.slotsAt40)slot=-1;
    ReleaseGuiControlSurfaceRangeAt43EA10(body.surfacesAt5C.begin,body.surfacesAt5C.end);
    body.surfacesAt5C.end=body.surfacesAt5C.begin;body.vptrAt00=&derived;body.timelineAt68=-1;return &body;
}
void DestroyCgUiButtonNativeAt463160(Body& body) noexcept
{body.vptrAt00=&baseMethods;DestroyGuiControlSurfaceReferencesAt43EAA0(body.surfacesAt5C);}
Body* DeleteCgUiButtonNativeAt4669E0(Body* body,std::uint32_t flags) noexcept
{
    if(flags&2u)
    {
        auto* raw=reinterpret_cast<std::byte*>(body)-4;
        const auto count=Read(*reinterpret_cast<std::uint32_t*>(raw));
        for(auto at=count;at;--at)DestroyCgUiButtonNativeAt463160(body[at-1u]);
        if(flags&1u)::operator delete[](raw,std::uint32_t(count*0x6Cu+4u));
        return reinterpret_cast<Body*>(raw);
    }
    DestroyCgUiButtonNativeAt463160(*body);
    if(flags&1u)::operator delete(body,std::size_t{0x6C});return body;
}
GraphicsSurfaceNativeBody* ResolveCgUiButtonSurfaceNativeAt4ADED0(Body& body,std::int32_t index) noexcept
{
    if(index<0)return nullptr;
    auto* const begin=Read(body.surfacesAt5C.begin);
    const auto end=reinterpret_cast<std::uintptr_t>(Read(body.surfacesAt5C.end));
    const auto count=std::bit_cast<std::int32_t>(std::uint32_t(end-reinterpret_cast<std::uintptr_t>(begin)))/12;
    if(index>=count)return nullptr;
    auto* slot=reinterpret_cast<YtlReferenceSlotRuntime*>(reinterpret_cast<std::uintptr_t>(begin)+std::uint32_t(index)*12u);
    return Resolve<GraphicsSurfaceNativeBody>(*slot);
}
}
