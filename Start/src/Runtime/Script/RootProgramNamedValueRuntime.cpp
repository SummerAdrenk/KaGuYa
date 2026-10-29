#include <Runtime/RootProgramNamedValueRuntime.h>

#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/ScrVmValueTable.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
[[nodiscard]] const char* NamedValueName(const ScrVmProgramNamedValue name)
{
    switch (name)
    {
    case ScrVmProgramNamedValue::Replay: return "replay";
    case ScrVmProgramNamedValue::ProgResult: return "prog_result";
    case ScrVmProgramNamedValue::ProgParam00: return "prog_param00";
    case ScrVmProgramNamedValue::ProgParam01: return "prog_param01";
    case ScrVmProgramNamedValue::RestartPoint: return "restart_point";
    case ScrVmProgramNamedValue::GagueDisp: return "gague_disp";
    case ScrVmProgramNamedValue::MasoGaugeDisplayFlag:
        // 4D063D 的 5B6028 查询串不属于 498020 的 41+45 项表，仍须实际解析出 -1。
        return "\x96\x82\x91\x66\x83\x51\x81\x5B\x83\x57\x81\x40"
            "\x95\x5C\x8E\xA6\x83\x74\x83\x89\x83\x4F";
    }
    throw ScrVmProgramCommandException(ScrVmProgramError::RuntimeBoundaryFailure,
        "unknown program named-value selector");
}

[[nodiscard]] const IEnvironmentArchiveRuntime* BorrowSlot0(EnvironmentStartupRuntime& environment)
{
    // 417990 缺槽/base=0 返回 null；只借 raw，不检查 ready，也不复制 shared 保活。
    return environment.Archives().CaptureArchiveAt4180C0(0);
}
}

RootProgramNamedValueRuntime::RootProgramNamedValueRuntime(EnvironmentStartupRuntime& environment,
    RootRuntimeOwner& root, ScriptNamedValueNativeTables tables,
    decltype(&::MessageBoxA) messageBox, IRuntimeStringManagerSource& strings) noexcept
    : environment_(environment), root_(root), tables_(tables), messageBox_(messageBox), strings_(strings)
{
}

RootProgramNamedValueRuntime::NamedTarget RootProgramNamedValueRuntime::PrepareNamedValue(
    const ScrVmProgramNamedValue name) const
{
    return PrepareNamedValue(NamedValueName(name), name == ScrVmProgramNamedValue::RestartPoint);
}

RootProgramNamedValueRuntime::NamedTarget RootProgramNamedValueRuntime::PrepareNamedValue(
    const char* name, const bool slotFirst) const
{
    // 4D4F4D/417B58先slot0，419A22先root；查表及GetValue错误沿用同一条原生链。
    const auto* const earlySystemData = slotFirst ? BorrowSlot0(environment_) : nullptr;
    root_.EnsureCreatedAt413BE0();
    RootRuntimePayload& capturedRoot = root_.RequirePublished();
    // 其余 PROGRAM 分支均为 root 先行，如 4D062D/4D0632 -> 4D0646。
    const auto* const systemData = slotFirst ? earlySystemData : BorrowSlot0(environment_);
    // 498020 不解引用 receiver；保留获取时点，null 不能变成 ready 或缺表错误。
    static_cast<void>(systemData);
    const auto index = ResolveScriptNamedValueNativeAt498020(name, tables_, strings_);
    // 4D0657/4D4FB3：对已捕获 root 取得 CParams 后才进入 Get/Set 的 -1 检查。
    return {capturedRoot.ParamsAt47BDB0(), index};
}

std::int32_t& RootProgramNamedValueRuntime::RequireValue(ScrVmParamsGeneration& params,
    const std::int32_t index, const char* const errorText) const
{
    if (index == -1)
    {
        // 4C73EC..4C7411 / 4C8275..4C829A：420C50 晚取标题 raw、HWND，提示后抛 int(1)。
        const auto& errorEnvironment = environment_.CaptureAt4180C0();
        const char* const caption = errorEnvironment.titleAt10.GetString();
        const HWND window = reinterpret_cast<HWND>(errorEnvironment.windowAt2C);
        static_cast<void>(messageBox_(window, errorText, caption, MB_ICONERROR));
        throw 1;
    }
    auto& values = params.SaveParams2At30().values;
    // 原 4C73A0/4C8220 仅拒绝 -1；其余越界是现代安全保护，不冒充原提示/int 异常。
    if (index < 0 || static_cast<std::size_t>(index) >= values.size())
        throw ScrVmValueException(ScrVmValueError::IndexOutOfRange, index,
            "PROGRAM named value index is outside the 512-slot table");
    return values[static_cast<std::size_t>(index)];
}

std::int32_t RootProgramNamedValueRuntime::ReadNamedValue(const ScrVmProgramNamedValue name) const
{
    const auto target = PrepareNamedValue(name);
    return RequireValue(target.params, target.index, "GetValue over num");
}

std::int32_t RootProgramNamedValueRuntime::ReadAfterRootAt4C73A0(const char* name) const
{
    const auto target = PrepareNamedValue(name, false);
    return RequireValue(target.params, target.index, "GetValue over num");
}

std::int32_t RootProgramNamedValueRuntime::ReadAfterSlot0At4C73A0(const char* name) const
{
    const auto target = PrepareNamedValue(name, true);
    return RequireValue(target.params, target.index, "GetValue over num");
}

void RootProgramNamedValueRuntime::WriteNamedValue(const ScrVmProgramNamedValue name,
    const std::int32_t value)
{
    const auto target = PrepareNamedValue(name);
    RequireValue(target.params, target.index, "SetValue over num") = value;
}

void RootProgramNamedValueRuntime::WriteProgResultFromRegistrySlot1ByteAt6F()
{
    // 4D01B2/4D01B7 先捕获 root；4D01BD 保存的稳定 Environment 稍后才用于 slot1。
    root_.EnsureCreatedAt413BE0();
    RootRuntimePayload& capturedRoot = root_.RequirePublished();
    EnvironmentRootStorage& valueEnvironment = environment_.CaptureAt4180C0();
    const auto* const systemData = BorrowSlot0(environment_);
    static_cast<void>(systemData);
    const auto index = ResolveScriptNamedValueNativeAt498020("prog_result", tables_, strings_);
    auto& params = capturedRoot.ParamsAt47BDB0();

    // 4D01E4 取得目标 CParams 后，4D01F0/4D01F7 才取当前 slot1 并读真实 CSaveData+6F。
    const auto* const save = static_cast<SaveDataEnvironmentArchive*>(
        ResolveEnvironmentArchiveAt417990(valueEnvironment, 1));
    // 原 null receiver 随后被解引用；只在消费点明确作现代安全拒绝，不补默认值。
    if (!save)
        throw std::logic_error("PROGRAM command 10 requires Environment slot 1 SaveData (modern safety boundary)");
    const std::int32_t value = save->valueAt6F != 0 ? 1 : 0;
    // 4D0203 的 -1 检查晚于 +6F 读取；写回已取得的 params，不重新 Current。
    RequireValue(params, index, "SetValue over num") = value;
}
}
