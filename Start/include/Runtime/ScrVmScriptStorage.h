#pragma once

#include <Runtime/ScriptNativeBody.h>
#include <optional>

namespace KaGuYa::Start::Runtime
{
using ScrVmWaitState = ScriptWaitNative;
using ScrVmUpdatePair = ScriptUpdatePairNative;
using ScrVmUpdateEffectState = ScriptEffectNative;

// 两个原Ref是唯一所有权入口；外部借用的分配叶不进入14h/0Ch正文或18h control。
class ScrVmScriptStorage final
{
public:
    // 默认入口只供独立夹具；生产必须借用同一CCh正文及外置合法性记录。
    explicit ScrVmScriptStorage(IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    ScrVmScriptStorage(ScriptNativeBody& body, ScriptNativePublication& publication,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    ~ScrVmScriptStorage() noexcept;
    ScrVmScriptStorage(const ScrVmScriptStorage&) = delete;
    ScrVmScriptStorage& operator=(const ScrVmScriptStorage&) = delete;

    // 显式阶段接口不冒充完整CScript构造EH；失败保留原已执行前缀。
    void CreateEffectAt4C8ECC();
    void CreatePairsAt4C9025(std::int32_t count);
    void CreateWaitAt4C917D();
    void InitializeAt4C8CF0(ParamsRuntimeRootOwner& params);
    [[nodiscard]] ScrVmUpdateEffectState& EffectAt4AD140();
    [[nodiscard]] const ScrVmUpdateEffectState& EffectAt4AD140() const;
    [[nodiscard]] ScrVmWaitState& WaitState();
    [[nodiscard]] const ScrVmWaitState& WaitState() const;
    [[nodiscard]] const YtlReferenceSlotRuntime& EffectReference() const noexcept { return effect_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& WaitReference() const noexcept { return wait_; }
    [[nodiscard]] ScriptNativeBody* BorrowedBody() const noexcept { return body_; }
    [[nodiscard]] ScriptNativePublication& Publication() noexcept { return publication_; }
    [[nodiscard]] const ScriptNativePublication& Publication() const noexcept { return publication_; }
    void Release();

    // 原WAIT是raw DWORD写入。未初始化读取拒绝属于现代安全边界，不在正文中伪造optional。
    // target允许是取值回调之前已捕获的旧正文；它不能发布新wait的字段。
    void WriteWaitValue(ScrVmWaitState& target, unsigned index, std::int32_t value);
    void SetWaitState(ScrVmWaitState value);
    [[nodiscard]] bool HasWaitValue(unsigned index) const;
    [[nodiscard]] std::int32_t ReadWaitValue(unsigned index) const;

private:
    void SynchronizeWaitPublication() const noexcept;
    IEnvironmentRootMemory& memory_;
    IRuntimeStringManagerSource& strings_;
    // 仅默认夹具拥有这两槽；生产引用原位Ref，析构view不减持。
    YtlReferenceSlotRuntime fixtureEffect_, fixtureWait_;
    ScriptNativePublication fixturePublication_;
    ScriptNativeBody* body_ = nullptr;
    YtlReferenceSlotRuntime& effect_;
    YtlReferenceSlotRuntime& wait_;
    ScriptNativePublication& publication_;
};
}
