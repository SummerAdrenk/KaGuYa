#pragma once
#include <Runtime/SystemInformationDisplayNative.h>

namespace KaGuYa::Start::Runtime
{
class SystemInformationDisplayRuntime;
// 显式选择原生/历史受控域；只借固定receiver与外部服务，不保活、不复制业务DWORD。
class InformationDisplayView final
{
public:
    explicit InformationDisplayView(SystemInformationDisplayRuntime* controlled) noexcept:controlled_(controlled){}
    InformationDisplayView(SystemInformationDisplayNativeBody14h* body,SystemInformationDisplayNativeServices* services) noexcept
        :body_(body),services_(services),native_(true){}
    bool IsNative() const noexcept{return native_;}
    explicit operator bool() const noexcept{return native_?body_!=nullptr:controlled_!=nullptr;}
    const void* Address() const noexcept{return native_?static_cast<const void*>(body_):controlled_;}
    SystemInformationDisplayNativeBody14h* NativeBody() const noexcept{return body_;}
    std::uint32_t& SuppressedAt10() const;
    bool CanOpenAt5105C0() const;
    std::int32_t RunAt5075B0() const;
private:
    SystemInformationDisplayNativeServices& NativeServices() const;
    SystemInformationDisplayRuntime* controlled_=nullptr;
    SystemInformationDisplayNativeBody14h* body_=nullptr;
    SystemInformationDisplayNativeServices* services_=nullptr;
    bool native_=false;
};
}
