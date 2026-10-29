#include "SettingDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime::SettingDialogNativeDetail
{
namespace
{
[[noreturn]] void ReservedPhaseLoop()
{
    // 530430跳表的1/2回到自身，不能变成默认成功/失败返回。
    __asm {
        again:
        jmp again
    }
    __assume(0);
}
}
void AddBackgroundBounds(Body& b)
{
    const auto base = Read(b.backgroundAt68.baseAt00);
    auto* surface = ResolveWithBase<GraphicsSurfaceNativeBody>(b.backgroundAt68, base);
    const auto width = Read(surface->widthAt08), height = Read(surface->heightAt0C);
    surface = ResolveWithBase<GraphicsSurfaceNativeBody>(b.backgroundAt68, base);
    const auto x = Read(surface->originXAt10), y = Read(surface->originYAt14);
    auto* dirty = Read(b.common.vptrAt00)->dirtyAt00(&b.common);
    Read(dirty->vptrAt00)->addXYWHAt10(dirty, x, y, width, height);
}
void __fastcall Begin(Common* p, void*)
{
    auto& b = Dialog(p);
    auto& s = RequireSettingDialogNativeServices();
    auto* root = Root(s.source);
    auto* draw = Resolve<void>(s.source.RootDrawSlotAt18(root));
    FreshReference captured;
    auto* returned = s.source.CaptureBackgroundAt4ACDD0(draw, &captured.slot);
    LocalReference lifetime{captured.slot}; // 成功Capture之后state0，业务赋值异常可展开。
    if (&b.backgroundAt68 != returned)
    {
        ReleaseYtlReference(b.backgroundAt68);
        CopyGuiClickedEventReference(b.backgroundAt68, *returned);
    }
    lifetime.Reload(false); // 原state1正常尾重读control，之后再启用。
    Read(p->vptrAt00)->writeEnabledAt24(p, 1);
    for (auto* position = Read(b.dialogsAt5C.begin); position != Read(b.dialogsAt5C.end); ++position)
    {
        auto* child = Resolve<Common>(*position);
        Read(child->vptrAt00)->beginAt44(child);
    }
    b.phaseAt74 = 0;
    root = Root(s.source);
    SelectSettingDialogNativePageAt5352F0(b, Read(s.source.RootPageAt214(root)));
    SnapshotSettingDialogNativeAt4FF430(b);
    root = Root(s.source);
    s.source.StopSoundAt4384D0(Resolve<void>(s.source.RootSoundSlotAt24(root)));
    auto* registry = CaptureRegistry(s.source, 2);
    root = Root(s.source);
    s.source.StopSoundMappingsAt413270(Resolve<void>(s.source.RootSoundSlotAt24(root)),
        &Field<EnvironmentVectorStorage<RuntimeCString>>(registry, 0x30));
    root = Root(s.source);
    s.source.PrepareSoundAt433F60(Resolve<void>(s.source.RootSoundSlotAt24(root)));
    b.fieldAtA4 = 0; // 50F609：所有准备正常返回才写。
}
std::int32_t __fastcall Poll(Common* p, void*)
{
    auto& b = Dialog(p);
    auto& s = RequireSettingDialogNativeServices();
    switch (Read(b.phaseAt74))
    {
    case 0: AddBackgroundBounds(b); b.phaseAt74 = 3; break;
    case 1:
    case 2: ReservedPhaseLoop();
    case 3:
        Read(p->vptrAt00)->processInputAt54(p);
        Read(p->vptrAt00)->flushDirtyAt58(p);
        return 1;
    case 4:
        AddBackgroundBounds(b);
        Read(p->vptrAt00)->writeEnabledAt24(p, 0);
        b.phaseAt74 = -1;
        Read(p->vptrAt00)->flushDirtyAt58(p);
        return 1;
    case 5:
    {
        auto* system = s.source.SystemAt4180C0();
        const auto window = Read(system->windowAt2C);
        (void)s.source.PostMessageA(reinterpret_cast<HWND>(window), WM_CLOSE, 0, 0);
        b.phaseAt74 = 3;
        break;
    }
    case 6:
        // 尚缺的原业务必须由具名native接口提供；这里不能填固定0/1冒充完成。
        if (!s.remaining.PollModeConfirmationAt51E4B0()) b.phaseAt74 = 3;
        break;
    case 7:
        if (!s.remaining.PollDefaultsConfirmationAt51B730(b)) b.phaseAt74 = 3;
        break;
    case 8:
        if (!s.remaining.PollNameConfirmationAt521AD0(b)) b.phaseAt74 = 3;
        break;
    default: return 0;
    }
    Read(p->vptrAt00)->flushDirtyAt58(p);
    return 1;
}
std::int32_t __fastcall End(Common* p, void*)
{
    auto& b = Dialog(p);
    for (auto* position = Read(b.dialogsAt5C.begin); position != Read(b.dialogsAt5C.end); ++position)
    {
        auto* child = Resolve<Common>(*position);
        (void)Read(child->vptrAt00)->endAt4C(child);
    }
    RequireSettingDialogNativeServices().remaining.ApplySettingsAt51CFC0(b);
    return 1; // 只有真实Apply返回后，509535才返回1；不在缺口处伪造成功。
}
}
