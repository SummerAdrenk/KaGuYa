#include <Runtime/ToolWindowNative.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/ScrVmExecutableProfile.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
EnvironmentToolWindowReplayNativeSource::EnvironmentToolWindowReplayNativeSource(EnvironmentRootOwner& environment,
    RootRuntimeOwner& root, ScriptNamedValueNativeTables names, IScrVmCommandErrorSink& errors,
    IRuntimeStringManagerSource& strings) noexcept
    : environment_(environment), root_(root), names_(names), errors_(errors), strings_(strings) {}
std::int32_t EnvironmentToolWindowReplayNativeSource::ReadReplayAt417B50()
{
    // 417B58先捕获Env slot0，417BA4才Ensure根；498020不消费已捕获的slot0正文。
    auto* environment = environment_.CaptureAt4180C0();
    const auto bytes = std::uint32_t(reinterpret_cast<std::uintptr_t>(environment->archivesAt30.end) -
        reinterpret_cast<std::uintptr_t>(environment->archivesAt30.begin));
    const auto count = std::bit_cast<std::int32_t>(bytes) / 12;
    if (count != 0)
        (void)ResolveEnvironmentObjectReference<std::byte>(environment->archivesAt30.begin[0]);
    root_.EnsureCreatedAt413BE0();
    auto* const root = root_.ReadPublishedAt6198C4();
    const auto index = ResolveScriptNamedValueNativeAt498020("replay", names_, strings_);
    return ReadScriptValueAt4C73A0(root->ParamsAt47BDB0(), index, errors_);
}
}
