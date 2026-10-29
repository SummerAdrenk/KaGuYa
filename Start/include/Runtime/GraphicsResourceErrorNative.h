#pragma once

#include <Runtime/RuntimeCString.h>
#include <Windows.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentRootOwner;

// 与旧const-char显示域分开：借caller的CString槽，在481605取Env之后才读chars。
class IGraphicsResourceErrorDisplayNative
{
public:
    virtual void Show(RuntimeCString& text) = 0;
protected:
    ~IGraphicsResourceErrorDisplayNative() = default;
};

class RootGraphicsResourceErrorDisplayNative final : public IGraphicsResourceErrorDisplayNative
{
public:
    using MessageBox = int (WINAPI*)(HWND, LPCSTR, LPCSTR, UINT);
    explicit RootGraphicsResourceErrorDisplayNative(EnvironmentRootOwner& environment,
        MessageBox messageBox = &::MessageBoxA) noexcept;
    void Show(RuntimeCString& text) override;
private:
    EnvironmentRootOwner& environment_;
    MessageBox messageBox_;
};
}
