#include <Runtime/OpacityWindowStateServicesRuntime.h>
#include <Runtime/MessageWindowLayerRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/EnvironmentPostWindowStartupRuntime.h>
#include <Runtime/PostWindowNativeLayers.h>
#include <Runtime/RootRuntimeOwner.h>

#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
OpacityWindowStateServicesRuntime::OpacityWindowStateServicesRuntime(
    DrawManagerLayerRuntime& layers, SaveDataEnvironmentArchive& save) noexcept
    : layers_(&layers), save_(&save) {}

OpacityWindowStateServicesRuntime::OpacityWindowStateServicesRuntime(
    EnvironmentPostWindowStartupRuntime& root, EnvironmentStartupRuntime& environment) noexcept
    : root_(&root), environment_(&environment) {}

OpacityWindowStateServicesRuntime::OpacityWindowStateServicesRuntime(
    RootRuntimeOwner& root, EnvironmentStartupRuntime& environment) noexcept
    : owner_(&root), environment_(&environment) {}

SaveDataEnvironmentArchive& OpacityWindowStateServicesRuntime::Save() const
{
    if (!environment_) return *save_;
    auto* body = static_cast<SaveDataEnvironmentArchive*>(environment_->Archives().CaptureArchiveAt4180C0(1));
    if (!body) throw std::logic_error("Opacity SaveData slot is empty (safe boundary)");
    return *body;
}

MessageWindowLayerRuntime& OpacityWindowStateServicesRuntime::ModernMessageWindow() const
{
    auto* window = dynamic_cast<MessageWindowLayerRuntime*>(layers_->LayerAt418C10(1));
    if (!window) throw std::logic_error("Opacity requires CMesWin at DrawManager slot 1");
    return *window;
}

MessageWindowLayerNativeBody50h& OpacityWindowStateServicesRuntime::NativeMessageWindow() const
{
    // 50D80A/52DFBE取当前manager；52E039/52E078写Save后再次取，不缓存旧窗口。
    return *reinterpret_cast<MessageWindowLayerNativeBody50h*>(DrawLayerAt418C10(*CaptureNativePost().DrawManagerAt18(), 1));
}
EnvironmentPostWindowStartupRuntime& OpacityWindowStateServicesRuntime::CaptureNativePost() const
{
    if (!owner_) return *root_;
    // 50D80A/52DFBE/52E039/52E078各自Ensure，不沿用进入Dialog时的Post。
    owner_->EnsureCreatedAt413BE0();
    return owner_->RequirePublished().PostWindow();
}

bool OpacityWindowStateServicesRuntime::SelectedNonzero() const
{
    if (root_ || owner_)
    {
        const auto& window = NativeMessageWindow();
        const auto selected = static_cast<const volatile MessageWindowLayerNativeBody50h&>(window).selectedAt30;
        // 50D87B/52DFF4先排除负数，再按有符号(end-begin)/12校验，最后判零类。
        if (selected < 0) return false;
        return selected < DrawLayerReferenceCount(window.stylesAt24) && selected != 0;
    }
    const auto& window = ModernMessageWindow();
    const auto selected = window.SelectedStyleAt30();
    // 50D87B / 52DFF4：负数或超样式数组范围归入零类，这是原判断。
    return selected > 0 && static_cast<std::size_t>(selected) < window.StyleCount();
}
bool OpacityWindowStateServicesRuntime::SelectMessageWindowOpacitySelectorNonZeroAt50D730() const
{
    return SelectedNonzero();
}
bool OpacityWindowStateServicesRuntime::SelectMessageWindowOpacitySelectorNonZeroAt52DF20() const
{
    return SelectedNonzero();
}
std::int32_t OpacityWindowStateServicesRuntime::ReadSaveSlot1OpacitySelectorZeroOrNonZero(bool nonzero) const
{
    std::int32_t value;
    const auto& save = Save();
    std::memcpy(&value, (nonzero ? save.bytesAt48 : save.bytesAt40).data() + 4, sizeof(value));
    return value;
}
void OpacityWindowStateServicesRuntime::WriteSaveSlot1OpacitySelectorZeroOrNonZero(bool nonzero,
    std::int32_t value)
{
    auto& save = Save();
    std::memcpy((nonzero ? save.bytesAt48 : save.bytesAt40).data() + 4, &value, sizeof(value));
}
void OpacityWindowStateServicesRuntime::RefreshMessageWindowStyleAt5325F0(std::int32_t selector)
{
    if (root_ || owner_)
    {
        auto& post = CaptureNativePost();
        auto& window = *reinterpret_cast<MessageWindowLayerNativeBody50h*>(DrawLayerAt418C10(*post.DrawManagerAt18(), 1));
        MessageWindowLayerNativeScope scope(post.NativeLayerServices().message);
        RefreshMessageWindowLayerNativeAt5325F0(window, selector);
        return;
    }
    ModernMessageWindow().RefreshStyleAt5325F0(selector);
}
}
