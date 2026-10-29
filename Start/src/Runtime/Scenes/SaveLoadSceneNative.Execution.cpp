#include "SaveLoadSceneNative.Internal.h"
#include <Runtime/EnvironmentArchiveRegistration.h>

namespace KaGuYa::Start::Runtime
{
using namespace SaveLoadSceneNativeDetail;
namespace
{
GuiDialogNativeBody5Ch* Dialog(Body& body) noexcept
{
    const auto base = Read(body.dialogAt08.baseAt00);
    if (!base) return nullptr;
    auto* control = static_cast<EnvironmentObjectReferenceControl*>(Read(body.dialogAt08.controlAt08));
    const auto stride = control ? Read(control->strideAt0C) : 0u;
    return reinterpret_cast<GuiDialogNativeBody5Ch*>(base + Read(body.dialogAt08.biasAt04) * stride);
}
void RestoreRegistry(Body& body, ISaveLoadSceneNativeSource& source, std::uint32_t site)
{
    auto* archive = ResolveEnvironmentArchiveAt417990(*source.CaptureEnvironmentAt4180C0(site), 2);
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(archive) + 4u) = Read(body.savedRegistryAt14);
}
constexpr char loadAfter[] = "\x83\x8D\x81\x5B\x83\x68\x91\x49\x91\xF0\x8C\xE3\x8F\x88\x97\x9D";
}
void UpdateSaveLoadSceneNativeAt4BA650(Body& body, SaveLoadSceneNativeServices& services)
{
    SaveLoadSceneNativeScope scope(services);
    auto& source = services.source;
    switch (Read(body.phaseAt04))
    {
    case 0:
    {
        source.WriteRootSubobjectAt4AD230DwordAt10(0);
        auto* dialog = Dialog(body);
        Read(dialog->vptrAt00)->beginAt44(dialog);
        body.phaseAt04 = 1;
        [[fallthrough]];
    }
    case 1:
    {
        auto* dialog = Dialog(body);
        // Poll消费完整EAX，不截成AL/bool；每次虚调用都重解当前Ref和vptr。
        if (Read(dialog->vptrAt00)->pollAt48(dialog) != 0) return;
        dialog = Dialog(body);
        Read(dialog->vptrAt00)->endAt4C(dialog);
        dialog = Dialog(body);
        const auto result = Read(dialog->vptrAt00)->resultAt50(dialog);
        body.phaseAt04 = result == 0 ? 2u : 3u;
        return;
    }
    case 2:
        source.PopThenActivatePendingAt4C1760();
        RestoreRegistry(body, source, 0x4BA755);
        source.WriteRootByteAt219(body.savedRootByteAt21, 0x4BA76E);
        return;
    case 3:
        RestoreRegistry(body, source, 0x4BA785);
        source.WriteRootByteAt219(body.savedRootByteAt21, 0x4BA79E);
        source.ClearScenesAt4C1460();
        source.QueueAt47F920(loadAfter);
        return;
    default:
        return;
    }
}
}
