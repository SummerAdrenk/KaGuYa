#pragma once
#include <Runtime/SaveLoadDialogNativeWorkflow.h>
#include <Runtime/ModalDialogRunnerRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct GeneralConfirmDialogNativeServices;
class ISaveLoadDialogNativeReturnTitleSource
{
public:
    virtual ~ISaveLoadDialogNativeReturnTitleSource()=default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0()=0;
    virtual GuiDialogNativeBody5Ch* ConstructGeneralConfirmationAt4E3330(void* raw84,std::uint32_t mode)=0;
    virtual void* CaptureInformationAfter413BE0()=0;
    virtual IModalDialogRunnerServices& ModalServices()=0;
    virtual BOOL PostMessageA(HWND window,UINT message,WPARAM w,LPARAM l)=0;
    // 正文外服务元数据；生产调用自动覆盖构造、modal及最后一次Ref释放。
    virtual GeneralConfirmDialogNativeServices* NativeGeneralConfirmationServices() const noexcept{return nullptr;}
};
std::int32_t ReturnToTitleFromSaveLoadNativeAt51E320(ISaveLoadDialogNativeReturnTitleSource&);
class ISaveLoadDialogNativeDeleteConfirmationSource
{
public:
    virtual ~ISaveLoadDialogNativeDeleteConfirmationSource()=default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0()=0;
    virtual void* CaptureInformationAfter413BE0()=0;
    virtual IModalDialogRunnerServices& ModalServices()=0;
};
// 显式叶链接保留给隔离测试；生产重载直接绑定506CE0，不伪造默认全Save表已完成。
using SaveLoadDialogNativeDeleteExecution=void (*)(SaveLoadDialogNativeBody184h&,void* externalServices);
std::int32_t ConfirmSaveLoadDeleteNativeAt51B8F0(SaveLoadDialogNativeBody184h&,
    ISaveLoadDialogNativeDeleteConfirmationSource&,SaveLoadDialogNativeDeleteExecution,void*);
class ISaveLoadDialogNativeDeletionSource;
std::int32_t ConfirmSaveLoadDeleteNativeAt51B8F0(SaveLoadDialogNativeBody184h&,
    ISaveLoadDialogNativeDeleteConfirmationSource&,ISaveLoadDialogNativeDeletionSource&);
}
