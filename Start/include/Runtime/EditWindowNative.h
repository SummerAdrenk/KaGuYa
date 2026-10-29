#pragma once
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/RuntimeCString.h>
#include <Platform/PendingWindowCreation.h>
#include <Windows.h>
#include <cstddef>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
struct EditWindowEndpointNativeBody24h;
// 原5B2694/5B26A8各四槽；不是现代WindowHost的C++虚表。
struct EditWindowEndpointNativeVtable4 final
{
    BOOL (__thiscall* processAt00)(EditWindowEndpointNativeBody24h*,HWND,UINT,WPARAM,LPARAM,LRESULT*,DWORD);
    EditWindowEndpointNativeBody24h* (__thiscall* deleteAt04)(EditWindowEndpointNativeBody24h*,std::uint32_t);
    WNDPROC (__thiscall* procedureAt08)(EditWindowEndpointNativeBody24h*);
    void (__thiscall* finalMessageAt0C)(EditWindowEndpointNativeBody24h*,HWND);
};
struct EditWindowEndpointNativeBody24h final
{
    const EditWindowEndpointNativeVtable4* vptrAt00;
    HWND windowAt04;
    union { Platform::PendingWindowCreateRecord creationAt08; };
    void* thunkAt14;
    void* currentMessageAt18;
    std::uint32_t destroyPendingAt1C;
    WNDPROC previousAt20;
    EditWindowEndpointNativeBody24h() noexcept {}
};
// 4687F9删除尺寸28h；事件是Control子对象+24，不属于通用24h端点。
struct EditControlNativeBody28h final
{
    union { EditWindowEndpointNativeBody24h endpointAt00; };
    std::uint32_t eventAt24;
    EditControlNativeBody28h() noexcept {}
};
// 46FA74..46FB5F确认98h及内嵌+24 Control；仅建立存储，不伪造窗口创建成功。
struct EditWindowNativeBody98h final
{
    union { EditWindowEndpointNativeBody24h outerAt00; };
    union { EditControlNativeBody28h controlAt24; };
    RECT logicalAt4C;
    RECT marginsAt5C;
    union { YtlReferenceSlotRuntime layoutAt6C; };
    std::uint32_t identifierAt78;
    std::int32_t byteLimitAt7C;
    std::uint32_t alignmentAt80;
    COLORREF textColorAt84;
    COLORREF backgroundAt88;
    union { YtlReferenceSlotRuntime brushAt8C; };
    EditWindowNativeBody98h() noexcept {}
};
static_assert(sizeof(EditWindowEndpointNativeVtable4)==0x10);
static_assert(sizeof(EditWindowEndpointNativeBody24h)==0x24);
static_assert(sizeof(EditControlNativeBody28h)==0x28);
static_assert(sizeof(EditWindowNativeBody98h)==0x98);
static_assert(alignof(EditWindowNativeBody98h)==4);
static_assert(std::is_standard_layout_v<EditWindowNativeBody98h>);
static_assert(std::is_trivially_destructible_v<EditWindowNativeBody98h>);
#define EDIT_NATIVE_OFFSET(type,field,offset) static_assert(offsetof(type,field)==offset)
EDIT_NATIVE_OFFSET(EditWindowEndpointNativeBody24h,windowAt04,4);
EDIT_NATIVE_OFFSET(EditWindowEndpointNativeBody24h,creationAt08,8);
EDIT_NATIVE_OFFSET(EditWindowEndpointNativeBody24h,thunkAt14,0x14);
EDIT_NATIVE_OFFSET(EditWindowEndpointNativeBody24h,currentMessageAt18,0x18);
EDIT_NATIVE_OFFSET(EditWindowEndpointNativeBody24h,destroyPendingAt1C,0x1C);
EDIT_NATIVE_OFFSET(EditWindowEndpointNativeBody24h,previousAt20,0x20);
EDIT_NATIVE_OFFSET(EditControlNativeBody28h,endpointAt00,0);
EDIT_NATIVE_OFFSET(EditControlNativeBody28h,eventAt24,0x24);
EDIT_NATIVE_OFFSET(EditWindowNativeBody98h,controlAt24,0x24);
static_assert(offsetof(EditWindowNativeBody98h,controlAt24)+offsetof(EditControlNativeBody28h,eventAt24)==0x48);
EDIT_NATIVE_OFFSET(EditWindowNativeBody98h,logicalAt4C,0x4C);
EDIT_NATIVE_OFFSET(EditWindowNativeBody98h,marginsAt5C,0x5C);
EDIT_NATIVE_OFFSET(EditWindowNativeBody98h,layoutAt6C,0x6C);
EDIT_NATIVE_OFFSET(EditWindowNativeBody98h,identifierAt78,0x78);
EDIT_NATIVE_OFFSET(EditWindowNativeBody98h,byteLimitAt7C,0x7C);
EDIT_NATIVE_OFFSET(EditWindowNativeBody98h,alignmentAt80,0x80);
EDIT_NATIVE_OFFSET(EditWindowNativeBody98h,textColorAt84,0x84);
EDIT_NATIVE_OFFSET(EditWindowNativeBody98h,backgroundAt88,0x88);
EDIT_NATIVE_OFFSET(EditWindowNativeBody98h,brushAt8C,0x8C);
#undef EDIT_NATIVE_OFFSET

class IEditWindowNativeTextApi
{
public:
    virtual ~IEditWindowNativeTextApi()=default;
    virtual int GetWindowTextLengthA(HWND)=0;
    virtual int GetWindowTextA(HWND,LPSTR,int)=0;
};
IEditWindowNativeTextApi& DefaultEditWindowNativeTextApi() noexcept;
// 原50B000返回fresh CString*；调用成功后才由caller取得正常释放资格。
RuntimeCString* ReadEditWindowNativeTextAt50B000(EditWindowNativeBody98h&,RuntimeCString*,
    IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource(),
    IEditWindowNativeTextApi& = DefaultEditWindowNativeTextApi());

struct EditWindowMessageNativeContext24h final
{
    MSG message;
    std::uint32_t sizeAt1C;
    BOOL handledAt20;
};
static_assert(sizeof(EditWindowMessageNativeContext24h)==0x24);
static_assert(offsetof(EditWindowMessageNativeContext24h,sizeAt1C)==0x1C);
static_assert(offsetof(EditWindowMessageNativeContext24h,handledAt20)==0x20);
class IEditWindowNativeMessageApi
{
public:
    virtual ~IEditWindowNativeMessageApi()=default;
    virtual LRESULT CallWindowProcA(WNDPROC,HWND,UINT,WPARAM,LPARAM)=0;
    virtual LONG GetWindowLongA(HWND,int)=0;
    virtual LONG SetWindowLongA(HWND,int,LONG)=0;
};
IEditWindowNativeMessageApi& DefaultEditWindowNativeMessageApi() noexcept;
// 439F60是所有原端点共用分派核；不新增异常恢复currentMessage的RAII资格。
LRESULT DispatchEditWindowNativeMessageAt439F60(EditWindowEndpointNativeBody24h&,UINT,WPARAM,LPARAM,
    IEditWindowNativeMessageApi& = DefaultEditWindowNativeMessageApi());
}
