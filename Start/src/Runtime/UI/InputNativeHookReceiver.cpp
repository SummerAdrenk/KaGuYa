#include "InputNativeObjects.Internal.h"
#include <Runtime/WinHookNative.h>

namespace KaGuYa::Start::Runtime
{
void InputNativeHookReceiver::AddRaw(void* raw,InputNativeServices& inputServices) const
{
    if(controlled_){controlled_->Add(raw,DispatchInputNativeHook,&inputServices);return;}
    // 原四槽协议只传raw正文；callback/context仅属于显式受控dispatcher接口。
    WinHookNativeScope scope(*services_);
    body_->vptrAt00->addAt00(body_,raw);
}
void InputNativeHookReceiver::RemoveRaw(void* raw) const
{
    if(controlled_){controlled_->Remove(raw);return;}
    WinHookNativeScope scope(*services_);
    body_->vptrAt00->removeAt04(body_,raw);
}
InputNativeHookReceiver InputNativeWinHookSource::CaptureAt450E20()
{return InputNativeHookReceiver(owner_.CaptureAt450E20(),services_);}
}
