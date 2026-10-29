#include <Runtime/DateWindowNative.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ScriptArithmeticCommandNative.h>

namespace KaGuYa::Start::Runtime
{
RootDateWindowStateSource::RootDateWindowStateSource(RootRuntimeOwner& root,
    ScriptNamedValueNativeTables tables, IScrVmCommandErrorSink& errors, IRuntimeStringManagerSource& strings) noexcept
    : root_(root), tables_(tables), errors_(errors), strings_(strings) {}

std::int32_t RootDateWindowStateSource::ReadDateValue(const char* name)
{
    root_.EnsureCreatedAt413BE0();
    auto& root = root_.RequirePublished();
    const auto index = ResolveScriptNamedValueNativeAt498020(name, tables_, strings_);
    return ReadScriptValueAt4C73A0(root.ParamsAt47BDB0(), index, errors_);
}
}
