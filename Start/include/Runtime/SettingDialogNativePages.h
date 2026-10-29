#pragma once
#include <Runtime/SettingPage01PreviewBlockNative.h>
#include <Runtime/SettingPage03Native.h>
#include <Runtime/SettingPage04Native.h>

namespace KaGuYa::Start::Runtime
{
// 原5352F0四分支的真实构造口；调用期scope由长寿场景宿主持有。
class SettingDialogNativePages final : public ISettingDialogNativePageConstruction
{
public:
    GuiDialogNativeBody5Ch* ConstructPage01At4F1EE0(void*,GuiDialogNativeBody5Ch*,std::int32_t) override;
    GuiDialogNativeBody5Ch* ConstructPage02At4F23B0(void*,GuiDialogNativeBody5Ch*,std::int32_t) override;
    GuiDialogNativeBody5Ch* ConstructPage03At4F2910(void*,GuiDialogNativeBody5Ch*,std::int32_t) override;
    GuiDialogNativeBody5Ch* ConstructPage04At4F2FE0(void*,GuiDialogNativeBody5Ch*,std::int32_t) override;
private:
    SettingPage01PreviewBlockNativeConstruction preview_;
};
}
