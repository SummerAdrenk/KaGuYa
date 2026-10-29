#include <Runtime/RootCriticalSectionNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
const CriticalSectionNativeVtable* CurrentTable(CriticalSectionNativeBody24h& body) noexcept
{ return *static_cast<const CriticalSectionNativeVtable* const volatile*>(&body.vptrAt00); }
}
void InitializeRootCriticalSectionNativeAt4010C0(CriticalSectionNativeBody24h& body, WinHookNativeServices& services)
{
    WinHookNativeScope scope(services);
    services.platform.Initialize(&body.sectionAt04);
    body.threadAt1C = 0xFFFFFFFFu;
    body.depthAt20 = 0;
}
void RetireRootCriticalSectionNativeAt5AA1D0(CriticalSectionNativeBody24h& body, WinHookNativeServices& services)
{
    WinHookNativeScope scope(services);
    body.vptrAt00 = &WinHookCriticalSectionNativeTable;
    services.platform.Delete(&body.sectionAt04);
    body.vptrAt00 = &WinHookRetiredCriticalSectionNativeTable;
}
RootCriticalSectionNativeReference::RootCriticalSectionNativeReference(
    CriticalSectionNativeBody24h& body, WinHookNativeServices& services) noexcept
    : body_(body), services_(services) {}
void RootCriticalSectionNativeReference::Enter()
{
    WinHookNativeScope scope(services_);
    CurrentTable(body_)->enterAt00(&body_);
}
void RootCriticalSectionNativeReference::Leave() noexcept
{
    WinHookNativeScope scope(services_);
    CurrentTable(body_)->leaveAt04(&body_);
}
bool RootCriticalSectionNativeReference::HeldByCurrentThreadAt4076D0() const
{
    WinHookNativeScope scope(services_);
    return CurrentTable(body_)->heldAt08(&body_) != 0;
}
}
