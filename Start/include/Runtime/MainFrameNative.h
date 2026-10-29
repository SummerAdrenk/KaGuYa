#pragma once
#include <Runtime/EditWindowNative.h>
#include <Runtime/UpdateUiNative.h>
#include <Runtime/WindowThunkNative.h>

namespace KaGuYa::Start::Runtime
{
class IEnvironmentRootMemory;
// 原90h CMainFrame；24h窗口端点核与Edit/Popup的原287字节分派同义，含ret10。
// 原未写槽只保留offset，不用成员初值或value-init制造默认值。
struct MainFrameNativeBody90h final
{
    union{EditWindowEndpointNativeBody24h windowAt00;};
    DWORD opaqueAt24[3];
    HACCEL acceleratorAt30;
    const void* filterVptrAt34;
    const void* idleVptrAt38;
    UpdateUiNativeBody18h updateAt3C;
    const void* menuVptrAt54;
    DWORD unwrittenAt58;
    DWORD menuWordAt5C;
    union{EditWindowEndpointNativeBody24h popupAt60;};
    DWORD popupUnwrittenAt84;
    DWORD popupUnwrittenAt88;
    BYTE paintByteAt8C;
    BYTE unwrittenAt8D[3];
    MainFrameNativeBody90h()noexcept{}
};
static_assert(sizeof(MainFrameNativeBody90h)==0x90&&alignof(MainFrameNativeBody90h)==4);
static_assert(std::is_standard_layout_v<MainFrameNativeBody90h>&&std::is_trivially_destructible_v<MainFrameNativeBody90h>);
#define MAIN_FRAME_NATIVE_OFFSET(field,offset) static_assert(offsetof(MainFrameNativeBody90h,field)==offset)
MAIN_FRAME_NATIVE_OFFSET(windowAt00,0);MAIN_FRAME_NATIVE_OFFSET(opaqueAt24,0x24);
MAIN_FRAME_NATIVE_OFFSET(acceleratorAt30,0x30);MAIN_FRAME_NATIVE_OFFSET(filterVptrAt34,0x34);
MAIN_FRAME_NATIVE_OFFSET(idleVptrAt38,0x38);MAIN_FRAME_NATIVE_OFFSET(updateAt3C,0x3C);
MAIN_FRAME_NATIVE_OFFSET(menuVptrAt54,0x54);MAIN_FRAME_NATIVE_OFFSET(unwrittenAt58,0x58);
MAIN_FRAME_NATIVE_OFFSET(menuWordAt5C,0x5C);MAIN_FRAME_NATIVE_OFFSET(popupAt60,0x60);
MAIN_FRAME_NATIVE_OFFSET(popupUnwrittenAt84,0x84);MAIN_FRAME_NATIVE_OFFSET(popupUnwrittenAt88,0x88);
MAIN_FRAME_NATIVE_OFFSET(paintByteAt8C,0x8C);MAIN_FRAME_NATIVE_OFFSET(unwrittenAt8D,0x8D);
#undef MAIN_FRAME_NATIVE_OFFSET
// caller须提供实际业务表；本构造核不伪造尚未接通的MainFrame/Menu消息分支。
struct MainFrameNativeConstructorTables final
{
    const EditWindowEndpointNativeVtable4& window;
    const void* filter;
    const void* idle;
    const void* menu;
    const EditWindowEndpointNativeVtable4& popup;
};
// 两段分开保留原EH资格：base后state=1，再调用40B600，tail后才进入state=3。
void InitializeMainFrameBaseNativeAt434379(MainFrameNativeBody90h&)noexcept;
void InitializeMainFrameTailNativeAt4343C6(MainFrameNativeBody90h&,MainFrameNativeConstructorTables)noexcept;
void ReleaseMainFrameBaseNativeAt40CC70(MainFrameNativeBody90h&,WindowThunkNativeServices&);
void ReleaseMainFrameNativeAt40EA80(MainFrameNativeBody90h&,IUpdateUiNativeMemory&,WindowThunkNativeServices&);
MainFrameNativeBody90h* DeleteMainFrameNativeAt411CC0(MainFrameNativeBody90h*,DWORD flags,
    IUpdateUiNativeMemory&,WindowThunkNativeServices&,IEnvironmentRootMemory&);
}
