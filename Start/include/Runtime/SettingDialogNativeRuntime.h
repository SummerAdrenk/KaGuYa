#pragma once
#include <Runtime/SettingDialogNative.h>
#include <Runtime/SettingPage02Native.h>
#include <Runtime/SettingPage01PreviewBlockNative.h>
#include <Runtime/SettingPage03Native.h>
#include <Runtime/SettingPage04Native.h>

namespace KaGuYa::Start::Runtime
{
struct SettingDialogNativeRuntimeServices final
{
    SettingDialogNativeServices& parent;
    SettingPage02NativeServices& page01And02;
    SettingPage01PreviewBlockNativeServices& preview;
    SettingPage03NativeServices& page03;
    SettingPage04NativeServices& page04;
};
// 自动作用域只借服务、不分配正文；寿命覆盖父/四页虚调用及最终Ref退役。
class SettingDialogNativeRuntimeScope final
{
public:
    explicit SettingDialogNativeRuntimeScope(SettingDialogNativeRuntimeServices& s) noexcept
        :pages_(s.page01And02),preview_(s.preview),sound_(s.page03),text_(s.page04),parent_(s.parent){}
    SettingDialogNativeRuntimeScope(const SettingDialogNativeRuntimeScope&)=delete;
    SettingDialogNativeRuntimeScope& operator=(const SettingDialogNativeRuntimeScope&)=delete;
private:
    SettingPage02NativeScope pages_;
    SettingPage01PreviewBlockNativeScope preview_;
    SettingPage03NativeScope sound_;
    SettingPage04NativeScope text_;
    SettingDialogNativeScope parent_;
};
}
