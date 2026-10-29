#include <Runtime/LegacyTitleWorkflow.h>
#include <Logging/Logger.h>

#include <bit>
#include <mbstring.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 49DFD0 的 CP932 场景/脚本常量；与现有 SceneFactory/TitleSceneNative 消费者一致。
constexpr char settings[] = "\x90\xDD\x92\xE8\x89\xE6\x96\xCA";
constexpr char load[] = "\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA";
constexpr char start[] = "\x83\x58\x83\x5E\x81\x5B\x83\x67.scr";
constexpr char script[] = "\x83\x58\x83\x4E\x83\x8A\x83\x76\x83\x67\x8F\x88\x97\x9D";
constexpr char cg[] = "\x82\x62\x82\x66\x95\x5C\x8E\xA6\x83\x82\x81\x5B\x83\x68";
constexpr char scene[] = "\x83\x56\x81\x5B\x83\x93\x8D\xC4\x90\xB6\x83\x82\x81\x5B\x83\x68";
constexpr char music[] = "\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x83\x82\x81\x5B\x83\x68";
constexpr char afterLoad[] = "\x83\x8D\x81\x5B\x83\x68\x91\x49\x91\xF0\x8C\xE3\x8F\x88\x97\x9D";
// 52FA14：原抛 const char*，不是取消，也不降级到 Win32 确认。
constexpr char missingMessageBox[] =
    "\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x7B\x83\x62\x83\x4E\x83\x58"
    "\x82\xCC\x95\x5C\x8E\xA6\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49";

void Advance(TransitOperation& op, std::int32_t amount) noexcept
{
    // 4C08BD 的 DWORD 回绕及 4C08D2 的 signed 比较；不用有符号溢出。
    op.current = std::bit_cast<std::int32_t>(std::uint32_t(op.current) + std::uint32_t(amount));
    if (op.current >= op.target) op.current = op.target;
}

struct TemporaryReference final
{
    YtlReferenceSlotRuntime& slot;
    bool armed = true;
    ~TemporaryReference() noexcept { if (armed) ReleaseYtlReference(slot); }
    void ReleaseNormally()
    {
        // 49E429/4C999C：先撤 EH 资格再正常释放；释放本身抛出不能重复减持。
        armed = false;
        ReleaseYtlReference(slot);
    }
};

void Tick(LegacyTitleWorkflowState& state, LegacyTitleWorkflowServices& services)
{
    auto& op = state.operation;
    // 4C7FA0 / 49E0E6..49E168：先 skip，再 done；++phase 后仍 dispatch/step/commit。
    if (services.host.Held(5) || services.host.MousePressedEdge(0)) Advance(op, 256);
    if (op.opcode == -1 || op.current >= op.target) ++state.phase;
    (void)services.host.CaptureDraw().RenderTransitionAt4C39E0(op);
    Advance(op, state.step);
    services.screen.InvalidatedRectIfLoaded(nullptr);
}

void PollMenu(LegacyTitleWorkflowState& state, LegacyTitleWorkflowServices& services)
{
    switch (services.menu.PollSelection())
    {
    case 0:
    {
        std::int32_t result = 1;
        auto& names = services.names;
        if ((names.EditableNameBit(0) || names.EditableNameBit(1)) && !names.SkipNamePrompt())
            result = names.RunModal();
        if (result == 1) state.phase = 4;
        break;
    }
    case 1: state.phase = 16; break;
    case 2: state.phase = 7; break;
    case 3: state.phase = 10; break;
    case 4: state.phase = 13; break;
    case 5: (void)services.host.PostWindowMessage(0x10, 0, 0); break;
    case 7:
        services.host.WriteSpecialMenuFlag(0);
        (void)services.host.PostWindowMessage(0x111, 0x1801C, 0);
        break;
    default: break;
    }
    // 49E3CE：普通 poll 的取消/无选择/退出/切页都 flush；不在异常展开时强行 flush。
    services.menu.FlushDirty();
}
}

void UpdateLegacyTitleWorkflow(LegacyTitleWorkflowState& state,
    LegacyTitleWorkflowResources resources, LegacyTitleWorkflowServices& services)
{
    GraphicsSurfaceNativeScope surfaceCalls(services.surfaces);
    auto& host = services.host;
    auto& op = state.operation;
    for (;;)
    {
        switch (state.phase)
        {
        case 0:
        {
            // 49E017..49E0DF：reset 与 source/step 先于音频，声音普通 false 不拦截。
            op = TransitOperation{};
            op.opcode = 57;
            op.source = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(resources.frame);
            state.step = 20;
            const auto volume = services.audio.ReadBgmVolume();
            const auto* name = services.audio.TitleBgmName();
            (void)services.audio.PlayBgm(name, volume, 1);
            (void)services.audio.ClearSound();
            services.audio.StopCommonSounds();
            state.phase = 1;
            [[fallthrough]];
        }
        case 1: case 5: case 8: case 11: case 14:
            Tick(state, services);
            return;
        case 2:
            host.CaptureCommon().ModeAt04() = 4;
            state.step = 0;
            state.phase = 3;
            [[fallthrough]];
        case 3:
        {
            bool settingsRequested;
            {
                // 49E19B..49E1CC：Clone -> 比较 -> 释放；释放完成才清命令/push。
                const auto command = host.CaptureScenes().ClonePendingCommandAt414540();
                settingsRequested = _mbscmp(
                    reinterpret_cast<const unsigned char*>(command.Value().GetString()),
                    reinterpret_cast<const unsigned char*>(settings)) == 0;
            }
            if (settingsRequested)
            {
                host.CaptureScenes().WritePendingCommandAt1C({});
                host.CaptureScenes().PushActiveScene(settings);
                return;
            }
            // 44E0C0/44E190：按键先于 load-enabled，再读 replay；两条早分支不 flush。
            if (host.Pressed(19) && host.CaptureCommon().BytesAt08()[1] != 0)
            {
                state.phase = 16;
                continue;
            }
            if (host.Pressed(21) && host.CaptureCommon().BytesAt08()[1] != 0 && host.ReplayValue() == 0)
            {
                state.phase = 17;
                continue;
            }
            PollMenu(state, services);
            if (state.phase == 3) return;
            continue;
        }
        case 4:
        {
            YtlReferenceSlotRuntime captured;
            host.CaptureFrame(captured);
            TemporaryReference temporary{captured};
            // 49E413..49E433 / 41F990：先释放旧 frame 再复制，正常尾释放前撤资格。
            CopyAssignGraphicsSurfaceReferenceAt465870(resources.frame, captured);
            temporary.ReleaseNormally();
            op = TransitOperation{};
            op.opcode = 0;
            op.source = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(resources.black);
            auto* frame = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(resources.frame);
            if (!frame)
            {
                // 49E466..49E46D 原空解引用危险域：保留已发生的赋值/reset，明确拒绝。
                Logging::Write(Logging::Level::Error, "LegacyTitle: empty captured frame at Baku 49E46D");
                throw std::runtime_error("LegacyTitle requires a captured frame (Baku 49E46D)");
            }
            op.previous = frame->handleAt04;
            state.step = 20;
            host.CaptureCommon().ModeAt04() = 0;
            state.phase = 5;
            continue;
        }
        case 6:
            (void)host.LoadStartupScript(start, -1);
            host.CaptureScenes().QueueScene(script);
            host.WriteStartupFlag(1);
            host.WriteStartupReadyFlag(1);
            return;
        case 7: case 10: case 13:
            // 49E4F9：三鉴赏出口共享 opcode58，不沿用新游戏的 previous 截屏。
            op = TransitOperation{};
            op.opcode = 58;
            op.source = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(resources.black);
            state.step = 20;
            host.CaptureCommon().ModeAt04() = 0;
            ++state.phase;
            continue;
        case 9: host.CaptureScenes().QueueScene(cg); return;
        case 12: host.CaptureScenes().QueueScene(scene); return;
        case 15: host.CaptureScenes().QueueScene(music); return;
        case 16:
            host.WriteLoadEntryFlag(0);
            host.CaptureScenes().PushActiveScene(load);
            // 49E5CC -> 49E5D4：factory/append 抛出时仍是16，不能提前写2。
            state.phase = 2;
            return;
        case 17:
        {
            state.phase = 18;
            const auto prepared = services.quickLoad.Prepare();
            if (prepared == 0)
            {
                state.phase = 3;
                continue; // 49E618 与 phase18 的取消不同：本次重新分派菜单。
            }
            if (prepared == 2)
            {
                auto* target = services.quickLoad.MessageBackgroundSlot();
                if (!target) throw static_cast<const char*>(missingMessageBox);
                YtlReferenceSlotRuntime argument;
                CopyGraphicsSurfaceReferenceAt47D740(argument, resources.frame);
                TemporaryReference temporary{argument};
                // 49E65D/4C9960：增持按值参数，赋消息框 Ref，消费参数；不冻结成 handle。
                CopyAssignGraphicsSurfaceReferenceAt465870(*target, argument);
                temporary.ReleaseNormally();
            }
            return;
        }
        case 18:
            switch (services.quickLoad.Poll())
            {
            case 1:
                if (services.quickLoad.Load())
                {
                    host.CaptureScenes().ClearScenes();
                    host.CaptureScenes().QueueScene(afterLoad);
                    host.CaptureCommon().ModeAt04() = 0;
                    return;
                }
                [[fallthrough]];
            case 2: state.phase = 3; return;
            default: return;
            }
        default:
            return; // 49E003：十进制19及其它值走 default，不是新的业务 phase。
        }
    }
}
}
