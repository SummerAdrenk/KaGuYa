#include <Runtime/ScriptFileCommandSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentStartupRuntime.h>

namespace KaGuYa::Start::Runtime
{
RootScriptFileCommandSource::RootScriptFileCommandSource(RootRuntimeOwner& root,
    EnvironmentStartupRuntime& environment, ScriptNamedValueNativeTables tables,
    const IScrResourceNameSource& names, IRuntimeStringManagerSource& strings) noexcept
    : root_(root), environment_(environment), tables_(tables), names_(names), strings_(strings) {}

ScriptFileRootReceiver RootScriptFileCommandSource::CaptureAfterEnsureAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return {&root_.RequirePublished(), [](void* p) -> ScrVmParamsGeneration& {
        return static_cast<RootRuntimePayload*>(p)->ParamsAt47BDB0();
    }};
}
const IEnvironmentArchiveRuntime* RootScriptFileCommandSource::CaptureSlot0At4180C0()
{ return environment_.Archives().CaptureArchiveAt4180C0(0); }
std::int32_t RootScriptFileCommandSource::ResolveAt498020(const IEnvironmentArchiveRuntime*, const char* name)
{
    // 498020只查EXE静态名字表，不解引用nullable slot0 receiver。
    return ResolveScriptNamedValueNativeAt498020(name, tables_, strings_);
}
ScrResourceNameReceiver RootScriptFileCommandSource::CapturePatternNames()
{ return names_.CapturePatternNames(); }
}
