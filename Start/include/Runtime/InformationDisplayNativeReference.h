#pragma once
#include <Runtime/SystemInformationDisplayNative.h>

namespace KaGuYa::Start::Runtime
{
// 48A64A..48A6F8：固定receiver为选中根的唯一+3C槽，不在此Ensure或重选根。
void CreateAndInstallInformationDisplayAt48A64A(YtlReferenceSlotRuntime&,
    SystemInformationDisplayNativeServices&);
// 40D1A0/46432D：原EHFlags=5退役，仅control非零才清base/control，bias保持。
void ReleaseInformationDisplayReferenceAt40D1A0(YtlReferenceSlotRuntime&,
    SystemDialogNativeServices* =nullptr) noexcept;
}
