#include <Runtime/MainFramePopupNativeRuntime.h>

namespace KaGuYa::Start::Runtime
{
MainFramePopupNativeRuntime::MainFramePopupNativeRuntime(IMainFrameContextMenuNativeSource& source,
    MainFrameChevronNativeServices chevron,IMainFrameContextMenuNativeApi& api)noexcept
    :context_(source),api_(api),chevron_(chevron){}
LRESULT MainFramePopupNativeRuntime::ContextMenuAt428C10(MainFrameNativeBody90h& frame,DWORD high,DWORD id,
    LPARAM l,BOOL* handled)
{return ShowMainFrameContextMenuNativeAt428C10(frame,high,id,l,handled,context_,api_);}
LRESULT MainFramePopupNativeRuntime::ChevronAt423550(MainFrameNativeBody90h& frame,WPARAM w,LPARAM l,BOOL* handled)
{return chevron_.ChevronAt423550(frame,w,l,handled);}
}
