#pragma once

#include <Runtime/ScriptFollowCommandNative.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/ScriptControlCommandNative.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/ScrVmFileCallHandlers.h>

#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::FileCommandDetail
{
union TextSlot
{
    RuntimeCString value;
    TextSlot() noexcept {}
    ~TextSlot() noexcept {}
};
struct TextUnwind
{
    RuntimeCString* value;
    ~TextUnwind() noexcept { if (value) DestroyRuntimeCStringAt40CD40(value); }
};
struct Command
{
    YtlReferenceSlotRuntime reference;
    std::uintptr_t base = 0;
    std::uint64_t extent = 0;
    bool knownExtent = false;
};
struct CommandUnwind
{
    YtlReferenceSlotRuntime* reference;
    // 4C94F0 EHFlags=5；四个入口正常释放也各有terminate状态。
    ~CommandUnwind() noexcept { UnwindScriptControlCommand(*reference); }
};
Command CreateCommand(ScrContainer& parser, IScriptFileCommandSource& source, IEnvironmentRootMemory& memory,
    ScriptControlCommandKind kind = ScriptControlCommandKind::FileJump);
void Show(IScrVmCommandErrorSink& errors, const char* text);
void ConstructReportAt4CA280(RuntimeCString& out, const char* prefix,
    const RuntimeCString& detail, IRuntimeStringManagerSource& strings);

template<class T> T Operand(const Command& command, std::uint32_t field)
{
    const auto* body = ResolveEnvironmentObjectReference<ScriptControlCommandNative>(command.reference);
    if (!body || !body->source)
        throw std::logic_error("File command null command/source (modern safety boundary)");
    const auto pointer = reinterpret_cast<std::uintptr_t>(body->source);
    const auto offset = static_cast<std::uint32_t>(pointer - command.base);
    if (command.knownExtent && std::uint64_t{offset} + field + sizeof(T) > command.extent)
        throw std::out_of_range("File command operand outside allocation (modern safety boundary)");
    T value;
    std::memcpy(&value, reinterpret_cast<const void*>(pointer + field), sizeof(value));
    return value;
}

// CALL/RTN/EXEC/FOLLOW的共同双CString报告。动作内部仍独立保持各命令的Ref/EH顺序。
template<class Action> std::int32_t ExecuteWithReport(const char* prefix,
    IScrVmCommandErrorSink& errors, IRuntimeStringManagerSource& strings, Action action)
{
    TextSlot detail;
    InitializeEmptyRuntimeCString(detail.value, strings); // 原内层try之前。
    TextUnwind detailUnwind{&detail.value};
    const char* const initialChars = detail.value.GetString();
    bool success = false;
    try { action(); success = true; }
    catch (const char* text)
    {
        SetRuntimeCStringAt437680(detail.value, text, text ? static_cast<int>(std::strlen(text)) : 0);
    }
    catch (...) { SetRuntimeCStringAt437680(detail.value, "syntax error", 12); }
    if (success)
    {
        // 四入口在内层catch之外撤state，再释放最初header；Free失败不重复清理。
        detailUnwind.value = nullptr;
        ReleaseRuntimeCStringHeader(reinterpret_cast<ATL::CStringData*>(const_cast<char*>(initialChars)) - 1);
        return 0;
    }
    TextSlot report;
    ConstructReportAt4CA280(report.value, prefix, detail.value, strings);
    TextUnwind reportUnwind{&report.value};
    const char* const reportChars = report.value.GetString(); // 先取chars，再进入4180C0。
    Show(errors, reportChars);
    // 正常双串销毁时state仍有效：Free抛出须按原展开再清report、detail。
    DestroyRuntimeCStringAt40CD40(&report.value);
    DestroyRuntimeCStringAt40CD40(&detail.value);
    reportUnwind.value = nullptr;
    detailUnwind.value = nullptr;
    return 2;
}
}
