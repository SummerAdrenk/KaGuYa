#pragma once

#include <Runtime/DrawLayersView.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
struct ScriptNativeBody;
struct ScriptParamsNativeBody;
class IEnvironmentArchiveRuntime;

// 只固定413BE0返回后读取的6198C4；两个getter延后执行，不缓存Params/Draw正文。
// getter不得重新Ensure或改用当前发布根。draw getter只解本次根+18原Ref，不调用47E940。
struct RestoredScriptNativeRootReceiver final
{
    void* object;
    ScriptParamsNativeBody* (*paramsAt47BDB0)(void*);
    DrawLayersView (*drawLayersAt18)(void*);
};

// 生产源的纯接口：不拥有Script/Params镜像，也不借现代恢复事务补齐业务。
// 所有异常原样外传；捕获/Get/包装器不得追加ready、loaded或非负索引保护。
class IRestoredScriptNativeServices
{
public:
    virtual ~IRestoredScriptNativeServices() = default;

    // 每次恰好413BE0一次，随后读取当前6198C4，返回固定根及延后getter。
    [[nodiscard]] virtual RestoredScriptNativeRootReceiver CaptureAfterEnsureAt413BE0() = 0;
    // 每次恰好4180C0一次；417990由内核执行，不能返回预先固定的归档正文。
    [[nodiscard]] virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0() = 0;

    // 4D3C90必须装载传入receiver的parserAtA0并保留原叶的错误显示/副作用。
    // 原调用者只检查AL，故返回BYTE而不是整个EAX或现代switch-result。
    [[nodiscard]] virtual std::uint8_t LoadScriptAt4D3C90(
        ScriptNativeBody& receiver, std::int32_t scriptId) = 0;
    // 只对已选view调用ResetOwnedLayersAt47E940(原Graphics服务)，不得再取根。
    virtual void ResetDrawOwnersAt47E940(DrawLayersView receiver) = 0;

    // 名字实参已读取；各包装器内部才4182F0，并先门控module Ref的base。
    // 复用NativeGraphicsRuntimeLibrary::SetTextFirst/SecondName，不预查ModuleLoaded。
    virtual void SetGraphicsTextFirstNameAt4D6CD0(const char* name) = 0;
    virtual void SetGraphicsTextSecondNameAt4D6DB0(const char* name) = 0;

    // 498020不读取ECX，但仍保留slot0选择/调用时序。适配器复用
    // ResolveScriptNamedValueNativeAt498020；字符串manager回调可改换当前根/Env。
    [[nodiscard]] virtual std::int32_t ResolveNamedValueAt498020(
        IEnvironmentArchiveRuntime* receiver, const char* name) = 0;
    // receiver是固定根上延后47BDB0的结果，不许再次捕获根。对498020返回域
    // [-1,85]复用WriteScriptValueAt4C8220；-1保留MessageBox及throw int(1)。
    virtual void WriteValueAt4C8220(ScriptParamsNativeBody& receiver,
        std::int32_t index, std::int32_t value) = 0;
    // 完整4D3FF0循环，直到非零route；必须消费同一CScript而非另取Script。
    [[nodiscard]] virtual std::int32_t DispatchAt4D3FF0(ScriptNativeBody& receiver) = 0;

    // 非原业务字段：raw写入之后立即OR对应ScrVmScriptCommandState mask。
    // 只更新所选receiver的外部合法性记录，不取根、不读写业务值、不触发回调或抛出。
    // 生产源须预先绑定选定payload.ScriptPublication()；即使当前根被替换也不能重绑。
    // 独立raw夹具允许不提供记录；此空默认不代表任何未实现的生产业务叶。
    virtual void OnPublishCommandFields(ScriptNativeBody&, unsigned) noexcept {}
};

// 原4D4CC0恢复内核；raw SAVE访问刻意不借SaveOffsetAt/LoadedScript安全边界。
[[nodiscard]] std::int32_t RestoreScriptNativeAt4D4CC0(
    ScriptNativeBody& receiver, IRestoredScriptNativeServices& services);
}
