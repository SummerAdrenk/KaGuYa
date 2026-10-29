#include "LogSceneNative.Internal.h"
#include <exception>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace LogSceneNativeDetail;
thread_local LogSceneNativeServices* active=nullptr;
__declspec(naked) std::uint8_t __fastcall Run(Body*,void*,std::int32_t)
{
    __asm {
        push esi
        mov esi,ecx
        mov eax,[esi]
        call dword ptr[eax+8]
        mov eax,[esi]
        mov ecx,esi
        call dword ptr[eax+0Ch]
        mov al,1
        pop esi
        ret 4
    }
}
__declspec(naked) void __fastcall Draw(Body*,void*){__asm{_emit 0xC3}}
void __fastcall Update(Body* p,void*){UpdateLogSceneNativeAt4B6900(*p,RequireLogSceneNativeServices());}
Body* __fastcall Delete(Body* p,void*,unsigned flags) noexcept{return DeleteLogSceneNativeAt4A6BA0(p,flags);}
std::uint8_t __fastcall Pure(Body*,void*,std::int32_t){_purecall();std::terminate();}
Body* __fastcall DeleteTask(Body*,void*,unsigned) noexcept;
struct Retired final{decltype(LogSceneNativeVtable4::runAt00) run;decltype(LogSceneNativeVtable4::deleteAt04) destroy;};
const Retired retired{reinterpret_cast<decltype(Retired::run)>(&Pure),reinterpret_cast<decltype(Retired::destroy)>(&DeleteTask)};
const LogSceneNativeVtable4 methods{
    reinterpret_cast<decltype(LogSceneNativeVtable4::runAt00)>(&Run),reinterpret_cast<decltype(LogSceneNativeVtable4::deleteAt04)>(&Delete),
    reinterpret_cast<decltype(LogSceneNativeVtable4::updateAt08)>(&Update),reinterpret_cast<decltype(LogSceneNativeVtable4::drawAt0C)>(&Draw)};
Body* __fastcall DeleteTask(Body* p,void*,unsigned flags) noexcept
{LogSceneNativeDetail::RetireTask(*p);if(flags&1)::operator delete(p,std::size_t{4});return p;}
}
namespace LogSceneNativeDetail{void RetireTask(Body& b) noexcept{b.vptrAt00=reinterpret_cast<const LogSceneNativeVtable4*>(&retired);}}
LogSceneNativeScope::LogSceneNativeScope(LogSceneNativeServices& s) noexcept:previous_(active)
{
    if(auto* gui=s.source.NativeDialogServices())dialog_.emplace(*gui);
    active=&s;
}
LogSceneNativeScope::~LogSceneNativeScope() noexcept{active=previous_;}
LogSceneNativeServices& RequireLogSceneNativeServices()
{if(!active)throw std::logic_error("Log native scene services are not bound");return *active;}
const LogSceneNativeVtable4& LogSceneNativeMethods() noexcept{return methods;}
void DestroyLogSceneNativeAt4A6BA0(LogSceneNativeBody20h& b) noexcept
{ReleaseYtlReference(b.dialogAt10);LogSceneNativeDetail::RetireTask(b);}
LogSceneNativeBody20h* DeleteLogSceneNativeAt4A6BA0(LogSceneNativeBody20h* p,unsigned flags) noexcept
{DestroyLogSceneNativeAt4A6BA0(*p);if(flags&1)::operator delete(p,std::size_t{0x20});return p;}
std::uint8_t RunLogSceneNativeAt4C3630(LogSceneNativeBody20h& b,std::int32_t argument,LogSceneNativeServices& s)
{LogSceneNativeScope scope(s);return LogSceneNativeDetail::Read(b.vptrAt00)->runAt00(&b,argument);}
}
