#include "SettingDialogNativeConfirm.Internal.h"
#include <Runtime/SettingOtherResourceProfile.h>
#include <Runtime/SettingPage04Native.h>

namespace KaGuYa::Start::Runtime
{
int PollSettingDialogNameNativeAt521AD0(SettingDialogNativeBodyACh& b)
{
    const auto& profile = CurrentSettingOtherProfile();
    if (profile.name.entry != SettingOtherNameEntry::ParentResult)
        throw std::logic_error("Parent name confirmation is not enabled for this game");
    using namespace SettingDialogNativeConfirmDetail;Fresh dialog;
    CreateSettingDialogNameConfirmNativeReference(dialog.slot);const auto base=Read(dialog.slot.baseAt00);
    auto* cachedControl=Read(dialog.slot.controlAt08);RefLife lifetime{dialog.slot};
    const auto stride=cachedControl?Read(static_cast<EnvironmentObjectReferenceControl*>(cachedControl)->strideAt0C):0u;
    auto* fixed=base?reinterpret_cast<NameRegistrationDialogNativeBody94h*>(base+Read(dialog.slot.biasAt04)*stride):nullptr;
    fixed->radioEnabledAt80=0; // 原factory完成后、Root/Modal之前；原默认Ctor值不能替代此写。
    auto* unused=ModalReceiver(0x521B6C);
    if(ModalAt5071D0(unused,dialog.slot)==6)
    {
        Fresh page;FindDialog(&b.common,page.slot,4);RefLife pageLife{page.slot};
        if(Read(page.slot.baseAt00))
        {
            if (profile.name.refresh == SettingOtherNameRefresh::ReloadPage)
            {
                auto* current = Resolve<Common>(page.slot);
                auto* methods = reinterpret_cast<const SettingPage04NativeVtable39*>(Read(current->vptrAt00));
                methods->reloadAt94(current);
            }
            else
            {
                Fresh block;FindDialog(Resolve<Common>(page.slot),block.slot,
                    SettingOtherBlockFor(SettingOtherBlock::Name).dialogId);RefLife blockLife{block.slot};
                if(Read(block.slot.baseAt00))SettingPage04NativeDetail::BuildNameTextAt512200(Resolve<Common>(block.slot),1,0);
                blockLife.NormalReload(); // 17/18仅更新名字；11-16保留原整页reload副作用。
            }
            AddBackgroundBounds(b); // page非空才dirty，即使block未找到/512200原catch返回。
        }
        pageLife.NormalReload();
    }
    lifetime.NormalCaptured(cachedControl); // 521CEC用521B38缓存EBX，不改成Modal后的当前control。
    return 0;
}
}
