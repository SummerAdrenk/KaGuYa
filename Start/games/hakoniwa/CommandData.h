#pragma once

#include <Runtime/SelectDialogNative.h>
#include <Runtime/CommandSelectionNative.h>
#include <span>

namespace KaGuYa::Start::Runtime::HakoniwaCommandData
{
// 原 CommandDialog 仅一组四按钮；可见性和选择结果由共享流程负责。
std::span<const GuiButtonDescriptorNative2Ch, 4> Buttons() noexcept;
// 外层固定四项，无 255 哨兵；每张 pair 表保留负 source 终止项。
std::span<const SelectNavigationDescriptorNative8h, 4> Navigation() noexcept;
const CommandSelectionResourceProfile& Profile() noexcept;
}
