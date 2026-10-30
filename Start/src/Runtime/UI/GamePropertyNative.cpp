#include <Runtime/GamePropertyNative.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <exception>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local GamePropertyNativeServices* current;
constinit DWORD buttonsAt6197EC=0;
template<class T>T& Part(GamePropertyNativeBody33Ch& body,DWORD offset)noexcept
{return *reinterpret_cast<T*>(reinterpret_cast<BYTE*>(&body)+offset);}
BOOL __fastcall Process(EditWindowEndpointNativeBody24h* b,void*,HWND w,UINT m,WPARAM p,LPARAM l,LRESULT* r,DWORD map)
{return ProcessGamePropertyNativeAt42E590(*reinterpret_cast<GamePropertyNativeBody33Ch*>(b),w,m,p,l,r,map,RequireGamePropertyNativeServices().sheet);}
EditWindowEndpointNativeBody24h* __fastcall Delete(EditWindowEndpointNativeBody24h* b,void*,DWORD flags)
{
    auto& s=RequireGamePropertyNativeServices();DestroyGamePropertyNativeAt40E840(*reinterpret_cast<GamePropertyNativeBody33Ch*>(b),s);
    if(flags&1u)s.sheet.endpoint.memory.DeleteBody(b,0x33C);return b;
}
WNDPROC __fastcall Procedure(EditWindowEndpointNativeBody24h*,void*){return &BoundPropertySheetNativeAt439E40;}
void __fastcall Final(EditWindowEndpointNativeBody24h*,void*,HWND){} // 原424DE0为ret4空叶。
// 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
// 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
// flags 的 DWORD/uint32_t 在此 x86 ABI 中均为无符号 32 位栈参数。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const EditWindowEndpointNativeVtable4 table{
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::processAt00)>(&Process),
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::deleteAt04)>(&Delete),
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::procedureAt08)>(&Procedure),
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::finalMessageAt0C)>(&Final)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
void Unwind(GamePropertyNativeBody33Ch& body,int state,GamePropertyNativeServices& s)noexcept
{
    if(state>=6)DestroyPropertyPageSystem2NativeAt40EBF0(GamePropertySystem2Native(body),s.sheet.endpoint);
    if(state>=5)DestroyPropertyPageSystem1NativeAt40EB90(GamePropertySystem1Native(body),s.sheet.endpoint);
    if(state>=4)
    {
        DestroyPropertyPageSaveLoadNativeAt40EB00(GamePropertySaveLoadNative(body),s.sheet.endpoint);
        DestroyPropertyPageSoundNativeAt40EB10(GamePropertySoundNative(body),s.sound);
        DestroyPropertyPageMessWinNativeAt411D60(GamePropertyMessWinNative(body),s.text);
        DestroyPropertyPageTextNativeAt411F10(GamePropertyTextNative(body),s.text);
    }
    if(state>=0)ReleasePropertySheetBaseNativeAt40CCB0(body.sheetAt00,s.sheet);
}
}
GamePropertyNativeScope::GamePropertyNativeScope(GamePropertyNativeServices& s)noexcept
    :previous_(current),sheet_(s.sheet),text_(s.text),sound_(s.sound),system_(s.system){current=&s;}
GamePropertyNativeScope::~GamePropertyNativeScope()noexcept{current=previous_;}
GamePropertyNativeServices& RequireGamePropertyNativeServices()
{if(!current)throw std::logic_error("native CGameProperty requires all six page services");return *current;}
const EditWindowEndpointNativeVtable4& GamePropertyNativeVtable()noexcept{return table;}
DWORD& ProcessPropertySheetButtonsAt6197EC()noexcept{return buttonsAt6197EC;}
PropertyPageTextNativeBody68h& GamePropertyTextNative(GamePropertyNativeBody33Ch& b)noexcept{return Part<PropertyPageTextNativeBody68h>(b,0x64);}
PropertyPageMessWinNativeBody9Ch& GamePropertyMessWinNative(GamePropertyNativeBody33Ch& b)noexcept{return Part<PropertyPageMessWinNativeBody9Ch>(b,0xCC);}
PropertyPageSoundNativeBodyA8h& GamePropertySoundNative(GamePropertyNativeBody33Ch& b)noexcept{return Part<PropertyPageSoundNativeBodyA8h>(b,0x168);}
PropertyPageSaveLoadNativeBody5Ch& GamePropertySaveLoadNative(GamePropertyNativeBody33Ch& b)noexcept{return Part<PropertyPageSaveLoadNativeBody5Ch>(b,0x210);}
PropertyPageSystem1NativeBody60h& GamePropertySystem1Native(GamePropertyNativeBody33Ch& b)noexcept{return Part<PropertyPageSystem1NativeBody60h>(b,0x26C);}
PropertyPageSystem2NativeBody68h& GamePropertySystem2Native(GamePropertyNativeBody33Ch& b)noexcept{return Part<PropertyPageSystem2NativeBody68h>(b,0x2CC);}
GamePropertyNativeBody33Ch* ConstructGamePropertyNativeAt40B860(GamePropertyNativeBody33Ch& body,DWORD selected,GamePropertyNativeServices& s)
{
    GamePropertyNativeScope scope(s);int state=-1;
    InitializePropertySheetBaseNativeAt40B88D(body.sheetAt00,table,s.module);state=0;
    try
    {
        ::new(&GamePropertyTextNative(body))PropertyPageTextNativeBody68h;
        InitializePropertyPageTextNativeAt40B91C(GamePropertyTextNative(body),s.module);
        ::new(&GamePropertyMessWinNative(body))PropertyPageMessWinNativeBody9Ch;
        InitializePropertyPageMessWinNativeAt40B9C0(GamePropertyMessWinNative(body),s.module);
        ::new(&GamePropertySoundNative(body))PropertyPageSoundNativeBodyA8h;
        InitializePropertyPageSoundNativeAt40BADD(GamePropertySoundNative(body),s.module);
        ::new(&GamePropertySaveLoadNative(body))PropertyPageSaveLoadNativeBody5Ch;
        InitializePropertyPageSaveLoadNativeAt40BBE6(GamePropertySaveLoadNative(body),s.module);state=4;
        ::new(&GamePropertySystem1Native(body))PropertyPageSystem1NativeBody60h;
        InitializePropertyPageSystem1NativeAt40BC89(GamePropertySystem1Native(body),s.module,s.strings);state=5;
        ::new(&GamePropertySystem2Native(body))PropertyPageSystem2NativeBody68h;
        // 第二nil失败时state仍5，原没有System2第一串的清理资格。
        InitializePropertyPageSystem2NativeAt40BD0F(GamePropertySystem2Native(body),s.module,s.strings);state=6;
        s.sheetButtonsAt6197EC=0;
        for(const DWORD offset:{0x64u,0xCCu,0x168u,0x210u,0x26Cu,0x2CCu})
        {
            auto& page=Part<PropertyPageNativeBody54h>(body,offset);
            static_cast<void>(AddPropertySheetPageNativeAt4129C0(body.sheetAt00,&page.headerAt20,s.sheet));
        }
        auto& sheet=body.sheetAt00;const auto window=*static_cast<const volatile HWND*>(&sheet.endpointAt00.windowAt04);
        static constexpr char caption[]="\x90\xDD\x92\xE8"; // 原PE5AE134，4个CP932字节。
        HWND currentWindow;
        if(!window)
        {sheet.headerAt24.flagsAt04&=~DWORD{1};sheet.headerAt24.captionAt14=caption;currentWindow=nullptr;}
        else
        {
            static_cast<void>(s.sheet.api.SendMessageA(window,0x46F,0,reinterpret_cast<LPARAM>(caption)));
            currentWindow=*static_cast<const volatile HWND*>(&sheet.endpointAt00.windowAt04);
        }
        sheet.headerAt24.flagsAt04|=0x80;
        if(!currentWindow)sheet.headerAt24.startAt1C=selected;
        else static_cast<void>(s.sheet.api.SendMessageA(currentWindow,0x465,selected,0));
        body.lastPageAt334=-1;body.centeredAt338=0;return &body;
    }
    catch(...){Unwind(body,state,s);throw;}
}
void DestroyGamePropertyNativeAt40E840(GamePropertyNativeBody33Ch& body,GamePropertyNativeServices& s)noexcept
{
    GamePropertyNativeScope scope(s);body.sheetAt00.endpointAt00.vptrAt00=&table;
    // 40E86F..40E929：System2(后串先)、System1、Save、Sound、Image/Mess、Text、sheet基类。
    Unwind(body,6,s);
}
}
