#include "SettingDialogNativeConfirm.Internal.h"

namespace KaGuYa::Start::Runtime
{
int PollSettingDialogDefaultsNativeAt51B730(SettingDialogNativeBodyACh& b)
{
    using namespace SettingDialogNativeConfirmDetail;bool accepted=true;
    if(Read(Field<std::uint8_t>(Registry(1,0x51B75A),0x7E)))
    {
        Fresh f;CreateSettingDialogGeneralConfirmNativeReference(f.slot,2);RefLife life{f.slot};
        auto* unused=ModalReceiver(0x51B7F5);accepted=ModalAt5071D0(unused,f.slot)==6;
        life.NormalCaptured(Read(f.slot.controlAt08));
    }
    if(accepted){ResetSettingDialogDefaultsNativeAt538BB0(b);AddBackgroundBounds(b);}
    return 0; // 取消同样返回0；不能沿用51E4B0的accepted作为Poll返回。
}
}
