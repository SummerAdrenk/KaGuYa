#pragma once
#include <Runtime/EditWindowNativeMaps.h>
#include <Runtime/WindowThunkNative.h>

namespace KaGuYa::Start::Runtime
{
class EditWindowNativeScope final
{
public:
    EditWindowNativeScope(EditWindowNativeMapServices&,WindowThunkNativeServices&) noexcept;
    EditWindowNativeScope(EditWindowNativeMapServices*,WindowThunkNativeServices*) noexcept;
    ~EditWindowNativeScope() noexcept;
    EditWindowNativeScope(const EditWindowNativeScope&)=delete;
    EditWindowNativeScope& operator=(const EditWindowNativeScope&)=delete;
private:
    EditWindowNativeMapServices* previous_;
    WindowThunkNativeScope thunks_;
};
EditWindowNativeMapServices& RequireEditWindowNativeMapServices();
const EditWindowEndpointNativeVtable4& EditWindowNativeVtable() noexcept;
const EditWindowEndpointNativeVtable4& EditControlNativeVtable() noexcept;
// 原46FA74..46FB5F内联存储构造，不执行46FA30后半的窗口/Root向量创建。
void InitializeEditWindowNativeStorageAt46FA74(EditWindowNativeBody98h&);
EditWindowNativeBody98h* DeleteEditWindowNativeAt468810(EditWindowNativeBody98h*,std::uint32_t) noexcept;
EditControlNativeBody28h* DeleteEditControlNativeAt4687E0(EditControlNativeBody28h*,std::uint32_t);
}
