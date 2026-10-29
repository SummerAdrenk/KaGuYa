#pragma once
#include <Runtime/SettingDialogNative.h>
#include "SettingSystemPageNative.Internal.h"

namespace KaGuYa::Start::Runtime::SettingDialogNativeDetail
{
using namespace SettingNativeDetail;
using Body = SettingDialogNativeBodyACh;
inline Body& Dialog(Common* p) noexcept { return *reinterpret_cast<Body*>(p); }
inline const GuiDialogIntoNativeVtable37& Table(Common* p) noexcept
{ return *reinterpret_cast<const GuiDialogIntoNativeVtable37*>(Read(p->vptrAt00)); }
void ConstructOwnedDialogReference(Slot&, Common*, std::uint32_t stride, IEnvironmentRootMemory&);
// 所有内联lookup保留end/begin顺序；两个4FF430的已释放锁后查询反过来读取。
void* CaptureRegistry(ISettingDialogNativeSource&, std::uint32_t index, bool beginFirst = false);
void AddBackgroundBounds(Body&);
void __fastcall Begin(Common*, void*);
std::int32_t __fastcall Poll(Common*, void*);
std::int32_t __fastcall End(Common*, void*);
void __fastcall ProcessInput(Common*, void*);
void __fastcall FlushDirty(Common*, void*);
}
