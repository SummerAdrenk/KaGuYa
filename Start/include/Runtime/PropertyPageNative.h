#pragma once
#include <Runtime/DialogEndpointNative.h>
#include <CommCtrl.h>

namespace KaGuYa::Start::Runtime
{
struct PropertyPageNativeHeader34h;
using PropertyPageNativeCallback=UINT(CALLBACK*)(HWND,UINT,PropertyPageNativeHeader34h*);
// 原SDK的V3前缀34h；不可用新SDK sizeof(PROPSHEETPAGEA)多读末尾字段。
struct PropertyPageNativeHeader34h final
{
    DWORD sizeAt00,flagsAt04;
    HINSTANCE instanceAt08;
    LPCSTR resourceAt0C;
    HICON iconAt10;
    LPCSTR titleAt14;
    DLGPROC procedureAt18;
    LPARAM objectAt1C;
    PropertyPageNativeCallback callbackAt20;
    UINT* refParentAt24;
    LPCSTR headerTitleAt28,headerSubtitleAt2C;
    HANDLE activationContextAt30;
};
struct PropertyPageNativeBody54h final
{
    union{DialogEndpointNativeBody20h endpointAt00;};
    PropertyPageNativeHeader34h headerAt20;
    PropertyPageNativeBody54h()noexcept{}
};
static_assert(sizeof(PropertyPageNativeHeader34h)==0x34&&sizeof(PropertyPageNativeBody54h)==0x54);
static_assert(offsetof(PropertyPageNativeHeader34h,objectAt1C)==0x1C&&offsetof(PropertyPageNativeHeader34h,callbackAt20)==0x20);
static_assert(offsetof(PropertyPageNativeBody54h,headerAt20)==0x20);
// 仅构造原共同字段；业务页的54h以后成员由其原Ctor负责，不提前清零。
void InitializePropertyPageNativeAt40B91C(PropertyPageNativeBody54h&,const DialogEndpointNativeVtable4&,
    WORD resource,PropertyPageNativeCallback,WindowClassModuleNativeBody38h&)noexcept;
// 原412960对record/object空值主动RaiseException(C0000005,1)，不退化成空记录。
void PushPropertyPageCreationNativeAt412960(Platform::PendingWindowCreateRecord*,void*,DialogEndpointNativeServices&);
UINT CALLBACK PropertyPageCreationNativeAt430F20(HWND,UINT,PropertyPageNativeHeader34h*);
INT_PTR CALLBACK InitialPropertyPageNativeAt438260(HWND,UINT,WPARAM,LPARAM);
}
