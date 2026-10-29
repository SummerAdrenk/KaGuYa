#include "SettingSceneNative.Internal.h"
#include <Runtime/EnvironmentArchiveRegistration.h>

namespace KaGuYa::Start::Runtime
{
using namespace SettingSceneNativeDetail;
namespace
{
void* CaptureRegistry(SettingSceneNativeServices& services)
{ return ResolveEnvironmentArchiveAt417990(*services.source.CaptureEnvironmentAt4180C0(), 2); }
void WriteSavedRegistry(Body& body, SettingSceneNativeServices& services)
{
    auto& word = RegistryWord(CaptureRegistry(services), 4);
    *static_cast<volatile std::uint32_t*>(&word) = Read(body.savedRegistryAt08);
}
}
void UpdateSettingSceneNativeAt4BA800(Body& body, SettingSceneNativeServices& services)
{
    SettingSceneNativeScope scope(services);
    switch (Read(body.phaseAt04))
    {
    case 0:
    {
        WriteSavedRegistry(body, services);
        auto* child = Resolve<GuiDialogNativeBody5Ch>(body.dialogAt14);
        Read(child->vptrAt00)->beginAt44(child);
        auto& word = RegistryWord(CaptureRegistry(services), 4);
        *static_cast<volatile std::uint32_t*>(&word) = 0;
        body.phaseAt04 = 1;
        // 4BA8D9：同帧落入 Poll；Begin/后续捕获抛出均不提前置 phase1。
        [[fallthrough]];
    }
    case 1:
    {
        auto* child = Resolve<GuiDialogNativeBody5Ch>(body.dialogAt14);
        if (Read(child->vptrAt00)->pollAt48(child) == 0)
        {
            // Poll 可修改槽；重新解析，不缓存原 child，也不询问 Result+50。
            child = Resolve<GuiDialogNativeBody5Ch>(body.dialogAt14);
            (void)Read(child->vptrAt00)->endAt4C(child);
            body.phaseAt04 = 2;
        }
        return;
    }
    case 2:
    {
        services.source.EnsureRootAt413BE0();
        auto* root = services.source.RootAt6198C4();
        auto& slot = services.source.RootSceneManagerSlotAt30(root);
        services.source.PopThenActivatePendingAt4C1760(Resolve<void>(slot));
        // 先推进，之后两次独立 4180C0/417990；无 catch/回滚/完成哨兵。
        WriteSavedRegistry(body, services);
        auto& flags = RegistryWord(CaptureRegistry(services), 0x14);
        *static_cast<volatile std::uint32_t*>(&flags) = Read(body.savedFlagsAt0C);
        return;
    }
    default:
        return;
    }
}
}
