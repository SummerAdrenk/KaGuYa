#include "LogSceneNative.Internal.h"
#include <Runtime/EnvironmentArchiveRegistration.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace LogSceneNativeDetail;
void RestoreRegistry(Body& b,LogSceneNativeServices& s,unsigned site)
{
    auto* e=s.source.CaptureEnvironmentAt4180C0(site);auto* value=ResolveEnvironmentArchiveAt417990(*e,2);
    *static_cast<volatile std::uint32_t*>(&RegistryValue(value))=Read(b.savedRegistryAt08);
}
void* Root(LogSceneNativeServices& s,unsigned site)
{s.source.EnsureRootAt413BE0(site);return s.source.RootAt6198C4(site);}
}
void UpdateLogSceneNativeAt4B6900(LogSceneNativeBody20h& b,LogSceneNativeServices& s)
{
    LogSceneNativeScope scope(s);
    switch(Read(b.phaseAt04))
    {
    case 0:
    {
        RestoreRegistry(b,s,0x4B69A1);auto* dialog=Resolve<Common>(b.dialogAt10);
        Read(dialog->vptrAt00)->beginAt44(dialog);
        auto* e=s.source.CaptureEnvironmentAt4180C0(0x4B69DD);
        *static_cast<volatile std::uint32_t*>(&RegistryValue(ResolveEnvironmentArchiveAt417990(*e,2)))=0;
        b.phaseAt04=1;[[fallthrough]];
    }
    case 1:
    {
        auto* dialog=Resolve<Common>(b.dialogAt10);
        if(Read(dialog->vptrAt00)->pollAt48(dialog)==0)
        {dialog=Resolve<Common>(b.dialogAt10);Read(dialog->vptrAt00)->endAt4C(dialog);b.phaseAt04=2;}
        return;
    }
    case 2:
    {
        auto* root=Root(s,0x4B6926);s.source.PopThenActivatePendingAt4C1760(Resolve<void>(s.source.RootSceneManagerSlotAt30(root)));
        RestoreRegistry(b,s,0x4B6959);
        root=Root(s,0x4B6972);s.source.WriteRootByteAt218(root,Read(b.savedRootByteAt1C));
        root=Root(s,0x4B698A);s.source.WriteRootByteAt219(root,Read(b.savedRootByteAt1D));return;
    }
    default:return;
    }
}
}
