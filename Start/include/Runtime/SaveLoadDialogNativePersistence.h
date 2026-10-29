#pragma once
#include <Runtime/SaveLoadDialogNative.h>
#include <Runtime/SaveFileFactoryNativeIo.h>
#include <Runtime/SaveLoadLabelNative.h>
#include <Runtime/MessageFormatSelectionRuntime.h>
#include <Runtime/ParamsRuntimeRoot.h>

namespace KaGuYa::Start::Runtime
{
class ISaveLoadDialogNativePersistenceSource : public ISaveFileFactoryNativeIoSource,
    public ISaveLoadLabelNativeSource
{
public:
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override = 0;
    virtual IRuntimeStringManagerSource& Strings() override = 0;
    virtual ParamsRuntimeRoot* CaptureParamsAt418480() = 0;
    virtual YtlReferenceSlotRuntime& RootFileManagerSlotAt00(void*) = 0;
    virtual YtlReferenceSlotRuntime& FileManagerMessageSlotAt10(void*) = 0;
    virtual std::int32_t ReadFormatAt417C40(void* message, std::int32_t index,
        MessageFormatSelectionRawTarget output, std::uint32_t variant, bool markRead) = 0;
    virtual void ReloadCatalogItemAt48EE00(void* root, std::int32_t page, std::int32_t slot) = 0;
    virtual int MessageBoxA(HWND, const char* text, const char* caption, UINT) = 0;
};
bool SaveLoadDefaultNameMatchesNativeAt50FEA0(void* saveData,
    ISaveLoadDialogNativePersistenceSource&);
std::int32_t ExecuteSaveLoadSaveNativeAt532A90(SaveLoadDialogNativeBody184h&,
    ISaveLoadDialogNativePersistenceSource&);
// sourceIndex按原分支先读一次，普通页参数在分配返回后另读；不得提前值快照。
void CreateSaveLoadFileFactoryNativeForSelection(YtlReferenceSlotRuntime& destination,
    const std::int32_t& sourceIndex, ISaveFileFactoryNativeIoSource&,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void CreateSaveLoadFileFactoryNativeForSelectionBranch(YtlReferenceSlotRuntime& destination,
    std::int32_t selectedBranch, const std::int32_t& currentSourceIndex, ISaveFileFactoryNativeIoSource&,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
std::int32_t ExecuteSaveLoadLoadNativeAt511FB0(SaveLoadDialogNativeBody184h&,
    ISaveLoadDialogNativePersistenceSource&,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
}
