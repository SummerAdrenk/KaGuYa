#pragma once

#include <Runtime/ScrContainer.h>
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;

// 4D402A/4D420E先捕获root，随后才47BDB0；后一次调用在捕获staged PC之后。
struct ScriptExecutionRootReceiver final
{
    void* object;
    std::uint32_t& (*programCounterAt47BDB0)(void*);
};
class IScriptExecutionRootSource
{
public:
    virtual ~IScriptExecutionRootSource() = default;
    virtual ScriptExecutionRootReceiver CaptureAfterEnsureAt413BE0() = 0;
};
class ScriptExecutionRootSource final : public IScriptExecutionRootSource
{
public:
    explicit ScriptExecutionRootSource(RootRuntimeOwner& root) noexcept : root_(root) {}
    ScriptExecutionRootReceiver CaptureAfterEnsureAt413BE0() override;
private:
    RootRuntimeOwner& root_;
};

// 这里只传槽号，不把先前取指快照当成原handler的参数。
// 原CScript+2C的函数表及各handler自身的命令Ref装配仍由调用者提供。
class IScriptNativeCommandDispatch
{
public:
    virtual ~IScriptNativeCommandDispatch() = default;
    virtual std::int32_t DispatchAt4D4200(std::uint32_t index) = 0;
};

struct ScriptSingleCommandNative final { const std::byte* source; };
static_assert(sizeof(ScriptSingleCommandNative) == 4);

// 不接管传入正文的分配失败回收；4C8490在control失败时留下已发布base。
void CreateScriptSingleCommandReferenceAt4C8490(YtlReferenceSlotRuntime& slot,
    ScriptSingleCommandNative* body, std::uint8_t owning,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
// 原循环用ESI捕获control，Delete不重读slot；5F18E4三条unwind均为terminate。
void ReleaseScriptDispatchCommandAt4D40C6(YtlReferenceSlotRuntime& slot) noexcept;

// 负target跳过staged处的一条记录，只取length；非负target直接写PC且不分配。
void ApplyScriptProgramCounterTargetAt4D6E90(ScrContainer& parser, std::int32_t target,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

[[nodiscard]] std::int32_t ExecuteScriptLoopAt4D3FF0(ScrContainer& parser,
    IScriptExecutionRootSource& root, IScriptNativeCommandDispatch& commands,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
}
