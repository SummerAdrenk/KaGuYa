#pragma once
#include <Runtime/SaveLoadDialogNativePersistence.h>
#include <Runtime/SaveLoadTimestampNative.h>
#include <Runtime/ModalDialogRunnerRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 535FF0/538430等已经还原的独立内核通过宿主声明同源服务；不替代整个确认算法。
class ISaveLoadDialogNativeConfirmationSource : public ISaveLoadDialogNativePersistenceSource,
    public ISaveLoadTimestampNativeSource
{
public:
    virtual std::int64_t CurrentTimeAt56A321() = 0;
    // 消费四个已拥有CString；调用者在进入前撤清理资格，异常也不得再次释放参数。
    virtual void ConfigureAt535FF0(GuiDialogNativeBody5Ch*, RuntimeCString& page,
        RuntimeCString& slot, RuntimeCString& time, RuntimeCString& scene, std::uint8_t saveMode) = 0;
    virtual void AssignPreviewAt538430(GuiDialogNativeBody5Ch*, YtlReferenceSlotRuntime& owned) = 0;
    virtual YtlReferenceSlotRuntime* CapturePreviewAt47D740(void* root, YtlReferenceSlotRuntime* fresh) = 0;
    virtual GraphicsSurfaceNativeBody* ConstructPreviewSurfaceAt40C2E0(void* raw24) = 0;
    virtual std::uint32_t LoadPreviewAt511D90(GraphicsSurfaceNativeBody*, const char* path, const void* option) = 0;
    virtual void* CaptureInformationAfter413BE0() = 0;
    virtual IModalDialogRunnerServices& ModalServices() = 0;
};
std::int32_t ProcessSaveLoadSaveConfirmationNativeAt525480(SaveLoadDialogNativeBody184h&,
    ISaveLoadDialogNativeConfirmationSource&);
std::int32_t ProcessSaveLoadLoadConfirmationNativeAt520AB0(SaveLoadDialogNativeBody184h&,
    ISaveLoadDialogNativeConfirmationSource&,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
}
