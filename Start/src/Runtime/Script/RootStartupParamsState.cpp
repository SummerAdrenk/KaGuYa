#include <Runtime/RootStartupParamsState.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ScrVmValueTable.h>

namespace KaGuYa::Start::Runtime
{
RootStartupParamsState::RootStartupParamsState(EnvironmentStartupRuntime& environment, RootRuntimeOwner& root,
    ScriptNamedValueNativeTables tables, decltype(&::MessageBoxA) messageBox, IRuntimeStringManagerSource& strings) noexcept
    : environment_(environment), root_(root), tables_(tables), messageBox_(messageBox), strings_(strings) {}

void RootStartupParamsState::ReplaceParamsGeneration()
{
    // 4D53DD/4D53E2/4D53E8：创建/捕获root，然后对该receiver执行481720。
    root_.EnsureCreatedAt413BE0();
    root_.RequirePublished().ReplaceParamsAt481720();
}
void RootStartupParamsState::WriteNamedValue(RootRuntimePayload& capturedRoot, const char* name, std::int32_t value)
{
    // 498020不解引用slot0，但417990仍必须执行；CParams取得晚于名称查询。
    static_cast<void>(environment_.Archives().CaptureArchiveAt4180C0(0));
    const auto index = ResolveScriptNamedValueNativeAt498020(name,tables_,strings_);
    auto& params = capturedRoot.ParamsAt47BDB0();
    if (index == -1)
    {
        // 4C8275..4C829A：晚取同一个Environment的标题/HWND，提示后抛int(1)。
        const auto& error = environment_.CaptureAt4180C0();
        static_cast<void>(messageBox_(reinterpret_cast<HWND>(error.windowAt2C),
            "SetValue over num", error.titleAt10.GetString(), MB_ICONERROR));
        throw 1;
    }
    auto& values = params.SaveParams2At30().values;
    if (index < 0 || static_cast<std::size_t>(index) >= values.size())
        throw ScrVmValueException(ScrVmValueError::IndexOutOfRange, index,
            "startup named value is outside the 512-slot table (modern safety boundary)");
    values[static_cast<std::size_t>(index)] = value;
}
void RootStartupParamsState::CopyRegistryPrologueFlag()
{
    // 4D5506/4D550B固定目标root；4D5513读取slot7，再4D552B取slot0，不能反序。
    root_.EnsureCreatedAt413BE0();
    auto& capturedRoot = root_.RequirePublished();
    const auto value = environment_.Archives().GlobalParams().wordsAt0C[0];
    WriteNamedValue(capturedRoot, "prologue_flag", value);
}
void RootStartupParamsState::WriteReplayFile(std::int32_t value)
{
    root_.EnsureCreatedAt413BE0();
    WriteNamedValue(root_.RequirePublished(), "replay_file", value);
}
void RootStartupParamsState::WriteReplayPosition(std::int32_t value)
{
    root_.EnsureCreatedAt413BE0();
    WriteNamedValue(root_.RequirePublished(), "replay_position", value);
}
void RootStartupParamsState::ClearHistoryAt4D5730()
{
    // 4D571B..4D5730：重新捕获root和CParams，仅将end还原begin，保留容量。
    root_.EnsureCreatedAt413BE0();
    root_.RequirePublished().ParamsAt47BDB0().historyAt3C.clear();
}
}
