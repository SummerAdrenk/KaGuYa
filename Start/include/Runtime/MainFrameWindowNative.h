#pragma once
#include <Runtime/MainFrameNativeDispatch.h>
#include <Runtime/MainFrameNativeCallbacks.h>
#include <Runtime/MainFrameMenuNative.h>
#include <Runtime/MainFramePopupPositionNative.h>
#include <Runtime/DialogEndpointNative.h>

namespace KaGuYa::Start::Runtime
{
struct MainFrameWindowNativeServices final
{
    MainFrameNativeDispatchServices& dispatch;
    IMainFrameNativeIdleSource& idle;
    IMainFrameNativeCallbackApi& callbacks;
    IEditWindowNativeMessageApi& messages;
    DialogEndpointNativeServices& creation;
    IUpdateUiNativeMemory& update;
    IEnvironmentRootMemory& memory;
};
// 必须覆盖创建、整个消息泵及同步重入；只借元数据，不向原90h加服务字段。
class MainFrameWindowNativeScope final
{
public:
    explicit MainFrameWindowNativeScope(MainFrameWindowNativeServices&) noexcept;
    ~MainFrameWindowNativeScope() noexcept;
    MainFrameWindowNativeScope(const MainFrameWindowNativeScope&) = delete;
    MainFrameWindowNativeScope& operator=(const MainFrameWindowNativeScope&) = delete;
private:
    MainFrameWindowNativeServices* previous_;
};
MainFrameNativeConstructorTables MainFrameWindowNativeConstructorTables() noexcept;
MainFrameWindowNativeServices& RequireMainFrameWindowNativeServices();
LRESULT CALLBACK InitialMainFrameWindowNativeAt438340(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK BoundMainFrameWindowNativeAt43A080(HWND, UINT, WPARAM, LPARAM);
void PushMainFrameWindowCreationNativeAt434592(MainFrameNativeBody90h&, MainFrameWindowNativeServices&);
}
