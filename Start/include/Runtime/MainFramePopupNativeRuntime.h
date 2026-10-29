#pragma once
#include <Runtime/MainFrameCommandNativeRuntime.h>
#include <Runtime/MainFrameContextMenuNative.h>
#include <Runtime/MainFrameChevronNative.h>

namespace KaGuYa::Start::Runtime
{
// 两条独立原生菜单链共享实际F正文；冷装配不取根、不创建HMENU或窗口。
class MainFramePopupNativeRuntime final:public IMainFrameNativePopupHandlers
{
public:
    MainFramePopupNativeRuntime(IMainFrameContextMenuNativeSource&,
        MainFrameChevronNativeServices = DefaultMainFrameChevronNativeServices(),
        IMainFrameContextMenuNativeApi& = DefaultMainFrameContextMenuNativeApi())noexcept;
    LRESULT ContextMenuAt428C10(MainFrameNativeBody90h&,DWORD,DWORD,LPARAM,BOOL*)override;
    LRESULT ChevronAt423550(MainFrameNativeBody90h&,WPARAM,LPARAM,BOOL*)override;
private:
    IMainFrameContextMenuNativeSource& context_;
    IMainFrameContextMenuNativeApi& api_;
    MainFrameChevronNativeRuntime chevron_;
};
}
