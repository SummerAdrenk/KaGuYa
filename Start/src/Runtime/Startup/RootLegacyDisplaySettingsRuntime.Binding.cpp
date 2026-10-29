#include <Runtime/RootLegacyDisplaySettingsRuntime.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace { thread_local RootLegacyDisplaySettingsScope* current; }
RootLegacyDisplaySettingsScope::RootLegacyDisplaySettingsScope(LegacyDisplaySettingsFamily family,
    IRootLegacyDisplaySettingsRuntime* runtime) noexcept : family_(family), runtime_(runtime), previous_(current)
{ current = this; }
RootLegacyDisplaySettingsScope::~RootLegacyDisplaySettingsScope() noexcept { current = previous_; }
IRootLegacyDisplaySettingsRuntime* BoundRootLegacyDisplaySettings()
{
    if (current)
    {
        if (current->family_ == LegacyDisplaySettingsFamily::Disabled) return nullptr;
        if (!current->runtime_) throw std::logic_error("Enabled legacy display scope has no runtime");
        return current->runtime_;
    }
    if (CompiledLegacyDisplaySettingsFamily() != LegacyDisplaySettingsFamily::Disabled)
        throw std::logic_error("Enabled legacy display requires its MainFrame service scope");
    return nullptr;
}
void RootLegacyDisplayApplicationSource::EnsureRootAt413BE0()
{
    try { base_.EnsureRootAt413BE0(); }
    catch (...)
    {
        // 434691之后的Ensure位于原运行catch外；在frame/scope展开前清理，不等Run返回。
        if (legacy_) legacy_->BeforeSubsystemRelease();
        throw;
    }
}
bool RootLegacyDisplayApplicationSource::InitializeBeforeWindowAt4778B0(void* root)
{
    if (!base_.InitializeBeforeWindowAt4778B0(root)) return false;
    if (legacy_) legacy_->AfterPreWindow();
    return true;
}
bool RootLegacyDisplayApplicationSource::InitializeAfterWindowAt489C90(void* root)
{
    if (!base_.InitializeAfterWindowAt489C90(root)) return false;
    if (legacy_) legacy_->AfterPostWindow();
    return true;
}
void RootLegacyDisplayApplicationSource::ReleaseSubsystemsAt477620(void* root)
{
    if (legacy_) legacy_->BeforeSubsystemRelease();
    base_.ReleaseSubsystemsAt477620(root);
}
LRESULT RootLegacyDisplayWindowApi::CallWindowProcA(WNDPROC previous,HWND window,UINT message,WPARAM w,LPARAM l)
{
    if (message == WM_CLOSE && legacy_ && !legacy_->BeforeClose(window)) return 0;
    return base_.CallWindowProcA(previous,window,message,w,l);
}
}
