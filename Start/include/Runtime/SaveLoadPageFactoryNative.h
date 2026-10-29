#pragma once
#include <Runtime/SaveLoadDialogNativeWorkflow.h>
#include <Runtime/SaveLoadPageNative.h>
#include <Runtime/SaveLoadSystemPageNative.h>
#include <Runtime/SaveLoadPopupNative.h>

namespace KaGuYa::Start::Runtime
{
// 四个分支都进入真实raw构造，禁止把modern页面或shared对象塞进5330A0返回值。
class SaveLoadPageFactoryNative final:public ISaveLoadDialogNativePageFactory
{
public:
    SaveLoadPageFactoryNative(SaveLoadPageNativeServices& pages,SaveLoadSystemPageNativeServices& system,
        SaveLoadPopupNativeServices& popup) noexcept:pages_(pages),system_(system),popup_(popup){}
    GuiDialogNativeBody5Ch* ConstructPage00At4ED190(void*,GuiDialogNativeBody5Ch*,std::int32_t) override;
    GuiDialogNativeBody5Ch* ConstructPage01At4EE390(void*,GuiDialogNativeBody5Ch*,std::int32_t) override;
    GuiDialogNativeBody5Ch* ConstructSystemAt4F0C80(void*,GuiDialogNativeBody5Ch*,std::int32_t) override;
    GuiDialogNativeBody5Ch* ConstructPopupAt4EF660(void*,GuiDialogNativeBody5Ch*,std::int32_t) override;
    SaveLoadPageNativeServices& Pages() noexcept{return pages_;}
    SaveLoadSystemPageNativeServices& System() noexcept{return system_;}
    SaveLoadPopupNativeServices& Popup() noexcept{return popup_;}
private:
    SaveLoadPageNativeServices& pages_;
    SaveLoadSystemPageNativeServices& system_;
    SaveLoadPopupNativeServices& popup_;
};
// 由Scene/Task owner持有到全部raw child最后Ref；不能只在Initialize期间绑定。
class SaveLoadPageFactoryNativeScope final
{
public:
    explicit SaveLoadPageFactoryNativeScope(SaveLoadPageFactoryNative& factory) noexcept
        :pages_(factory.Pages()),system_(factory.System()),popup_(factory.Popup()){}
    SaveLoadPageFactoryNativeScope(const SaveLoadPageFactoryNativeScope&)=delete;
    SaveLoadPageFactoryNativeScope& operator=(const SaveLoadPageFactoryNativeScope&)=delete;
private:
    SaveLoadPageNativeScope pages_;
    SaveLoadSystemPageNativeScope system_;
    SaveLoadPopupNativeScope popup_;
};
}
