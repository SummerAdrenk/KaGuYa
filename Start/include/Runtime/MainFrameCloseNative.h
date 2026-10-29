#pragma once
#include <Runtime/MainFrameNativeDispatch.h>
#include <Runtime/GeneralConfirmDialogNative.h>

namespace KaGuYa::Start::Runtime
{
class IMainFrameCloseNativeDialogs
{
public:
    virtual ~IMainFrameCloseNativeDialogs()=default;
    virtual GeneralConfirmDialogNativeServices* NativeCalls()noexcept=0;
    virtual void CreateReferenceAt42380D(YtlReferenceSlotRuntime& fresh)=0;
    virtual int RunAt5071D0(void* capturedInformation,const YtlReferenceSlotRuntime*)=0;
};
class GeneralConfirmMainFrameCloseNativeDialogs final:public IMainFrameCloseNativeDialogs
{
public:
    explicit GeneralConfirmMainFrameCloseNativeDialogs(GeneralConfirmDialogNativeServices& calls)noexcept:calls_(calls){}
    GeneralConfirmDialogNativeServices* NativeCalls()noexcept override{return &calls_;}
    void CreateReferenceAt42380D(YtlReferenceSlotRuntime&)override;
    int RunAt5071D0(void*,const YtlReferenceSlotRuntime*)override;
private:
    GeneralConfirmDialogNativeServices& calls_;
};
// 423750的try只覆盖模态执行；构造/control分配失败外传，尾Ref释放是原terminate资格。
LRESULT CloseMainFrameNativeAt423750(MainFrameNativeBody90h&,UINT,WPARAM,LPARAM,
    IMainFrameNativeRootSource&,IMainFrameNativeWindowApi&,IMainFrameCloseNativeDialogs&);
}
