#include <Runtime/LegacyConfirmationInternal.h>
#include <Runtime/SceneManagerNative.h>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
using namespace LegacyConfirmationDetail;
namespace
{
ISystemDialogNativeSource& System(LegacyConfirmationServices& s)
{
    if (!s.system) Reject("LegacyConfirmationSystem: missing real system/root source");
    return *s.system;
}
void* Root(ISystemDialogNativeSource& s) { s.EnsureRootAt413BE0(); return s.RootAt6198C4(); }
}
LegacyConfirmationSystem::LegacyConfirmationSystem(const LegacyConfirmationData& data,
    LegacyConfirmationServices& services) : LegacyConfirmation(services), data_(data)
{
    Scope scope(services_);
    Slot factory{}; CreateFactoryToken(factory); RefLife life{factory};
    // 4DEFA0：1静态+4按钮，没有导航；独立interaction=1/suppress=0。
    AppendStatic(data_.systemBackground);
    for (const auto& d : data_.systemButtons) AppendButton(d);
    AdoptGuiInputAt536EA0(state_.common.inputAt18, NewGameGuiInputAt4EAAED());
    life.Finish();
}
void LegacyConfirmationSystem::Begin()
{
    Scope scope(services_);
    CaptureBackground();
    auto& p = state_.common;
    p.vptrAt00->writeEnabledAt24(&p, 1);
    ClearGuiClickedEventFlagsAt532200(events_);
    phase_ = 0; result_ = -1; resultWritten_ = true;
    p.vptrAt00->resetChildrenAt68(&p);
    p.activeAt4C = nullptr; // 4DF1E0不reset input latch。
    auto& s = System(services_);
    auto* root = Root(s);
    auto* mode = ResolveEnvironmentObjectReference<WindowModeNative2Ch>(s.RootWindowModeSlotAt0C(root));
    if (!mode) Reject("Baku 4DF303: absent window mode");
    const bool windowed = static_cast<std::uint8_t>(mode->modeAt04) != 0; // 4B9CCA读低byte。
    for (int id = 1; id <= 3; ++id)
    {
        Slot child; p.vptrAt00->findChildAt14(&p, &child, id); RefLife life{child};
        auto& c = RequireGuiControl(child);
        // 4DF4C8先固定child，再Ensure/读根1F9；对应统一业务ReadByteAt219。
        auto selected = c.vptrAt00->writeStateAt44;
        const bool enabled = id == 3 ? s.RootMenuStateAt219(Root(s)) != 0 : (id == 1 ? windowed : !windowed);
        selected(&c, enabled);
        life.Finish();
    }
    begun_ = true;
}
bool LegacyConfirmationSystem::CanOpen()
{
    Scope scope(services_);
    auto& p = state_.common;
    if (p.vptrAt00->readStateAt20(&p) && !suppressOnce_)
    {
        auto& s = System(services_);
        auto* env = s.SystemAt4180C0();
        if (!s.IsIconic(s.SystemWindowAt2C(env)))
        {
            Slot child; p.vptrAt00->findChildAt14(&p, &child, 0); RefLife life{child};
            if (child.baseAt00)
            {
                POINT local; const auto* pointer = p.vptrAt00->pointerAt74(&p, &local);
                const auto x = pointer->x, y = pointer->y;
                RECT logical; s.LogicalRectangleAt478E50(Root(s), &logical);
                if (x >= logical.left && x < logical.right)
                {
                    auto& c = RequireGuiControl(child); RECT scratch, bounds;
                    ::CopyRect(&bounds, c.vptrAt00->rectAt3C(&c, &scratch));
                    // 4DFA97：含bottom，无现代x>=menu.left条件，也不测logical.bottom。
                    if (y >= logical.top && y <= bounds.bottom) { life.Finish(); return true; }
                }
            }
            life.Finish();
        }
    }
    suppressOnce_ = false; // 所有普通失败尾清一次抑制，异常不清。
    return false;
}
void LegacyConfirmationSystem::ProcessInput()
{
    if (!CanOpen()) { result_ = 0; phase_ = 4; return; }
    LegacyConfirmation::ProcessInput();
    for (auto* at = events_.begin; at != events_.end; ++at)
    {
        if (!RequireGuiClickedEvent(*at).clickedFlagAt04) continue;
        result_ = RequireGuiClickedEvent(*at).eventValueAt08;
        phase_ = 4;
        return;
    }
}
bool LegacyConfirmationSystem::Poll()
{
    Scope scope(services_);
    if (!begun_) Reject("Baku 4DF6B0 requires Begin");
    auto& p = state_.common;
    switch (phase_)
    {
    case 0: AddBackgroundBounds(); phase_ = 3; break;
    case 3: p.vptrAt00->processInputAt54(&p); break;
    case 4: AddBackgroundBounds(); p.vptrAt00->writeEnabledAt24(&p, 0); phase_ = -1; break;
    case 1: case 2: Reject("Baku 4DF6B0 unreachable spin phase");
    default: return false;
    }
    p.vptrAt00->flushDirtyAt58(&p); return true;
}
std::int32_t LegacyConfirmationSystem::Result() { return -1; } // 原4C9660，不是成功占位。
bool LegacyConfirmationSystem::End()
{
    auto& s = System(services_);
    switch (result_)
    {
    case 1: case 2: case 4:
    {
        auto* env = s.SystemAt4180C0();
        static_cast<void>(s.PostMessageA(s.SystemWindowAt2C(env), result_ == 4 ? WM_CLOSE : WM_COMMAND,
            result_ == 1 ? 0x800C : (result_ == 2 ? 0x800B : 0), 0));
        break;
    }
    case 3:
    {
        auto* selected = ResolveEnvironmentObjectReference<SceneManagerNativeBody48h>(s.RootSceneManagerSlotAt30(Root(s)));
        if (!selected) Reject("Baku 419950: absent scene manager for settings command");
        SetRuntimeCStringAt437680(selected->commandAt1C, data_.settingsCommand, int(std::strlen(data_.settingsCommand)));
        suppressOnce_ = true; // 4DF59B字符串成功后才写1。
        break;
    }
    default: break;
    }
    return true; // 4DF520所有分支都返回1，owner清modal；不释放截图。
}
}
