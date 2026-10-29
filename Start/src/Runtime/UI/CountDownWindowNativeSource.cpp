#include <Runtime/CountDownWindowNative.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
EnvironmentCountDownWindowNativeStateSource::EnvironmentCountDownWindowNativeStateSource(
    EnvironmentRootOwner& environment, RootRuntimeOwner& root, ScriptNamedValueNativeTables tables,
    IScrVmCommandErrorSink& errors, IRuntimeStringManagerSource& strings) noexcept
    : environment_(environment), root_(root), tables_(tables), errors_(errors), strings_(strings) {}
GlobalParamsArchive* EnvironmentCountDownWindowNativeStateSource::CaptureGlobalInlineAt4180C0()
{
    auto* const root = environment_.CaptureAt4180C0();
    const auto bytes = std::uint32_t(reinterpret_cast<std::uintptr_t>(root->archivesAt30.end) - reinterpret_cast<std::uintptr_t>(root->archivesAt30.begin));
    const auto count = std::bit_cast<std::int32_t>(bytes) / 12;
    if (static_cast<std::uint32_t>(count) <= 7U) return nullptr;
    return ResolveEnvironmentObjectReference<GlobalParamsArchive>(root->archivesAt30.begin[7]);
}
GlobalParamsArchive* EnvironmentCountDownWindowNativeStateSource::CaptureGlobalVia417990()
{ return static_cast<GlobalParamsArchive*>(ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 7)); }
std::int32_t EnvironmentCountDownWindowNativeStateSource::ReadSuffixAfterRootAt486F8C()
{
    root_.EnsureCreatedAt413BE0();
    auto& captured = root_.RequirePublished();
    // 498020不消费slot0接收者，但4180C0/417990本身仍必须在原位置执行。
    (void)ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 0);
    // 10号466B6D/46BF10同样读取countdown_suffix（原表索引40）；五帧不另造number变量。
    const auto index = ResolveScriptNamedValueNativeAt498020("countdown_suffix",tables_,strings_);
    return ReadScriptValueAt4C73A0(captured.ParamsAt47BDB0(), index, errors_);
}
}
