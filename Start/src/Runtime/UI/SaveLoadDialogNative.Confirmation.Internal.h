#pragma once
#include "SaveLoadDialogNative.Persistence.Internal.h"
#include <Runtime/SaveLoadDialogNativeConfirmation.h>
#include <Runtime/SaveFileStateNativeArchive.h>
#include <Runtime/EnvironmentArchiveRegistration.h>

namespace KaGuYa::Start::Runtime::SaveLoadConfirmationDetail
{
using namespace SaveLoadPersistenceDetail;
SaveFileStateNativeArchive* Catalog(ISaveLoadDialogNativeConfirmationSource&);
void* InlineSaveData(ISaveLoadDialogNativeConfirmationSource&);
SaveFileNativeItem* Item(const Slot& page, YtlReferenceControlRuntime* cached, std::int32_t index);
void CopyPageReferenceAt4FA0D0(Slot&, const Slot&);
GuiDialogNativeBody5Ch* ResolveConfirmation(const Slot&) noexcept;
void WriteConfirmationMode(GuiDialogNativeBody5Ch*, std::int32_t);
void CloneText(RuntimeCString& fresh, const RuntimeCString&);
void ResolveCurrentDescription(RuntimeCString& target, ISaveLoadDialogNativeConfirmationSource&);
// 四槽原顺序GetNil；正常尾用调用原535FF0前逐项缓存的header，EH重读当前槽。
class Labels final
{
public:
    explicit Labels(ISaveLoadDialogNativeConfirmationSource&);
    ~Labels() noexcept;
    Labels(const Labels&) = delete;
    Labels& operator=(const Labels&) = delete;
    void Configure(GuiDialogNativeBody5Ch*, std::uint8_t, ISaveLoadDialogNativeConfirmationSource&);
    void Finish();
    TextSlot page, slot, time, scene;
private:
    void DestroyCurrent() noexcept;
    unsigned completed_ = 0;
    const char* cached_[4];
};
}
