#include "ProgramSceneNative.Internal.h"
#include <Runtime/SceneTaskRunNative.h>
#include <exception>
#include <stdexcept>

extern "C" int __cdecl _purecall();
namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace ProgramSceneNativeDetail;
thread_local ProgramSceneNativeServices* active=nullptr;
void __fastcall Update(Body* b,void*){UpdateProgramSceneNativeAt4B9710(*b,RequireProgramSceneNativeServices());}
Body* __fastcall Delete(Body* b,void*,unsigned flags)noexcept{return DeleteProgramSceneNativeAt4A6ED0(b,flags);}
std::uint8_t __fastcall Pure(Body*,void*,int){_purecall();std::terminate();}
Body* __fastcall DeleteTask(Body* b,void*,unsigned flags)noexcept;
struct Retired{decltype(ProgramSceneNativeVtable4::runAt00) run;decltype(ProgramSceneNativeVtable4::deleteAt04) destroy;};
// 原生x86虚槽桥接：对象走ECX，额外void*只占EDX且不消费，其余参数/返回与原槽对应。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const Retired retired{reinterpret_cast<decltype(retired.run)>(&Pure),reinterpret_cast<decltype(retired.destroy)>(&DeleteTask)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
// 原生x86虚槽桥接：对象走ECX，额外void*只占EDX且不消费，其余参数/返回与原槽对应。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const ProgramSceneNativeVtable4 methods{
    reinterpret_cast<decltype(methods.runAt00)>(&RunSceneTaskNativeAt4C3630),
    reinterpret_cast<decltype(methods.deleteAt04)>(&Delete),reinterpret_cast<decltype(methods.updateAt08)>(&Update),
    reinterpret_cast<decltype(methods.drawAt0C)>(&DrawSceneTaskNativeAt4B5E20)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
Body* __fastcall DeleteTask(Body* b,void*,unsigned flags)noexcept
{Retire(*b);if(flags&1u)::operator delete(b,std::size_t{4});return b;}
}
ProgramSceneNativeScope::ProgramSceneNativeScope(ProgramSceneNativeServices& s)noexcept
    :previous_(active),confirmation_(s.confirmation),gui_(s.gui){active=&s;}
ProgramSceneNativeScope::~ProgramSceneNativeScope()noexcept{active=previous_;}
ProgramSceneNativeServices& RequireProgramSceneNativeServices()
{if(!active)throw std::logic_error("Program native scene services are required");return *active;}
const ProgramSceneNativeVtable4& ProgramSceneNativeMethods()noexcept{return methods;}
namespace ProgramSceneNativeDetail
{
void Retire(Body& b)noexcept{b.vptrAt00=reinterpret_cast<const ProgramSceneNativeVtable4*>(&retired);}
void ReleaseNormal(Slot& slot)noexcept{ReleaseYtlReference(slot);}
void ReleaseRetainedNormal(Slot& slot)noexcept
{
    // Update的内联正常尾不清局部Ref；仍在payload后重读当前control，不退到旧缓存。
    auto* control=Read(slot.controlAt08);if(!control)return;
    if(--control->referenceCountAt04==0)
    {
        if(Read(control->owningAt08))control->DestroyPayloadAtVslot00();
        if(auto* current=Read(slot.controlAt08))current->DeleteControlAtVslot04(1);
    }
}
UpdateFrame::~UpdateFrame()noexcept
{
    switch(state)
    {
    case 0:case 3:case 6:case 8:case 9:DestroyRuntimeCStringAt40CD40(&text.value);break;
    case 1:case 4:case 7:case 10:case 13:ReleaseNormal(first);break;
    case 11:case 14:ReleaseNormal(argument);ReleaseNormal(first);break;
    case 16:memory.DeleteBody(rawConfirmation,0x84);break;
    case 17:ReleaseNormal(confirmation);break;
    case 18:ReleaseNormal(second);ReleaseNormal(confirmation);break;
    case 2:case 5:case 12:case 15:case 19:case 20:std::terminate();
    default:break;
    }
}
}
void DestroyProgramSceneNativeAt4A6ED0(Body& b)noexcept{ReleaseYtlReference(b.dialogAt08);Retire(b);}
Body* DeleteProgramSceneNativeAt4A6ED0(Body* b,unsigned flags)noexcept
{DestroyProgramSceneNativeAt4A6ED0(*b);if(flags&1u)::operator delete(b,std::size_t{0x20});return b;}
const char* ProgramSceneNativeName(unsigned address)
{
    switch(address)
    {
    case 0x5AE218:return "\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA";
    case 0x5AE224:return "\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA";
    case 0x5AE8AC:return "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75";
    case 0x5AE8BC:return "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68";
    case 0x5B4E68:return "\x90\xDD\x92\xE8\x89\xE6\x96\xCA";
    case 0x5B4E74:return "\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xD6";
    case 0x5B4E80:return "\x8D\x44\x8A\xB4\x93\x78\x89\xE6\x96\xCA";
    case 0x5B4DB0:return "\x83\x8D\x81\x5B\x83\x68\x91\x49\x91\xF0\x8C\xE3\x8F\x88\x97\x9D";
    default:throw std::logic_error("Unknown Program original name address");
    }
}
}
