#include <Runtime/InformationDisplayView.h>
#include <Runtime/SystemInformationDisplayRuntime.h>
#include <Runtime/SystemDialogNative.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
SystemInformationDisplayNativeServices& InformationDisplayView::NativeServices() const
{
    if(!services_)throw std::logic_error("native InformationDisplay services are not bound");
    return *services_;
}
std::uint32_t& InformationDisplayView::SuppressedAt10() const
{return native_?body_->suppressedAt10:controlled_->SuppressedAt10();}
bool InformationDisplayView::CanOpenAt5105C0() const
{
    if(!native_)return controlled_->CanOpenAt5105C0();
    auto& services=NativeServices();SystemDialogNativeScope scope(services.dialog);
    return QuerySystemInformationDisplayNativeAt5105C0(*body_,services)!=0;
}
std::int32_t InformationDisplayView::RunAt5075B0() const
{
    if(!native_)return controlled_->RunAt5075B0();
    auto& services=NativeServices();SystemDialogNativeScope scope(services.dialog);
    return RunSystemInformationDisplayNativeAt5075B0(*body_,services);
}
}
