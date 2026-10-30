#include <Runtime/MainFrameMenuNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local MainFrameMenuNativeServices* current;
template<class T>T Read(const void* p,DWORD offset=0)noexcept
{return *reinterpret_cast<const volatile T*>(reinterpret_cast<DWORD>(p)+offset);}
template<class T>void Write(void* p,DWORD offset,T value)noexcept
{*reinterpret_cast<volatile T*>(reinterpret_cast<DWORD>(p)+offset)=value;}
void* ResolveInline(const void* ref)noexcept
{
    const auto base=Read<DWORD>(ref);if(!base)return nullptr;
    const auto* control=Read<const void*>(ref,8);const auto stride=control?Read<DWORD>(control,0x0C):0u;
    return reinterpret_cast<void*>(base+Read<DWORD>(ref,4)*stride);
}
void* InlineArchive(IMainFrameMenuNativeSource& source,DWORD index)
{
    auto* e=source.EnvironmentAt4180C0();const auto end=Read<DWORD>(e,0x34),begin=Read<DWORD>(e,0x30);
    const auto count=static_cast<DWORD>(std::bit_cast<std::int32_t>(end-begin)/12);
    return count>index?ResolveInline(reinterpret_cast<const void*>(begin+12u*index)):nullptr;
}
void* Root(IMainFrameMenuNativeSource& source){source.EnsureRootAt413BE0();return source.RootAt6198C4();}
SceneManagerNativeBody48h* InlineScenes(IMainFrameMenuNativeSource& source)
{return static_cast<SceneManagerNativeBody48h*>(ResolveInline(&source.ScenesAt30(Root(source))));}
SceneManagerNativeBody48h* GetterScenesAt4186D0(IMainFrameMenuNativeSource& source,void* root)
{
    const auto* ref=&source.ScenesAt30(root);if(!Read<DWORD>(ref))return nullptr;
    const auto* control=Read<const void*>(ref,8);const auto stride=control?Read<DWORD>(control,0x0C):0u;
    const auto bias=Read<DWORD>(ref,4);
    // 4186E9/4186F5再次读取base；薄包装不能用内联固定base的版本代替getter。
    return reinterpret_cast<SceneManagerNativeBody48h*>(bias*stride+Read<DWORD>(ref));
}
void Pending(SceneManagerNativeBody48h* manager,const char* bytes,int length)
{SetRuntimeCStringAt437680(*reinterpret_cast<RuntimeCString*>(reinterpret_cast<DWORD>(manager)+0x1Cu),bytes,length);}
struct Command{WORD id;const char* bytes;int length;};
constexpr Command sceneCommands[]{
    {0x8018,"\x82\x62\x82\x66\x95\x5C\x8E\xA6\x83\x82\x81\x5B\x83\x68",0x0E},
    {0x8019,"\x83\x56\x81\x5B\x83\x93\x8D\xC4\x90\xB6\x83\x82\x81\x5B\x83\x68",0x10},
    {0x801A,"\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x83\x82\x81\x5B\x83\x68",0x0E},
    {0x801B,"\x83\x5E\x83\x43\x83\x67\x83\x8B",8}};
constexpr Command navigationCommands[]{
    {0x8020,"\x83\x81\x83\x6A\x83\x85\x81\x5B\x95\x5C\x8E\xA6",0x0C},
    {0x8021,"\x8E\x9F\x82\xD6",4},
    {0x8022,"\x91\x4F\x82\xD6",4},
    {0x8023,"\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x8F\xC1\x8B\x8E",0x0E},
    {0x8024,"\x96\xDF\x82\xE9",4}};
template<std::size_t N>LRESULT Emit(DWORD id,const Command(&commands)[N],IMainFrameMenuNativeSource& source)
{
    for(const auto& command:commands)if(static_cast<WORD>(id)==command.id)
    {Pending(GetterScenesAt4186D0(source,Root(source)),command.bytes,command.length);break;}
    return 0;
}
void GatedPending(WORD id,IMainFrameMenuNativeSource& source)
{
    if(!Read<DWORD>(InlineArchive(source,2),4))return;
    const bool load=(id&1u)==0;
    auto* unsave=ResolveEnvironmentArchiveAt417990(*source.EnvironmentAt4180C0(),2);
    if(!Read<BYTE>(unsave,load?9u:8u))return;
    if(id>=0x8009)
    {
        auto* capturedRoot=Root(source);
        auto* system=ResolveEnvironmentArchiveAt417990(*source.EnvironmentAt4180C0(),0);
        const auto index=source.ResolveNamedAt498020(system,"replay");
        if(source.ReadValueAfter47BDB0(capturedRoot,index))return;
    }
    auto* scene=InlineScenes(source);
    if(id==0x8007)Pending(scene,"\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA",0x0A);
    else if(id==0x8008)Pending(scene,"\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA",0x0A);
    else if(id==0x8009)Pending(scene,"\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75",0x0E);
    else Pending(scene,"\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68",0x0E);
}
void WindowModeCommand(WORD id,MainFrameMenuNativeServices& s)
{
    LONG style,ex;
    if(id==0x800B){style=Read<LONG>(s.source.EnvironmentAt4180C0(),0x28);ex=0;}
    else{ex=Read<LONG>(s.source.EnvironmentAt4180C0(),0x24);style=Read<LONG>(s.source.EnvironmentAt4180C0(),0x20);}
    auto* root=Root(s.source);auto* mode=static_cast<WindowModeNative2Ch*>(ResolveInline(&s.source.ModeAt0C(root)));
    const auto result=s.calls.SetModeAt438AD0(root,mode,id==0x800B?BYTE{1}:BYTE{0},style,ex);
    if(id==0x800B&&!result)
    {
        static constexpr char caption[]="\x89\xE6\x96\xCA\x95\xCF\x8D\x58\x82\xCC\x8E\xB8\x94\x73";
        static constexpr char text[]="\x83\x74\x83\x8B\x83\x58\x83\x4E\x83\x8A\x81\x5B\x83\x93\x89\xE6\x96\xCA\x82\xC9\x82\xC5\x82\xAB\x82\xDC\x82\xB9\x82\xF1\x82\xC5\x82\xB5\x82\xBD\x81\x42";
        static_cast<void>(s.api.MessageBoxA(Read<HWND>(s.source.EnvironmentAt4180C0(),0x2C),text,caption,0x10));
    }
    root=Root(s.source);mode=static_cast<WindowModeNative2Ch*>(ResolveInline(&s.source.ModeAt0C(root)));
    const auto value=Read<BYTE>(mode,4);Write<BYTE>(InlineArchive(s.source,1),5,value==0?BYTE{1}:BYTE{0});
}
BOOL __fastcall Entry(void* menu,void*,HWND w,UINT m,WPARAM p,LPARAM l,LRESULT* out,DWORD map)
{
    if(!current)throw std::logic_error("42F040 requires its explicit native menu service scope");
    auto& frame=*reinterpret_cast<MainFrameNativeBody90h*>(reinterpret_cast<DWORD>(menu)-0x54u);
    return ProcessMainFrameMenuNativeAt42F040(frame,w,m,p,l,out,map,*current);
}
// 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
// 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const MainFrameMenuNativeVtable1 table{reinterpret_cast<decltype(MainFrameMenuNativeVtable1::processAt00)>(&Entry)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
class Api final:public IMainFrameMenuNativeApi
{
public:
    HWND FindWindowA(LPCSTR c,LPCSTR n)override{return ::FindWindowA(c,n);}
    int MessageBoxA(HWND w,LPCSTR t,LPCSTR c,UINT f)override{return ::MessageBoxA(w,t,c,f);}
    BOOL PostMessageA(HWND w,UINT m,WPARAM p,LPARAM l)override{return ::PostMessageA(w,m,p,l);}
};
}
IMainFrameMenuNativeApi& DefaultMainFrameMenuNativeApi()noexcept{static Api api;return api;}
MainFrameMenuNativeScope::MainFrameMenuNativeScope(MainFrameMenuNativeServices& s)noexcept:previous_(current){current=&s;}
MainFrameMenuNativeScope::~MainFrameMenuNativeScope()noexcept{current=previous_;}
const MainFrameMenuNativeVtable1& MainFrameMenuNativeVtable()noexcept{return table;}
LRESULT ExecuteMainFrameSceneCommandNativeAt428B30(DWORD,DWORD id,LPARAM,BOOL*,IMainFrameMenuNativeSource& source)
{return Emit(id,sceneCommands,source);}
LRESULT ExecuteMainFrameNavigationCommandNativeAt423440(DWORD,DWORD id,LPARAM,BOOL*,IMainFrameMenuNativeSource& source)
{return Emit(id,navigationCommands,source);}
BOOL ProcessMainFrameMenuNativeAt42F040(MainFrameNativeBody90h& frame,HWND,UINT message,WPARAM w,LPARAM l,
    LRESULT* result,DWORD map,MainFrameMenuNativeServices& s)
{
    if(map)return FALSE;
    if(message==WM_INITMENUPOPUP)
    {
        if(s.source.PopupIndexAt208(Root(s.source))>=0)
            if(const auto window=s.api.FindWindowA("#32768",nullptr))
            {frame.popupUnwrittenAt88=0;static_cast<void>(s.calls.InstallPopupAt4387D0(frame.popupAt60,window));}
        *result=0;return TRUE;
    }
    if(message!=WM_COMMAND)return FALSE;
    const WORD id=LOWORD(w);auto* menu=static_cast<void*>(&frame.menuVptrAt54);
    // 原叶收到初始为0的map栈槽，而不是WTL宏常见的handled=TRUE；回写不决定父handled。
    BOOL leafHandled=0;LRESULT value;
    switch(id)
    {
    case 0x8007:case 0x8008:case 0x8009:case 0x800A:GatedPending(id,s.source);break;
    case 0x800B:case 0x800C:WindowModeCommand(id,s);break;
    case 0x800D:s.calls.SettingsAt428E30(menu);break;
    case 0x800E:value=s.calls.ToggleSkipAt4230E0(menu,HIWORD(w),id,l,&leafHandled);*result=value;return TRUE;
    case 0x800F:value=s.calls.ToggleAutoAt42AA70(menu,HIWORD(w),id,l,&leafHandled);*result=value;return TRUE;
    case 0x8010:Write<DWORD>(InlineArchive(s.source,2),0x1C,2);break;
    case 0x8011:
    {
        auto* unsave=InlineArchive(s.source,2);
        auto* sounds=static_cast<SoundManagerNativeBody50h*>(ResolveInline(&s.source.SoundsAt24(Root(s.source))));
        s.calls.PlayVoicesAt42C4F0(sounds,reinterpret_cast<const YtlReferenceSlotRuntime*>(reinterpret_cast<DWORD>(unsave)+0x3Cu));break;
    }
    case 0x8012:Write<DWORD>(InlineArchive(s.source,2),0x20,2);break;
    case 0x8014:s.calls.AboutAt42F84A();break;
    case 0x8015:s.calls.ResetAndQueueTitleAt47E180(InlineScenes(s.source));break;
    case 0x8016:static_cast<void>(s.api.PostMessageA(Read<HWND>(s.source.EnvironmentAt4180C0(),0x2C),WM_CLOSE,0,0));break;
    case 0x8017:value=s.calls.ActionAt42AF70(menu,HIWORD(w),id,l,&leafHandled);*result=value;return TRUE;
    case 0x8018:case 0x8019:case 0x801A:case 0x801B:
        value=ExecuteMainFrameSceneCommandNativeAt428B30(HIWORD(w),id,l,&leafHandled,s.source);*result=value;return TRUE;
    case 0x8020:case 0x8021:case 0x8022:case 0x8023:case 0x8024:
        value=ExecuteMainFrameNavigationCommandNativeAt423440(HIWORD(w),id,l,&leafHandled,s.source);*result=value;return TRUE;
    default:return FALSE;
    }
    *result=0;return TRUE;
}
}
