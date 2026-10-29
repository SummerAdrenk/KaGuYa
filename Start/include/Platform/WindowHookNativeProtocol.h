#pragma once
#include <Windows.h>
#include <cstdint>

namespace KaGuYa::Start::Platform
{
// 43F6E0只调用第零槽，不拥有接收者；析构不可排在此槽前。
class NativeWindowMessageHook
{
public:
    virtual std::int32_t ProcessWindowMessage(HWND,UINT,WPARAM,LPARAM,LRESULT*,std::uint32_t)=0;
protected:
    ~NativeWindowMessageHook()=default;
};
static_assert(sizeof(NativeWindowMessageHook)==4);
class IWindowHookMessageSource
{
public:
    virtual ~IWindowHookMessageSource()=default;
    virtual std::int32_t DispatchNative(HWND,UINT,WPARAM,LPARAM,LRESULT*,std::uint32_t)=0;
};
IWindowHookMessageSource& DefaultWindowHookMessageSource() noexcept;
}
