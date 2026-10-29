#include <Runtime/RootSceneReplayRuntime.h>

#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ScrVmValueTable.h>

namespace KaGuYa::Start::Runtime
{
RootSceneReplayRuntime::RootSceneReplayRuntime(EnvironmentStartupRuntime& environment,
    RootRuntimeOwner& root, ScriptNamedValueNativeTables tables,
    decltype(&::MessageBoxA) messageBox, IRuntimeStringManagerSource& strings) noexcept
    : environment_(environment), root_(root), tables_(tables), messageBox_(messageBox), strings_(strings)
{
}

std::int32_t RootSceneReplayRuntime::ReadReplayValue()
{
    // 4D0CCA..4D0D0F：先取slot0。417990的缺槽/base空是null，不作ready检查或保活。
    const auto* const systemData = environment_.Archives().CaptureArchiveAt4180C0(0);

    root_.EnsureCreatedAt413BE0();
    RootRuntimePayload& capturedRoot = root_.RequirePublished();
    // 4D0D28调用498020，但498020不解引用receiver，仅查EXE静态41+45表。
    // 不能把上面的slot获取移到解析后，也不能用ReadyArchive添加原本没有的拒绝。
    static_cast<void>(systemData);
    const auto index = ResolveScriptNamedValueNativeAt498020("replay", tables_, strings_);
    // 4D0D30对已捕获根调用47BDB0，不再次解析singleton；-1检查在此后才发生。
    auto& params = capturedRoot.ParamsAt47BDB0();
    if (index == -1)
    {
        // 4C73EC..4C7411 -> 420C50：当前Environment标题/窗口、提示后抛int(1)。
        const auto& errorEnvironment = environment_.CaptureAt4180C0();
        const char* const caption = errorEnvironment.titleAt10.GetString();
        const HWND window = reinterpret_cast<HWND>(errorEnvironment.windowAt2C);
        static_cast<void>(messageBox_(window, "GetValue over num", caption, MB_ICONERROR));
        throw 1;
    }
    const auto& values = params.SaveParams2At30().values;
    // 原4C73A0未做此范围保护；现代仅明确拒绝，不取低位、不扩容或补默认值。
    if (index < 0 || static_cast<std::size_t>(index) >= values.size())
        throw ScrVmValueException(ScrVmValueError::IndexOutOfRange, index,
            "SCENE replay index is outside the 512-slot table");
    return values[static_cast<std::size_t>(index)];
}
}
