#pragma once

#include <Runtime/ScriptDispatchNative.h>

namespace KaGuYa::Start::Runtime
{
class ScrScriptRuntimeState;
class ScrVmValueTable;
class ScrVmCallStack;
class ScrVmScriptStorage;
class IScrVmSceneReplaySource;
class IScrVmSceneSource;
class IScrVmSceneCommandErrorSink;

// Ensure/capture与Params访问分开：CALL在两者之间读取staged PC。
struct ScriptControlRootReceiver final
{
    void* object;
    std::uint32_t (*readPcAt47BDB0)(void*);
    void (*writeValueAt47BDB0)(void*, std::uint16_t, std::int32_t);
    void (*pushReturnAt47BDB0)(void*, std::uint32_t);
    std::uint32_t (*popReturnAt47BDB0)(void*);
    std::int32_t (*readValueAt47BDB0)(void*, std::uint16_t);
};
class IScriptControlCommandSource
{
public:
    virtual ~IScriptControlCommandSource() = default;
    virtual ScriptControlRootReceiver CaptureAfterEnsureAt413BE0() = 0;
};
class RootScriptControlCommandSource final : public IScriptControlCommandSource
{
public:
    explicit RootScriptControlCommandSource(RootRuntimeOwner& root) noexcept : root_(root) {}
    ScriptControlRootReceiver CaptureAfterEnsureAt413BE0() override;
private:
    RootRuntimeOwner& root_;
};

// 独立VM的明确绑定入口；不用它代替完整生产root拥有链。
class BoundScriptControlCommandSource final : public IScriptControlCommandSource
{
public:
    BoundScriptControlCommandSource(ScrScriptRuntimeState& runtime,
        ScrVmValueTable* values = nullptr, ScrVmCallStack* stack = nullptr) noexcept
        : runtime_(runtime), values_(values), stack_(stack), readValues_(values) {}
    BoundScriptControlCommandSource(ScrScriptRuntimeState& runtime, const ScrVmValueTable& values) noexcept
        : runtime_(runtime), values_(nullptr), stack_(nullptr), readValues_(&values) {}
    ScriptControlRootReceiver CaptureAfterEnsureAt413BE0() override;
private:
    ScrScriptRuntimeState& runtime_;
    ScrVmValueTable* values_;
    ScrVmCallStack* stack_;
    const ScrVmValueTable* readValues_;
};

struct ScriptControlCommandNative final { const std::byte* source; };
static_assert(sizeof(ScriptControlCommandNative) == 4);
enum class ScriptControlCommandKind { Flag, LabelJump, Conditional, Wait, Assign, Compare, Title, Scene, FileJump, Update, Music, Sound, Voice, Text, Menu, Program };
// 4C8340/4C8420/4C83B0：版式相同，保留三个不同的原control动态类型。
void CreateScriptControlCommandReference(YtlReferenceSlotRuntime& slot,
    ScriptControlCommandNative* body, ScriptControlCommandKind kind,
    IEnvironmentRootMemory& memory);
void UnwindScriptControlCommand(YtlReferenceSlotRuntime& slot) noexcept;
[[nodiscard]] std::int32_t ExecuteScriptSetFlagAt4CE2A0(ScrContainer& parser,
    IScriptControlCommandSource& source, IEnvironmentRootMemory& memory);
[[nodiscard]] std::int32_t ExecuteScriptClearFlagAt4CE160(ScrContainer& parser,
    IScriptControlCommandSource& source, IEnvironmentRootMemory& memory);
[[nodiscard]] std::int32_t ExecuteScriptJumpAt4CE7B0(ScrContainer& parser,
    IScriptControlCommandSource& source, IEnvironmentRootMemory& memory);
[[nodiscard]] std::int32_t ExecuteScriptCallAt4CEC40(ScrContainer& parser,
    IScriptControlCommandSource& source, IEnvironmentRootMemory& memory);
[[nodiscard]] std::int32_t ExecuteScriptReturnAt4D0A90(ScrContainer& parser,
    IScriptControlCommandSource& source);
[[nodiscard]] std::int32_t ExecuteScriptIfTrueAt4CEA90(ScrContainer& parser,
    IScriptControlCommandSource& source, IEnvironmentRootMemory& memory);
[[nodiscard]] std::int32_t ExecuteScriptIfFalseAt4CE8E0(ScrContainer& parser,
    IScriptControlCommandSource& source, IEnvironmentRootMemory& memory);
[[nodiscard]] std::int32_t ExecuteScriptWaitAt4D11D0(ScrContainer& parser,
    IScriptControlCommandSource& source, ScrVmScriptStorage& storage, IEnvironmentRootMemory& memory);
[[nodiscard]] std::int32_t ExecuteScriptSceneAt4D0CA0(ScrContainer& parser,
    IScriptControlCommandSource& pcSource, IScrVmSceneReplaySource& replay,
    IScrVmSceneSource& scene, IScrVmSceneCommandErrorSink& errors, IEnvironmentRootMemory& memory);
}
