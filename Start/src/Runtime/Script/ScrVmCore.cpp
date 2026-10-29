#include <Runtime/ScrVmCore.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
std::int32_t IScrVmInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory&)
{
    const auto* script = runtime.LoadedScript();
    if (!script) throw std::logic_error("SCR handler has no loaded script");
    const auto instruction = DecodeScrInstructionAt(*script, runtime.ProgramCounter());
    return Execute(runtime, instruction);
}

std::int32_t ScrNopInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState&, IEnvironmentRootMemory&)
{
    // 4CE150只有xor eax,eax/retn：不查PC/正文/长度，也不分配包装。
    return 0;
}

ScrInvalidOpcodeHandler::ScrInvalidOpcodeHandler(
    IScrVmCommandErrorSink& errorSink, IScriptExecutionRootSource* root,
    IRuntimeStringManagerSource& strings) noexcept
    : errorSink_(errorSink), root_(root), strings_(strings)
{
}

std::int32_t ScrInvalidOpcodeHandler::Execute(
    ScrScriptRuntimeState& runtime, const ScrInstructionView&)
{
    // 原入口自行取当前脚本；即使经现代工具接口调用，也不能信任外部instruction快照。
    return ExecuteRuntimeEntry(runtime, DefaultEnvironmentRootMemory());
}

std::int32_t ScrInvalidOpcodeHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory)
{
    class BoundRoot final : public IScriptExecutionRootSource
    {
    public:
        explicit BoundRoot(ScrScriptRuntimeState& runtime) : runtime_(runtime) {}
        ScriptExecutionRootReceiver CaptureAfterEnsureAt413BE0() override
        {
            return {&runtime_, [](void* p) -> std::uint32_t& {
                return static_cast<ScrScriptRuntimeState*>(p)->ProgramCounterSlotAt14();
            }};
        }
    private:
        ScrScriptRuntimeState& runtime_;
    } bound(runtime);
    return ExecuteScriptInvalidAt4CDC00(runtime.NativeParserAtA0(), root_ ? *root_ : bound,
        errorSink_, memory, strings_);
}

std::int32_t ScrNopInstructionHandler::Execute(
    ScrScriptRuntimeState&,
    const ScrInstructionView&)
{
    return 0;
}

ScrVmCore::ScrVmCore(HandlerTable handlers, IEnvironmentRootMemory& memory)
    : handlers_(std::move(handlers)), memory_(memory)
{
    for (const IScrVmInstructionHandler* const handler : handlers_)
    {
        if (handler == nullptr)
            throw std::invalid_argument("SCR VM handler table contains an empty slot");
    }
}

ScrVmCore::ScrVmCore(HandlerTable handlers, IScriptExecutionRootSource& root,
    IEnvironmentRootMemory& memory) : ScrVmCore(std::move(handlers), memory)
{
    root_ = &root;
}

ScrVmCore::ScrVmCore(HandlerTable handlers, ScriptNativeBody& body, ScriptNativePublication& publication,
    IScriptExecutionRootSource* root, IEnvironmentRootMemory& memory)
    : ScrVmCore(std::move(handlers), memory)
{
    root_ = root;
    nativeBody_ = &body;
    nativePublication_ = &publication;
}

void ScrVmCore::RequireNativeRuntime(const ScrScriptRuntimeState& runtime) const
{
    if (nativeBody_ && (nativeBody_ != &runtime.NativeBody() || nativePublication_ != &runtime.Publication()))
        throw std::invalid_argument("CScript runtime and native entry receiver differ");
}
ScrVmNativeEntryContextRuntime ScrVmCore::BorrowNativeEntryContext(ScrScriptRuntimeState& runtime) const
{RequireNativeRuntime(runtime);return ScrVmNativeEntryContextRuntime(*this,runtime);}
std::int32_t ScrVmNativeEntryContextRuntime::Invoke(ScriptNativeBody& receiver,unsigned opcode)
{
    if(&receiver!=core_.nativeBody_||&receiver!=&runtime_.NativeBody())
        throw std::invalid_argument("CScript native entry context has a different receiver");
    return core_.handlers_[opcode]->ExecuteRuntimeEntry(runtime_,core_.memory_);
}
std::int32_t ScrVmCore::ExecuteUntilYield(ScrScriptRuntimeState& runtime) const
{
    RequireNativeRuntime(runtime);
    class BoundSlotSource final : public IScriptExecutionRootSource
    {
    public:
        explicit BoundSlotSource(ScrVmParamsGenerationSlot& slot) : slot_(slot) {}
        ScriptExecutionRootReceiver CaptureAfterEnsureAt413BE0() override
        {
            return {&slot_, [](void* p) -> std::uint32_t& {
                return static_cast<ScrVmParamsGenerationSlot*>(p)->Current().programCounter;
            }};
        }
    private:
        ScrVmParamsGenerationSlot& slot_;
    } bound(*runtime.generationSlot_);

    class HandlerDispatch final : public IScriptNativeCommandDispatch
    {
    public:
        HandlerDispatch(const HandlerTable& table, ScrScriptRuntimeState& state,
            IEnvironmentRootMemory& memory, ScriptNativeBody* body)
            : table_(table), state_(state), memory_(memory), body_(body) {}
        std::int32_t DispatchAt4D4200(std::uint32_t index) override
        {
            if (body_) return body_->entriesAt2C[index](body_);
            // 显式工具/夹具模式仍可注入测试handler；生产路径不回退到此分支。
            return table_[index]->ExecuteRuntimeEntry(state_, memory_);
        }
    private:
        const HandlerTable& table_;
        ScrScriptRuntimeState& state_;
        IEnvironmentRootMemory& memory_;
        ScriptNativeBody* body_;
    } dispatch(handlers_, runtime, memory_, nativeBody_);
    auto context=BorrowNativeEntryContext(runtime);ScriptNativeEntryScope scope(context);
    return ExecuteScriptLoopAt4D3FF0(runtime.NativeParserAtA0(), root_ ? *root_ : bound, dispatch, memory_);
}
}
