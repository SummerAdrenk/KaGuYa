#pragma once
#include <Runtime/LegacyTitleMenu.h>
#include <Runtime/LegacyTitleScene.h>
#include <Runtime/GuiControlFactoryNative.h>

namespace KaGuYa::Start::Runtime::BakuTitleData
{
// 原569690六条28h记录、569780导航、56A1E0动作表；不注册工厂或选择owner。
const LegacyTitleMenuData& Menu() noexcept;
const LegacyTitleMenuNames& Names() noexcept;
const LegacyTitleSceneData& Scene() noexcept;
// 只映射图像状态和事件到共享新版GUI，不复刻旧菜单内部状态机。
const GuiStaticDescriptorNative24h& SharedBackground() noexcept;
std::span<const GuiButtonDescriptorNative2Ch, 6> SharedButtons() noexcept;
}
