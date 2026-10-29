#include <Runtime/ScriptMenuCommandNative.h>
#include <Runtime/ScrVmMenuHandler.h>
#include <Runtime/ScriptSoundGroupSource.h>
#include "ScriptFileCommandSupport.h"

#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace FileCommandDetail;
struct MenuUnwind
{
    YtlReferenceSlotRuntime* slot;
    ~MenuUnwind() noexcept { if (slot) UnwindScriptControlCommand(*slot); }
    void Finish(YtlReferenceControlRuntime* captured) noexcept
    {
        slot = nullptr;
        // 4CF4BF/4CF814固定EDI；4C96B0异常展开才在Destroy后重读Ref.control。
        ReleaseCapturedScriptStackControl(captured);
    }
};
const ParamsGameSystem& GameSystem(IScriptSoundGroupSource& source)
{
    auto* root = source.CaptureAt418480();
    if (!root || !root->GameSystem()) throw std::logic_error("MENU null GameSystem (modern safety boundary)");
    return *root->GameSystem();
}
const ScriptControlCommandNative* InlineBody(const Command& command) noexcept
{
    // 内联取值的bias是已知0；只有第二组的4CA1F0调用重读三字Ref。
    return reinterpret_cast<const ScriptControlCommandNative*>(command.reference.baseAt00);
}
template<class T> T ReadAt(const Command& command, const ScriptControlCommandNative* body, std::uint32_t field)
{
    if (!body || !body->source) throw std::logic_error("MENU null command/source (modern safety boundary)");
    const auto address = reinterpret_cast<std::uintptr_t>(body->source);
    const auto offset = static_cast<std::uint32_t>(address - command.base);
    if (command.knownExtent && std::uint64_t{offset} + field + sizeof(T) > command.extent)
        throw std::out_of_range("MENU operand outside allocation (modern safety boundary)");
    T result;
    std::memcpy(&result, reinterpret_cast<const void*>(address + field), sizeof(result));
    return result;
}
std::int8_t Mode(const Command& command) { return ReadAt<std::int8_t>(command, InlineBody(command), 4); }
std::uint8_t Count(const Command& command) { return ReadAt<std::uint8_t>(command, InlineBody(command), 5); }
std::int32_t RelativeOperand(const Command& command, std::uint32_t base)
{
    const auto* body = InlineBody(command);
    const auto count = ReadAt<std::uint8_t>(command, body, 5);
    return ReadAt<std::int32_t>(command, body, base + 4u * count);
}
std::int32_t Tail(const Command& command, IScriptSoundGroupSource& groups, std::uint32_t base)
{
    const auto* body = InlineBody(command);
    const auto count = ReadAt<std::uint8_t>(command, body, 5);
    const auto& first = GameSystem(groups);
    const auto& second = GameSystem(groups);
    return ReadAt<std::int32_t>(command, body, base + 4u * (count + second.soundSlotCounts[1] + first.soundSlotCounts[3]));
}
ScrVmParamsGeneration& Params(IScriptFileCommandSource& pc)
{
    const auto root = pc.CaptureAfterEnsureAt413BE0();
    return root.paramsAt47BDB0(root.object);
}
std::int32_t NamedValue(const ScriptMenuCommandNativeContext& c, const char* name)
{
    const auto* slot = c.pc.CaptureSlot0At4180C0();
    const auto root = c.pc.CaptureAfterEnsureAt413BE0();
    const auto index = c.pc.ResolveAt498020(slot, name);
    return ReadScriptValueAt4C73A0(root.paramsAt47BDB0(root.object), index, c.errors);
}
void MarkChoice(const ScriptMenuCommandNativeContext& c, std::int32_t choice)
{
    const auto root = c.menu.CaptureMessageRootAfterEnsureAt413BE0();
    const auto message = root.messageAt00_10(root.object);
    // 4CF79F先排除负ID；空message此时不会被解引用，不能提前拒绝。
    if (choice < 0) return;
    if (!message.choicesAt1C) throw std::logic_error("MENU null message flags (modern safety boundary)");
    const auto& ref = message.choicesAt1C->RawSlot();
    const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(ref.controlAt08);
    const auto count = control ? control->countAt10 : 0;
    if (choice >= std::bit_cast<std::int32_t>(count)) return;
    const auto base = ref.baseAt00;
    if (!base) throw std::logic_error("MENU null flag array (modern safety boundary)");
    const auto old = *reinterpret_cast<const std::uint8_t*>(base +
        (ref.biasAt04 + static_cast<std::uint32_t>(choice)) * control->strideAt0C);
    *reinterpret_cast<std::uint8_t*>(base +
        (ref.biasAt04 + static_cast<std::uint32_t>(choice)) * control->strideAt0C) = 1;
    if (!old) message.persistAt43D3D0(message.object);
}
}

std::int32_t ExecuteScriptMenuAt4CEDA0(ScrScriptRuntimeState& runtime,
    ScrVmScriptCommandState& state, ScrMusicInstructionHandler& music,
    IScrVmMenuLayerBoundary& menu, const ScriptMenuCommandNativeContext& c,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    constexpr char prefix[] = "CmdMenu\x8F\x88\x97\x9D\x82\xAA\x8E\xB8\x94\x73"
        "\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49 : ";
    static_assert(sizeof(prefix) == 31);
    std::int32_t next = 4;
    const auto error = ExecuteWithReport(prefix, c.errors, strings, [&] {
        auto command = CreateCommand(runtime.NativeParserAtA0(), c.pc, memory, ScriptControlCommandKind::Menu);
        auto* const capturedControl = command.reference.controlAt08;
        MenuUnwind unwind{&command.reference};
        if (state.ReadFlagsAt04() & 2u)
        {
            const auto format = RelativeOperand(command, 6);
            Params(c.pc).messageFormatIndexAt60 = format;
            menu.ApplyResultLayerList(RelativeOperand(command, 10));
            music.ApplyNativeResourceId(RelativeOperand(command, 14));
            for (std::uint32_t i = 0; std::bit_cast<std::int32_t>(i) <
                std::bit_cast<std::int32_t>(GameSystem(c.groups).soundSlotCounts[3]); ++i)
                ApplyScriptFirstSoundAt4D6640(RelativeOperand(command, 18u + 4u * i), i, c.names, c.soundLeaves, c.soundGate);
            for (std::uint32_t i = 0; std::bit_cast<std::int32_t>(i) <
                std::bit_cast<std::int32_t>(GameSystem(c.groups).soundSlotCounts[1]); ++i)
            {
                const auto& first = GameSystem(c.groups);
                const auto* body = ResolveEnvironmentObjectReference<ScriptControlCommandNative>(command.reference);
                const auto firstCount = first.soundSlotCounts[3];
                const auto count = ReadAt<std::uint8_t>(command, body, 5);
                const auto id = ReadAt<std::int32_t>(command, body, 18u + 4u * (count + firstCount + i));
                ApplyScriptSecondSoundAt4D62A0(id, i, c.names, c.groups, c.soundLeaves, c.soundGate);
            }
        }
        menu.SelectEffect(Tail(command, c.groups, 18));
        if (!(state.ReadFlagsAt04() & 1u) && Mode(command) != 3 && Mode(command) != 5)
        {
            const auto save = Tail(command, c.groups, 22);
            Params(c.pc).SaveParams2At30().instructionIndexAtSaveParams08 = save;
        }
        Params(c.pc).menuCandidatesAt64.clear();
        const auto count = Count(command);
        for (std::uint32_t i = 0; i < count; ++i)
        {
            const auto* body = InlineBody(command);
            const auto currentCount = ReadAt<std::uint8_t>(command, body, 5);
            const auto value = i < currentCount ? ReadAt<std::int32_t>(command, body, 6u + 4u * i) : -1;
            Params(c.pc).menuCandidatesAt64.Push(static_cast<std::uint32_t>(value), memory);
        }
        const bool restoring = state.ReadStateByteAt26() != 0;
        state.WriteStateByteAt27(1);
        std::int32_t selection = -1;
        if (!restoring)
        {
            const auto mode = Mode(command);
            if (mode == 0 || mode == 3)
            {
                state.WriteStateByteAt24(1); state.WriteStateByteAt27(0);
                unwind.Finish(capturedControl); return;
            }
            if (mode == 1 || mode == 2)
            {
                auto* slot = c.menu.CaptureSlot7At4180C0();
                if (!slot) throw std::logic_error("MENU null slot7 (modern safety boundary)");
                selection = slot->wordsAt0C[mode == 1 ? 2u : 3u];
            }
            else if (mode == 4) selection = NamedValue(c, "prog_result");
        }
        state.WriteStateByteAt28(0);
        if (Mode(command) == 5) state.WriteStateByteAt28(1);
        if (selection >= 0 && selection < std::bit_cast<std::int32_t>(Params(c.pc).menuCandidatesAt64.Size()))
        {
            const auto* slot = c.pc.CaptureSlot0At4180C0();
            const auto root = c.pc.CaptureAfterEnsureAt413BE0();
            const auto index = c.pc.ResolveAt498020(slot, "menu_result");
            WriteScriptValueAt4C8220(root.paramsAt47BDB0(root.object), index, selection, c.errors);
            const auto& candidates = Params(c.pc).menuCandidatesAt64;
            if (static_cast<std::uint32_t>(selection) >= candidates.Size())
                throw std::out_of_range("MENU selected candidate changed out of range (modern safety boundary)");
            MarkChoice(c, std::bit_cast<std::int32_t>(candidates.storage.begin[selection]));
            next = 0;
            unwind.Finish(capturedControl); return;
        }
        state.WriteStateByteAt24((state.ReadFlagsAt04() & 2u) ? 1u : NamedValue(c, "replay") != 0 ? 1u : 0u);
        const auto mode = Mode(command);
        if (mode == 0 || mode == 3) state.WriteStateByteAt27(0);
        unwind.Finish(capturedControl);
    });
    return error ? error : next;
}
}
