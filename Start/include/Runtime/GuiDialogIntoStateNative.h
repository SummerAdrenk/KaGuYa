#pragma once
#include <Runtime/GuiDialogIntoNative.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 5103C0：取nested id0，存在且readState()==0时返回1；不把未找到当作inactive。
std::int32_t ReadFirstNestedGuiInactiveAt5103C0(GuiDialogNativeBody5Ch&);
// 50BAB0：first nested inactive或命中固定六个子控件时返回1，否则0。
// 查询/释放可能重入；保留normal-retain、normal-clear与EH-clear的原资格区别。
std::int32_t HitNestedGuiControlsAt50BAB0(GuiDialogNativeBody5Ch&,POINT);
std::span<const std::int32_t,6> NestedGuiHitIdsSample18() noexcept;
}
