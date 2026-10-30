#include <Runtime/AboutDialogNative.h>
#include <bit>
#include <exception>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local AboutDialogNativeServices* current;
template<class T>T Read(const void* p,DWORD offset=0)noexcept
{return *reinterpret_cast<const volatile T*>(reinterpret_cast<DWORD>(p)+offset);}
void* Resolve(const void* ref)noexcept
{
    const auto base=Read<DWORD>(ref);if(!base)return nullptr;
    const auto* control=Read<const void*>(ref,8);const auto stride=control?Read<DWORD>(control,0x0C):0u;
    return reinterpret_cast<void*>(base+Read<DWORD>(ref,4)*stride);
}
void* System(IAboutDialogNativeSource& source)
{
    auto* e=source.EnvironmentAt4180C0();const auto end=Read<DWORD>(e,0x34),begin=Read<DWORD>(e,0x30);
    // 424ED2是count==0而不是unsigned<=0；负位型不额外拒绝。
    if(std::bit_cast<std::int32_t>(end-begin)/12==0)return nullptr;
    return Resolve(reinterpret_cast<const void*>(begin));
}
AboutDialogNativeServices& Services()
{if(!current)throw std::logic_error("native About requires explicit source/endpoint services");return *current;}
BOOL __fastcall Process(DialogEndpointNativeBody20h* body,void*,HWND w,UINT m,WPARAM p,LPARAM l,LRESULT* out,DWORD map)
{return ProcessAboutDialogNativeAt42E520(*body,w,m,p,l,out,map,Services());}
DialogEndpointNativeBody20h* __fastcall Delete(DialogEndpointNativeBody20h* body,void*,DWORD flags)
{
    auto& s=RequireDialogEndpointNativeServices();ReleaseDialogEndpointThunkNativeAt40E730(*body,s);
    if(flags&1u)s.memory.DeleteBody(body,0x20);return body;
}
DLGPROC __fastcall Procedure(DialogEndpointNativeBody20h*,void*){return &BoundDialogEndpointNativeAt415810;}
void __fastcall Final(DialogEndpointNativeBody20h*,void*,HWND){} // 原424DC0为真正空回调。
// 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
// 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const DialogEndpointNativeVtable4 table{
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::processAt00)>(&Process),
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::deleteAt04)>(&Delete),
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::procedureAt08)>(&Procedure),
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::finalMessageAt0C)>(&Final)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
class Api final:public IAboutDialogNativeApi
{
public:
    HWND GetActiveWindow()override{return ::GetActiveWindow();}
    void SetLastError(DWORD e)override{::SetLastError(e);}
    INT_PTR DialogBoxParamA(HINSTANCE i,LPCSTR r,HWND p,DLGPROC f,LPARAM l)override{return ::DialogBoxParamA(i,r,p,f,l);}
    BOOL SetDlgItemTextA(HWND w,int id,LPCSTR t)override{return ::SetDlgItemTextA(w,id,t);}
    BOOL EndDialog(HWND w,INT_PTR r)override{return ::EndDialog(w,r);}
};
}
IAboutDialogNativeApi& DefaultAboutDialogNativeApi()noexcept{static Api api;return api;}
AboutDialogNativeScope::AboutDialogNativeScope(AboutDialogNativeServices& s)noexcept:previous_(current){current=&s;}
AboutDialogNativeScope::~AboutDialogNativeScope()noexcept{current=previous_;}
const DialogEndpointNativeVtable4& AboutDialogNativeVtable()noexcept{return table;}
void InitializeAboutDialogNativeAt42F84A(DialogEndpointNativeBody20h& body)noexcept
{
    body.windowAt04=nullptr;body.thunkAt14=nullptr;body.currentMessageAt18=nullptr;
    body.destroyPendingAt1C=0;body.vptrAt00=&table;
}
LRESULT InitializeAboutDialogNativeAt424EB0(DialogEndpointNativeBody20h& body,AboutDialogNativeServices& s)
{
    const auto item=[&](int id,DWORD offset)
    {
        const auto* text=Read<const char*>(System(s.source),offset);
        static_cast<void>(s.api.SetDlgItemTextA(Read<HWND>(&body,4),id,text));
    };
    item(0x640,0x18);item(0x641,0x20);item(0x642,0x1C);
    const auto* text=Read<const char*>(s.source.GameSystemAt418480(),0x24);
    static_cast<void>(s.api.SetDlgItemTextA(Read<HWND>(&body,4),0x643,text));
    static_cast<void>(CenterDialogWindowNativeAt413990(&body.windowAt04,nullptr,s.endpoint.api));return 1;
}
BOOL ProcessAboutDialogNativeAt42E520(DialogEndpointNativeBody20h& body,HWND,UINT message,WPARAM w,LPARAM,
    LRESULT* result,DWORD map,AboutDialogNativeServices& s)
{
    if(map)return FALSE;
    if(message==WM_INITDIALOG){*result=InitializeAboutDialogNativeAt424EB0(body,s);return TRUE;}
    if(message==WM_COMMAND&&LOWORD(w)==2)
    {static_cast<void>(s.api.EndDialog(Read<HWND>(&body,4),2));*result=0;return TRUE;}
    return FALSE;
}
void RunAboutDialogNativeAt42F84A(AboutDialogNativeServices& s)
{
    AboutDialogNativeScope about(s);DialogEndpointNativeScope endpoint(s.endpoint);
    DialogEndpointNativeBody20h body;InitializeAboutDialogNativeAt42F84A(body);
    try
    {
        s.source.EnsureRootAt413BE0();s.source.WriteMinimizeAt1B2(s.source.RootAt6198C4(),0);
        const auto parent=s.api.GetActiveWindow();auto* thunk=Read<void*>(&body,0x14);
        if(!thunk){thunk=AllocateWindowThunkNativeAt53F0E4(s.endpoint.thunks);body.thunkAt14=thunk;}
        if(!thunk)s.api.SetLastError(0x0E);
        else
        {
            InitializeWindowThunkNativeAt53F1E6(thunk,0,nullptr,s.endpoint.thunks);
            PushDialogCreationNativeAt42F8B9(body,s.endpoint);
            static_cast<void>(s.api.DialogBoxParamA(*static_cast<const volatile HINSTANCE*>(&s.nativeBase.resourceInstanceAt08),
                MAKEINTRESOURCEA(0x6D),parent,&InitialDialogEndpointNativeAt4382D0,0));
        }
        s.source.EnsureRootAt413BE0();s.source.WriteMinimizeAt1B2(s.source.RootAt6198C4(),1);
        ReleaseDialogEndpointThunkNativeAt40E730(body,s.endpoint);
    }
    catch(...)
    {
        // state0只释放thunk；不补Root标志恢复/创建链撤回。正常Free抛出仍有再次展开资格。
        try{ReleaseDialogEndpointThunkNativeAt40E730(body,s.endpoint);}catch(...){std::terminate();}
        throw;
    }
}
}
