#include "SettingDialogNativeConfirm.Internal.h"

namespace KaGuYa::Start::Runtime
{
int PollSettingDialogModeNativeAt51E4B0()
{
    using namespace SettingDialogNativeConfirmDetail;int accepted=1;
    if(Read(Field<std::uint8_t>(Registry(1,0x51E4DC),0x7D)))
    {
        Fresh f;CreateSettingDialogGeneralConfirmNativeReference(f.slot,1);RefLife life{f.slot};
        auto* unused=ModalReceiver(0x51E576);accepted=ModalAt5071D0(unused,f.slot)==6;
        auto* control=Read(f.slot.controlAt08);life.NormalCaptured(control);
    }
    if(accepted)
    {
        auto& s=RequireSettingDialogNativeConfirmServices().source;
        auto* environment=s.CaptureEnvironmentAt4180C0(0x51E5E7);
        const auto window=Read(environment->windowAt2C);
        static_cast<void>(s.PostMessageA(reinterpret_cast<HWND>(window),0x111,0x8015,0));
        // PostMessage返回FALSE仍重新取archive2并提交DWORD+04=0，不缓存前一个根。
        Field<unsigned>(Registry(2,0x51E603,true),4)=0;
    }
    return accepted;
}
}
