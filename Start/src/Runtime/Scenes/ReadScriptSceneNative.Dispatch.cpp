#include "ReadScriptSceneNative.Internal.h"
#include <Runtime/GameBuildProfile.h>
#include <stdexcept>
#include <exception>
#include <new>

namespace KaGuYa::Start::Runtime
{
IReadScriptMenuServices& IReadScriptSceneNativeSource::CommandMenu()
{ throw std::logic_error("Command-menu workflow is not provided by this source"); }
namespace
{
using namespace ReadScriptSceneNativeDetail;
struct TaskTable final
{
    decltype(Table::runAt00) run;
    decltype(Table::deleteAt04) destroy;
};
[[noreturn]] void PureTaskRun() { _purecall(); std::terminate(); }
std::uint8_t __fastcall PureRun(Body*, void*, std::int32_t) { PureTaskRun(); }
Body* __fastcall DeleteTask(Body* body, void*, std::uint32_t flags) noexcept;
// 原生x86虚槽桥接：对象走ECX，额外void*只占EDX且不消费，其余参数/返回与原槽对应。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const TaskTable task{
    reinterpret_cast<decltype(TaskTable::run)>(&PureRun),
    reinterpret_cast<decltype(TaskTable::destroy)>(&DeleteTask)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
Body* __fastcall DeleteTask(Body* body, void*, std::uint32_t flags) noexcept
{
    RetireTask(*body);
    if ((flags & 1u) != 0) ::operator delete(body, std::size_t{4});
    return body;
}
// 逐条保留4C3630：只写AL，不归一化EAX；Update返回后晚读真实vptr，ret4。
__declspec(naked) std::uint8_t __fastcall RawRun(Body*, void*, std::int32_t)
{
    __asm {
        push esi
        mov esi, ecx
        mov eax, [esi]
        call dword ptr [eax+8]
        mov eax, [esi]
        mov ecx, esi
        call dword ptr [eax+0Ch]
        mov al, 1
        pop esi
        ret 4
    }
}
Body* __fastcall RawDelete(Body* body, void*, std::uint32_t flags) noexcept
{ return DeleteReadScriptSceneNativeAt4A6F60(body, flags, ActiveServices()); }
void __fastcall RawUpdate(Body* body, void*)
{ UpdateReadScriptSceneNativeAt4BA530(*body, ActiveServices()); }
void __fastcall RawDraw(Body* body, void*)
{ DrawReadScriptSceneNativeAt4B5A20(*body, ActiveServices()); }
// 原生x86虚槽桥接：对象走ECX，额外void*只占EDX且不消费，其余参数/返回与原槽对应。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const Table methods{
    reinterpret_cast<decltype(Table::runAt00)>(&RawRun),
    reinterpret_cast<decltype(Table::deleteAt04)>(&RawDelete),
    reinterpret_cast<decltype(Table::updateAt08)>(&RawUpdate),
    reinterpret_cast<decltype(Table::drawAt0C)>(&RawDraw)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
void ReportError(ReadScriptSceneNativeServices& services, const char* message)
{
    const auto target = services.source.CaptureErrorTargetAt4180C0();
    static_cast<void>(services.source.MessageBoxErrorA(target.window, message, target.caption, 0x10));
    // MessageBox返回后另一次413BE0；消息或清理抛出不被同一Update try再捕获。
    services.source.CleanupCurrentEnvironmentAt4AC6E0();
}
}
namespace ReadScriptSceneNativeDetail
{
void RetireTask(Body& body) noexcept { body.vptrAt00 = reinterpret_cast<const Table*>(&task); }
void DispatchUpdateStep(Body& body, std::uint8_t& retainedAl, ReadScriptSceneNativeServices& services)
{
    OwnedStateView state(body);
    ReadScriptEffectOwnerNative effects(body.effectsAt1C);
    const auto main = *static_cast<const volatile std::int32_t*>(&body.mainStateAt04);
    switch (main)
    {
    case 0: retainedAl = RunVm(state, services.source); break;
    case 1: retainedAl = RunEffect(state, effects, services.source); break;
    case 2: retainedAl = RunSuspend(state, effects, services.source); break;
    case 3: retainedAl = RunText(state, body, services.source); break;
    case 4: retainedAl = RunMenu(state, body, services.source); break;
    case 5: retainedAl = RunWait(state, services.source); break;
    case 7:
        if (CompiledGameProfile().commandSelection) retainedAl = RunCommandMenu(state, body, services.source);
        break;
    default: throw 1;
    }
}
bool RunVm(OwnedStateView& state, IReadScriptSceneNativeSource& source)
{
    // 4BFFDC/4BFFE7/4BFFEE由执行叶固定同一Script；4BFFD0没有额外preparer调用。
    // 不走现代DispatchOneStep，也不在完整4D3FF0之前另做一次PC/snapshot准备。
    const auto result = source.ExecuteCurrentScriptAt4D3FF0();
    if (result < 2 || result > 16)
    {
        // 5B53BC、5F0F70 attr1：构造本body的CString，实际抛const char*。
        constexpr char format[] = "ReadScript\x83\x52\x83\x7D\x83\x93\x83\x68\x83\x47\x83\x89\x81\x5B : %d";
        FormatRuntimeCStringAt417130(state.errorMessageAtB0, format, result);
        throw state.errorMessageAtB0.GetString();
    }
    ScrVmYieldDispatcher dispatcher(state, source.VmYield());
    return dispatcher.DispatchResult(result);
}
}
const Table& ReadScriptSceneNativeMethods() noexcept { return methods; }
std::uint8_t RunReadScriptSceneNativeAt4C3630(Body& body, std::int32_t argument,
    ReadScriptSceneNativeServices& services)
{
    ReadScriptSceneNativeScope scope(services);
    EffectCallScope calls(services);
    return methods.runAt00(&body, argument);
}
void UpdateReadScriptSceneNativeAt4BA530(Body& body, ReadScriptSceneNativeServices& services)
{
    ReadScriptSceneNativeScope scope(services);
    EffectCallScope calls(services);
    try
    {
        std::uint8_t retainedAl = 0;
        do { DispatchUpdateStep(body, retainedAl, services); } while (retainedAl != 0);
    }
    // 原5F09E4：HT_IsConst=1、.PAD；char*/const char*/typed-null均命中第一项。
    // 这里不补做nativeParse/UserCode临时清理；实际叶的EH展开先于进入handler。
    catch (const char* message) { ReportError(services, message); }
    catch (...) { ReportError(services, "ReadScript syntax error!"); }
}
void DrawReadScriptSceneNativeAt4B5A20(Body& body, ReadScriptSceneNativeServices& services)
{
    if (*static_cast<const volatile std::uint8_t*>(&body.drawEnabledAt08) == 0) return;
    ReadScriptSceneNativeScope scope(services);
    EffectCallScope calls(services);
    auto& source = services.source;
    if (!source.SuppressFrameAdvanceAt4C3350())
        source.DrawLayers().CaptureDrawLayersAt413BE0().AdvanceFramesAt46B240();
    // 四次独立捕获；WindowZero的effect/dirty和真实GUI vptr由既有原生view消费。
    source.DrawLayers().CaptureDrawLayersAt413BE0().WindowZero().AdvanceEffectAndQueueDirtyAt4B5AC5();
    source.DrawLayers().CaptureDrawLayersAt413BE0().WindowZero().PropagateDirtyRectsAt4C4250();
    source.DrawLayers().CaptureDrawLayersAt413BE0().RegisteredLayers().RenderDirtyRects(source.Screen());
}
}
