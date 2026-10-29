#include <Runtime/ScriptDateCommandNative.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/DrawLayersView.h>
#include <Runtime/GameBuildProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
DrawLayersView Layers(void* object)
{
    // 4D1D33/4D1D7A只借本次捕获root的当前+18，不在回调内再次Ensure。
    return static_cast<RootRuntimePayload*>(object)->PostWindow().DrawLayersAt18();
}
}

std::int32_t ExecuteScriptDateWindowAt4D1CE0(IScriptDateCommandSource& source,
    IScrVmCommandErrorSink& errors)
{
    const auto first = source.CaptureAfterEnsureAt413BE0();
    if (first.windowEnabledAt4D1D33(first.object) != 0)
    {
        const auto second = source.CaptureAfterEnsureAt413BE0();
        if (second.patternEnabledAt4D1D7A(second.object) != 0)
            static_cast<void>(source.CaptureAfterEnsureAt413BE0());
    }
    // 4D1D8D先slot0后root；4D1E06写入阶段顺序相反，且不增持旧receiver。
    const auto* replayTable = source.CaptureSlot0At4180C0();
    const auto replayRoot = source.CaptureAfterEnsureAt413BE0();
    const auto replayIndex = source.ResolveAt498020(replayTable, "replay");
    const auto replay = ReadScriptValueAt4C73A0(replayRoot.paramsAt47BDB0(replayRoot.object), replayIndex, errors);

    const auto dateRoot = source.CaptureAfterEnsureAt413BE0();
    const auto* dateTable = source.CaptureSlot0At4180C0();
    const auto dateIndex = source.ResolveAt498020(dateTable, "date_win");
    WriteScriptValueAt4C8220(dateRoot.paramsAt47BDB0(dateRoot.object), dateIndex, replay != 0 ? -1 : 0, errors);
    return 0;
}

std::int32_t ExecuteScriptDatePlaceAt4D1BF0(IScriptDateCommandSource& source,
    IScrVmCommandErrorSink& errors)
{
    for (const char* name : {"date_win", "place"})
    {
        const auto* table = source.CaptureSlot0At4180C0();
        const auto root = source.CaptureAfterEnsureAt413BE0();
        const auto index = source.ResolveAt498020(table, name);
        WriteScriptValueAt4C8220(root.paramsAt47BDB0(root.object), index, -1, errors);
    }
    // Ero Zemi 4C6C10：旧版有实体日期层，仍显示时返回效果请求，不能只清变量。
    if (CompiledGameProfile().dateWindow)
    {
        const auto root = source.CaptureAfterEnsureAt413BE0();
        if (!root.dateWindowEnabled) throw std::logic_error("DATE_PLACE lacks its configured date layer source");
        if (root.dateWindowEnabled(root.object)) return 3;
    }
    return 0;
}

RootScriptDateCommandSource::RootScriptDateCommandSource(RootRuntimeOwner& root,
    EnvironmentStartupRuntime& environment, ScriptNamedValueNativeTables tables,
    IRuntimeStringManagerSource& strings) noexcept
    : root_(root), environment_(environment), tables_(tables), strings_(strings) {}

ScriptDateRootReceiver RootScriptDateCommandSource::CaptureAfterEnsureAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return {&root_.RequirePublished(), [](void* object) { return Layers(object).LayerAt418C10(0).EnabledByteAt0C(); },
        [](void* object) {
            return Layers(object).WindowZero().PatternEnabledAt4AD2A0(0);
        }, [](void* object) -> ScrVmParamsGeneration& {
            return static_cast<RootRuntimePayload*>(object)->ParamsAt47BDB0();
        }, [](void* object) -> std::uint8_t {
            return CompiledGameProfile().dateWindow ? Layers(object).LayerAt418C10(5).EnabledByteAt0C() : 0;
        }};
}
const IEnvironmentArchiveRuntime* RootScriptDateCommandSource::CaptureSlot0At4180C0()
{ return environment_.Archives().CaptureArchiveAt4180C0(0); }
std::int32_t RootScriptDateCommandSource::ResolveAt498020(const IEnvironmentArchiveRuntime*, const char* name)
{
    // 498020不解引用receiver；保留取源时点，null不变成缺表异常。
    return ResolveScriptNamedValueNativeAt498020(name, tables_, strings_);
}
}
