#include <Runtime/ScriptAudioCommandsNative.h>
#include <Runtime/VoiceBatchNative.h>
#include <Runtime/ScriptVoiceSource.h>
#include "ScriptFileCommandSupport.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace FileCommandDetail;
struct CommandCleanup
{
    YtlReferenceSlotRuntime* slot;
    ~CommandCleanup() noexcept { if (slot) UnwindScriptControlCommand(*slot); }
    void Finish(YtlReferenceControlRuntime* captured) noexcept
    {
        slot = nullptr;
        YtlReferenceSlotRuntime normal{0, 0, captured};
        ReleaseScriptDispatchCommandAt4D40C6(normal);
    }
};
}
std::int32_t ExecuteScriptMusicAt4CF9E0(ScrScriptRuntimeState& runtime,
    IScriptFileCommandSource& pc, ScrMusicInstructionHandler& music, IEnvironmentRootMemory& memory)
{
    auto command = CreateCommand(runtime.NativeParserAtA0(), pc, memory, ScriptControlCommandKind::Music);
    auto* const control = command.reference.controlAt08;
    CommandCleanup cleanup{&command.reference};
    // 4CFAEE在state1内读DWORD；MUSIC没有SOUND/VOICE的内层catch及错误框。
    music.ApplyNativeResourceId(Operand<std::int32_t>(command, 4));
    cleanup.Finish(control);
    return 0;
}
std::int32_t ExecuteScriptSoundAt4D1520(ScrScriptRuntimeState& runtime,
    IScriptFileCommandSource& pc, ScrSoundInstructionHandler& sound, IScrVmCommandErrorSink& errors,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    constexpr char prefix[] = "CmdSound\x8F\x88\x97\x9D\x82\xAA\x8E\xB8\x94\x73"
        "\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49 : ";
    static_assert(sizeof(prefix) == 32);
    return ExecuteWithReport(prefix, errors, strings, [&] {
        auto command = CreateCommand(runtime.NativeParserAtA0(), pc, memory, ScriptControlCommandKind::Sound);
        auto* const control = command.reference.controlAt08;
        CommandCleanup cleanup{&command.reference};
        const auto port = Operand<std::int8_t>(command, 4);
        const auto resource = Operand<std::int32_t>(command, 5);
        sound.ApplyNativeOperands(port, resource);
        cleanup.Finish(control);
    });
}
std::int32_t ExecuteScriptVoiceAt4D2FF0(ScrScriptRuntimeState& runtime,
    IScriptFileCommandSource& pc, const IScrVmVoiceNameSource& names, IScriptVoiceSource& source,
    IScrVmCommandErrorSink& errors, IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    constexpr char prefix[] = "CmdVoice\x8F\x88\x97\x9D\x82\xAA\x8E\xB8\x94\x73"
        "\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49 : ";
    static_assert(sizeof(prefix) == 32);
    return ExecuteWithReport(prefix, errors, strings, [&] {
        auto command = CreateCommand(runtime.NativeParserAtA0(), pc, memory, ScriptControlCommandKind::Voice);
        auto* const control = command.reference.controlAt08;
        CommandCleanup cleanup{&command.reference};
        const auto count = Operand<std::uint8_t>(command, 4);
        if (!count)
        {
            const auto voice = source.CaptureVoiceManagerAfterEnsureAt413BE0();
            if (!voice) throw std::logic_error("VOICE null manager (modern safety boundary)");
            voice.StopAndClearAllVoiceSlots();
        }
        else
        {
            // 4D3178的state4先于412870；失败时重读Ref，正常state7固定已捕获control。
            RuntimeCStringArrayReference list;
            list.RebuildAt412870(count, strings);
            for (std::uint32_t i = 0; i < count; ++i)
            {
                const auto id = Operand<std::int32_t>(command, 5 + 4 * i);
                const char* name = names.FindVoiceNameAt4D3286(id);
                SetRuntimeCStringAt437680(list.At(i), name, name ? static_cast<int>(std::strlen(name)) : 0);
            }
            // 4D333F在整批名字解析后才捕获manager；不是handler构造时固定的对象。
            const auto voice = source.CaptureVoiceManagerAfterEnsureAt413BE0();
            if (!voice) throw std::logic_error("VOICE null manager (modern safety boundary)");
            (void)voice.PlayBatchAt42C4F0(list);
            // 此释放必须在command之前，且异常不能进入外层错误报告。
            ReleaseScriptDispatchCommandAt4D40C6(list.RawSlot());
        }
        cleanup.Finish(control);
    });
}
}
