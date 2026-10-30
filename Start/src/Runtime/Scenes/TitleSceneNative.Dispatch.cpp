#include "TitleSceneNative.Internal.h"
#include <Runtime/RuntimeCStringResult.h>
#include <mbstring.h>
#include <cstring>
#include <exception>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace TitleSceneNativeDetail;
std::uint8_t __fastcall PureRun(Body*,void*,std::int32_t){_purecall();std::terminate();}
Body* __fastcall RetiredDelete(Body* body,void*,std::uint32_t flags) noexcept;
struct RetiredTable final{decltype(Table::runAt00) run;decltype(Table::deleteAt04) destroy;};
// 原生x86虚槽桥接：对象走ECX，额外void*只占EDX且不消费，其余参数/返回与原槽对应。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const RetiredTable retired{reinterpret_cast<decltype(RetiredTable::run)>(&PureRun),
    reinterpret_cast<decltype(RetiredTable::destroy)>(&RetiredDelete)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
Body* __fastcall RetiredDelete(Body* body,void*,std::uint32_t flags) noexcept
{Retire(*body);if(flags&1u)::operator delete(body,std::size_t{4});return body;}
__declspec(naked) std::uint8_t __fastcall RawRun(Body*,void*,std::int32_t)
{
    __asm {push esi
        mov esi,ecx
        mov eax,[esi]
        call dword ptr [eax+8]
        mov eax,[esi]
        mov ecx,esi
        call dword ptr [eax+0Ch]
        mov al,1
        pop esi
        ret 4}
}
Body* __fastcall RawDelete(Body* body,void*,std::uint32_t flags) noexcept
{return DeleteTitleSceneNativeAt4A71E0(body,flags);}
void __fastcall RawUpdate(Body* body,void*){UpdateTitleSceneNativeAt4BADA0(*body,ActiveServices());}
__declspec(naked) void __fastcall RawDraw(Body*,void*){__asm {_emit 0xC3}} // 4B5E20仅C3；MSVC的ret会编码为C2 00 00。
// 原生x86虚槽桥接：对象走ECX，额外void*只占EDX且不消费，其余参数/返回与原槽对应。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const Table methods{reinterpret_cast<decltype(Table::runAt00)>(&RawRun),
    reinterpret_cast<decltype(Table::deleteAt04)>(&RawDelete),reinterpret_cast<decltype(Table::updateAt08)>(&RawUpdate),
    reinterpret_cast<decltype(Table::drawAt0C)>(&RawDraw)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
template<class Result>Result DialogCall(Body& body,std::uint32_t offset)
{
    auto* dialog=ResolveEnvironmentObjectReference<void>(body.dialogAt08);
    auto* table=*static_cast<const std::uintptr_t* const*>(dialog);
    return reinterpret_cast<Result (__thiscall*)(void*)>(table[offset/4])(dialog);
}
ATL::CStringData* CloneCommand(TitleSceneNativeManagerReceiver receiver)
{
    auto* header=reinterpret_cast<ATL::CStringData*>(const_cast<char*>(receiver.body->commandAt1C.GetString()))-1;
    return CloneRuntimeCStringDataAt414540(header);
}
bool Same(const char* first,const char* second)
{return _mbscmp(reinterpret_cast<const unsigned char*>(first),reinterpret_cast<const unsigned char*>(second))==0;}
constexpr char load[]="\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA";
constexpr char setting[]="\x90\xDD\x92\xE8\x89\xE6\x96\xCA";
constexpr char start[]="\x83\x58\x83\x5E\x81\x5B\x83\x67.scr";
constexpr char script[]="\x83\x58\x83\x4E\x83\x8A\x83\x76\x83\x67\x8F\x88\x97\x9D";
constexpr char after[]="\x83\x8D\x81\x5B\x83\x68\x91\x49\x91\xF0\x8C\xE3\x8F\x88\x97\x9D";
constexpr char cg[]="\x82\x62\x82\x66\x95\x5C\x8E\xA6\x83\x82\x81\x5B\x83\x68";
constexpr char scene[]="\x83\x56\x81\x5B\x83\x93\x8D\xC4\x90\xB6\x83\x82\x81\x5B\x83\x68";
constexpr char music[]="\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x83\x82\x81\x5B\x83\x68";
}
namespace TitleSceneNativeDetail
{
void Retire(Body& body) noexcept{body.vptrAt00=reinterpret_cast<const Table*>(&retired);}
}
const Table& TitleSceneNativeMethods() noexcept{return methods;}
void UpdateTitleSceneNativeAt4BADA0(Body& body,TitleSceneNativeServices& services)
{
    TitleSceneNativeScope calls(services);auto& source=services.source;
    const auto phase=*static_cast<const volatile std::int32_t*>(&body.phaseAt04);
    if(phase!=0&&phase!=1)return;
    if(phase==0){source.ClearInformationDwordAfter413BE0();DialogCall<void>(body,0x44);body.phaseAt04=1;}
    if(*reinterpret_cast<const volatile std::uint32_t*>(
        reinterpret_cast<std::uintptr_t>(InlineArchive(source.CaptureEnvironmentAt4180C0(),2))+4)!=0)
    {
        bool requested;
        {
            const auto receiver=source.CaptureScenesAfter413BE0();
            // CloneCommand已取得引用；只写未构造返回槽的原生指针，不再执行CString赋值。
            RuntimeCStringResult text([&](RuntimeCString* out){const auto* chars=reinterpret_cast<const char*>(CloneCommand(receiver)+1);std::memcpy(static_cast<void*>(out),&chars,4);});
            requested=Same(text.Value().GetString(),load)||source.EvaluateLoadPredicateAfter413BE0();
        }
        // 4BB043即使predicate=false仍取根；Settings clone消费这次固定manager。
        const auto receiver=source.CaptureScenesAfter413BE0();
        if(requested)
        {
            View(receiver).WritePendingCommandAt1C({});source.WriteRootByteAfter413BE0(0x1AF,0);
            View(source.CaptureScenesAfter413BE0()).PushActiveScene(load);return;
        }
        auto* header=CloneCommand(receiver);
        const bool settingsRequested=Same(reinterpret_cast<const char*>(header+1),setting);
        // 4BB180没有state2资格，不为第二个Clone增加现代局部owner。
        ReleaseRuntimeCStringHeader(header);
        if(settingsRequested)
        {View(source.CaptureScenesAfter413BE0()).WritePendingCommandAt1C({});View(source.CaptureScenesAfter413BE0()).PushActiveScene(setting);return;}
    }
    if(DialogCall<std::int32_t>(body,0x48)!=0)return;
    const auto selected=DialogCall<std::uint32_t>(body,0x50);
    switch(selected)
    {
    case 0:
        static_cast<void>(source.LoadStartupScriptAfter413BE0(start,-1));
        View(source.CaptureScenesAfter413BE0()).QueueScene(script);
        source.WriteRootByteAfter413BE0(0x218,1);source.WriteRootByteAfter413BE0(0x219,1);return;
    case 1:View(source.CaptureScenesAfter413BE0()).ClearScenes();View(source.CaptureScenesAfter413BE0()).QueueScene(after);return;
    case 2:View(source.CaptureScenesAfter413BE0()).QueueScene(cg);return;
    case 3:View(source.CaptureScenesAfter413BE0()).QueueScene(scene);return;
    case 4:View(source.CaptureScenesAfter413BE0()).QueueScene(music);return;
    default:return;
    }
}
}
