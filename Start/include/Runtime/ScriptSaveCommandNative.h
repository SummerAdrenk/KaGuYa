#pragma once

#include <Runtime/ScriptDispatchNative.h>

namespace KaGuYa::Start::Runtime
{
class RootProgramNamedValueRuntime;
class ScrVmScriptCommandState;
class ScrScriptRuntimeState;

struct ScriptSaveRootReceiver final
{
    void* object;
    std::uint32_t (*readPcAt47BDB0)(void*);
    void (*writeSlotAt47BDB0)(void*, std::int32_t);
};

class IScriptSaveCommandSource
{
public:
    virtual ~IScriptSaveCommandSource() = default;
    virtual std::uint32_t ReadFlagsAt04() = 0;
    virtual std::int32_t ReadReplayAt4D0AF3() = 0;
    virtual void WriteGateAt25(bool value) = 0;
    virtual ScriptSaveRootReceiver CaptureAfterEnsureAt413BE0() = 0;
};

// 原根的生产取源；门控仍借现有CScript状态，完整CCh拥有链不在本类型内伪造。
class RootScriptSaveCommandSource final : public IScriptSaveCommandSource
{
public:
    RootScriptSaveCommandSource(RootRuntimeOwner& root, RootProgramNamedValueRuntime& named,
        const ScrVmScriptCommandState& command, ScrScriptRuntimeState& state) noexcept;
    std::uint32_t ReadFlagsAt04() override;
    std::int32_t ReadReplayAt4D0AF3() override;
    void WriteGateAt25(bool value) override;
    ScriptSaveRootReceiver CaptureAfterEnsureAt413BE0() override;
private:
    RootRuntimeOwner& root_;
    RootProgramNamedValueRuntime& named_;
    const ScrVmScriptCommandState& command_;
    ScrScriptRuntimeState& state_;
};

struct ScriptSaveCommandNative final { const std::byte* source; };
static_assert(sizeof(ScriptSaveCommandNative) == 4);

// 4D0BC7..4D0C0B：control发布在字段初始化之间，与4C8490不同。
void CreateScriptSaveCommandReferenceAt4D0BAE(YtlReferenceSlotRuntime& slot,
    ScriptSaveCommandNative* body, IEnvironmentRootMemory& memory);
// 异常路径4C9800重新取slot；正常4D0C60捕获ESI，不能合并两个释放规则。
void UnwindScriptSaveCommandAt4C9800(YtlReferenceSlotRuntime& slot) noexcept;
[[nodiscard]] std::int32_t ExecuteScriptSaveAt4D0AC0(ScrContainer& parser,
    IScriptSaveCommandSource& source,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
}
