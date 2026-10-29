#pragma once

#include <Runtime/ScrContainer.h>
#include <Runtime/ScriptOwnedStateNative.h>
#include <array>

namespace KaGuYa::Start::Runtime
{
struct ScriptCommandPrefix final
{
    std::uint32_t flags;
    std::uint8_t reload[4];
};
struct ScriptCommandTail final { std::uint8_t bytes[5]; };

// 合法性记录属于现代适配层；不保存业务值，也不进入原CCh正文。
struct ScriptNativePublication final
{
    unsigned commandMask = 0;
    const ScriptWaitNative* waitBody = nullptr;
    unsigned waitMask = 0;
};

struct ScriptNativeBody;
using ScriptNativeEntry = std::int32_t (__thiscall*)(ScriptNativeBody*);
struct ScriptNativeVtable final
{
    ScriptNativeBody* (__thiscall* deletingDestructor)(ScriptNativeBody*, std::uint32_t);
};

struct ScriptNativeBody final
{
    // 显式夹具/分阶段入口：只完成原构造至函数表登记，不能代替47CD00的完整构造。
    struct PrefixOnlyTag final {};
    explicit ScriptNativeBody(PrefixOnlyTag,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    ScriptNativeBody(ParamsRuntimeRootOwner& params, IEnvironmentRootMemory& memory,
        IRuntimeStringManagerSource& strings);
    ~ScriptNativeBody() noexcept;
    ScriptNativeBody(const ScriptNativeBody&) = delete;
    ScriptNativeBody& operator=(const ScriptNativeBody&) = delete;

    const ScriptNativeVtable* vptrAt00;
    ScriptCommandPrefix commandAt04;
    YtlReferenceSlotRuntime effectAt0C;
    YtlReferenceSlotRuntime waitAt18;
    ScriptCommandTail commandAt24;
    std::uint8_t unwrittenAt29[3];
    std::array<ScriptNativeEntry, 29> entriesAt2C;
    union { ScrContainer parserAtA0; };
};
static_assert(sizeof(ScriptNativeEntry) == 4);
static_assert(sizeof(ScriptNativeBody) == 0xCC);
static_assert(offsetof(ScriptNativeBody, commandAt04) == 4);
static_assert(offsetof(ScriptNativeBody, effectAt0C) == 0x0C);
static_assert(offsetof(ScriptNativeBody, waitAt18) == 0x18);
static_assert(offsetof(ScriptNativeBody, commandAt24) == 0x24);
static_assert(offsetof(ScriptNativeBody, entriesAt2C) == 0x2C);
static_assert(offsetof(ScriptNativeBody, parserAtA0) == 0xA0);

// 只给原无参thiscall入口提供外部服务；传入receiver必须实际决定被消费的正文。
class IScriptNativeEntryContext
{
public:
    virtual ~IScriptNativeEntryContext() = default;
    virtual std::int32_t Invoke(ScriptNativeBody& receiver, unsigned opcode) = 0;
};
class ScriptNativeEntryScope final
{
public:
    explicit ScriptNativeEntryScope(IScriptNativeEntryContext& context) noexcept;
    ~ScriptNativeEntryScope() noexcept;
    ScriptNativeEntryScope(const ScriptNativeEntryScope&) = delete;
    ScriptNativeEntryScope& operator=(const ScriptNativeEntryScope&) = delete;
private:
    IScriptNativeEntryContext* previous_;
};

// 此入口不Ensure根；new失败释放CCh，control失败保留已发布base，返回时重解当前槽。
ScriptNativeBody* GetScriptAt47CD00(YtlReferenceSlotRuntime& slot,
    ParamsRuntimeRootOwner& params,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
