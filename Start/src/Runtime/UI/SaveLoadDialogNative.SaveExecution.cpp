#include "SaveLoadDialogNative.Persistence.Internal.h"
#include <Runtime/ScriptParamsNativeBody.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/SaveFileCatalog.h>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
using namespace SaveLoadPersistenceDetail;
bool SaveLoadDefaultNameMatchesNativeAt50FEA0(void* save,
    ISaveLoadDialogNativePersistenceSource& source)
{
    auto* params = source.CaptureParamsAt418480();
    const auto* given = params->gameSystemAt04.get()->defaultGivenName.Native().GetString();
    if (!given) throw ATL::CAtlException(E_FAIL);
    const auto* current = reinterpret_cast<const RuntimeCString*>(reinterpret_cast<std::uintptr_t>(save) + 0x58);
    if (::_mbscmp(reinterpret_cast<const unsigned char*>(current->GetString()),
        reinterpret_cast<const unsigned char*>(given))) return false;
    // 第一个名称相同才重新捕获Params；不能把两字段从同一个GameSystem快照读取。
    params = source.CaptureParamsAt418480();
    const auto* family = params->gameSystemAt04.get()->defaultFamilyName.Native().GetString();
    if (!family) throw ATL::CAtlException(E_FAIL);
    current = reinterpret_cast<const RuntimeCString*>(reinterpret_cast<std::uintptr_t>(save) + 0x5C);
    return !::_mbscmp(reinterpret_cast<const unsigned char*>(current->GetString()),
        reinterpret_cast<const unsigned char*>(family));
}
std::int32_t ExecuteSaveLoadSaveNativeAt532A90(SaveLoadDialogNativeBody184h& dialog,
    ISaveLoadDialogNativePersistenceSource& source)
{
    TextSlot label;
    InitializeEmptyRuntimeCString(label.value, source.Strings());
    TextUnwind labelUnwind{label.value};
    const auto* initialChars = label.value.GetString();
    source.EnsureRootAt413BE0();
    auto* params = static_cast<ScriptParamsNativeBody*>(source.ParamsAt47BDB0(source.RootAt6198C4()));
    const auto formatIndex = params->messageFormatIndexAt60;
    if (formatIndex == -1)
    {
        source.EnsureRootAt413BE0();
        params = static_cast<ScriptParamsNativeBody*>(source.ParamsAt47BDB0(source.RootAt6198C4()));
        const auto* header = reinterpret_cast<const ATL::CStringData*>(params->SaveParamsAt08().saveTitleAt08.GetString()) - 1;
        TextSlot copied;
        const auto chars = reinterpret_cast<std::uintptr_t>(CloneRuntimeCStringDataAt414540(const_cast<ATL::CStringData*>(header))) + 16u;
        std::memcpy(&copied.value, &chars, sizeof(chars));
        TextUnwind copiedUnwind{copied.value};
        AssignRuntimeCStringAt410220(label.value, copied.value);
        copiedUnwind.FinishCached(reinterpret_cast<const char*>(chars));
    }
    else
    {
        Format format;
        auto* save = ResolveEnvironmentArchiveAt417990(*source.CaptureEnvironmentAt4180C0(), 1);
        const auto variant = std::uint32_t(!SaveLoadDefaultNameMatchesNativeAt50FEA0(save, source));
        source.EnsureRootAt413BE0();
        auto* root = source.RootAt6198C4();
        auto* files = ResolveEnvironmentObjectReference<void>(source.RootFileManagerSlotAt00(root));
        auto* message = ResolveEnvironmentObjectReference<void>(source.FileManagerMessageSlotAt10(files));
        if (source.ReadFormatAt417C40(message, formatIndex, format.Target(), variant, false) < 0)
        {
            format.Finish(); labelUnwind.FinishCached(initialChars); return 0;
        }
        if (!format.body.baseAt00) throw 1;
        TextSlot text;
        const auto* returned = ReadMessageText(ResolveEnvironmentObjectReference<void>(format.body), &text.value);
        TextUnwind textUnwind{text.value};
        TextSlot expanded;
        const auto* result = ExpandSaveLoadLabelNativeAt4DC660(&expanded.value, returned->GetString(), source);
        TextUnwind expandedUnwind{expanded.value};
        AssignChars(label.value, result->GetString());
        expandedUnwind.Finish(); textUnwind.Finish(); format.Finish();
    }
    source.EnsureRootAt413BE0();
    params = static_cast<ScriptParamsNativeBody*>(source.ParamsAt47BDB0(source.RootAt6198C4()));
    auto* destination = &params->SaveParamsAt08().saveDescriptionAt04;
    // 532D13在Set/后续回调之前缓存ESI；正常尾释放该header，EH读取当前TEXT槽。
    const auto* labelChars = label.value.GetString();
    AssignChars(*destination, labelChars);
    Slot selected;
    reinterpret_cast<const SaveLoadDialogNativeVtable37*>(dialog.common.vptrAt00)
        ->findDialogAt90(&dialog.common, &selected, 2);
    ReferenceUnwind selectedUnwind{selected};
    if (!selected.baseAt00)
    { selectedUnwind.FinishClearing(); labelUnwind.FinishCached(labelChars); return 0; }
    const auto page = PageIndex(ResolveEnvironmentObjectReference<GuiDialogNativeBody5Ch>(selected));
    SaveFileDeleteFactoryNativeBody0Ch factory;
    const auto* catalog = static_cast<SaveFileStateNativeArchive*>(
        ResolveEnvironmentArchiveAt417990(*source.CaptureEnvironmentAt4180C0(), 6));
    ConstructRegularSaveFileDeleteFactoryNativeAt4DCDB0(factory, SaveFilePhysicalPageNumber(*catalog, page), source);
    struct FactoryUnwind
    {
        SaveFileDeleteFactoryNativeBody0Ch& body;
        bool armed = true;
        ~FactoryUnwind() noexcept { if (armed) DestroySaveFileDeleteFactoryNativeAt463A90(body); }
        void Finish() { armed = false; DestroySaveFileDeleteFactoryNativeAt463A90(body); }
    } factoryUnwind{factory};
    auto* save = ResolveEnvironmentArchiveAt417990(*source.CaptureEnvironmentAt4180C0(), 1);
    const auto mode = *reinterpret_cast<const std::uint8_t*>(reinterpret_cast<std::uintptr_t>(save) + 0x74) != 0;
    if (SaveFileFactoryNativeAt488CF0(factory, Increment(dialog.slotIndexAt164), mode, source))
    {
        auto* env = source.CaptureEnvironmentAt4180C0();
        (void)source.MessageBoxA(reinterpret_cast<HWND>(env->windowAt2C), saveFailure, env->titleAt10.GetString(), MB_ICONERROR);
        DeleteSaveFileFactorySlotNativeAt476460(factory, Increment(dialog.slotIndexAt164), 1, source);
    }
    source.EnsureRootAt413BE0();
    source.ReloadCatalogItemAt48EE00(source.RootAt6198C4(), page, dialog.slotIndexAt164);
    NotifyPage(ResolveEnvironmentObjectReference<GuiDialogNativeBody5Ch>(selected));
    // 532E91页面重建先于factory释放；不可照删除核先释放factory。
    factoryUnwind.Finish(); selectedUnwind.FinishClearing(); labelUnwind.FinishCached(labelChars);
    return 1;
}
}
