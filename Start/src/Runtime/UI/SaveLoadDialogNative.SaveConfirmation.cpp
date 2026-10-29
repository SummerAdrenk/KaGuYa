#include "SaveLoadDialogNative.Confirmation.Internal.h"
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/SystemInformationDisplayNative.h>
#include <Runtime/SaveFileCatalog.h>
#include <Runtime/SaveLoadResourceProfile.h>

namespace KaGuYa::Start::Runtime
{
using namespace SaveLoadConfirmationDetail;
std::int32_t ProcessSaveLoadSaveConfirmationNativeAt525480(SaveLoadDialogNativeBody184h& dialog,
    ISaveLoadDialogNativeConfirmationSource& source)
{
    Slot selected;
    reinterpret_cast<const SaveLoadDialogNativeVtable37*>(dialog.common.vptrAt00)
        ->findDialogAt90(&dialog.common, &selected, 2);
    ReferenceUnwind selectedUnwind{selected};
    std::int32_t pageIndex = -1;
    bool occupied = false;
    if (selected.baseAt00)
    {
        pageIndex = PageIndex(ResolveEnvironmentObjectReference<GuiDialogNativeBody5Ch>(selected));
        auto* catalog = Catalog(source);
        const auto address = reinterpret_cast<std::uintptr_t>(catalog->regularPagesAt20.begin) + std::uint32_t(pageIndex) * 12u;
        const auto& page = *reinterpret_cast<const Slot*>(address);
        occupied = Item(page, page.controlAt08, dialog.slotIndexAt164)->timeAt00.baseAt00 != 0;
    }
    const auto* save = static_cast<const std::byte*>(InlineSaveData(source));
    auto required = std::to_integer<std::uint8_t>(save[0x77]);
    if (occupied)
    {
        save = reinterpret_cast<const std::byte*>(ResolveEnvironmentArchiveAt417990(*source.CaptureEnvironmentAt4180C0(), 1));
        required = std::to_integer<std::uint8_t>(save[0x7B]);
    }
    bool accepted = !required;
    if (required)
    {
        auto* confirmation = ResolveConfirmation(dialog.slotConfirmationAt174);
        if (confirmation)
        {
            WriteConfirmationMode(confirmation, 1 + occupied);
            Labels labels(source);
            FormatRuntimeCStringAt417130(labels.page.value, "%04d", SaveFilePhysicalPageNumber(*Catalog(source), pageIndex));
            FormatRuntimeCStringAt417130(labels.slot.value, "%02d", Increment(dialog.slotIndexAt164));
            const auto currentTime = source.CurrentTimeAt56A321();
            FormatSaveLoadTimestampNativeAt525480And520AB0(labels.time.value, currentTime, source);
            TextSlot current; InitializeEmptyRuntimeCString(current.value, source.Strings());
            TextUnwind currentUnwind{current.value};
            ResolveCurrentDescription(current.value, source);
            const auto* cached = current.value.GetString();
            TextSlot shortened;
            const auto& style=CurrentSaveLoadResourceProfile().confirmation.text[3];
            TruncateSaveLoadLabelNativeAt4DA8E0(&shortened.value, cached, style.maxCharacters, style.maxLines, source);
            TextUnwind shortenedUnwind{shortened.value};
            TextSlot copied; CloneText(copied.value, shortened.value); TextUnwind copiedUnwind{copied.value};
            const auto* copiedChars = copied.value.GetString();
            AssignRuntimeCStringAt410220(labels.scene.value, copied.value);
            copiedUnwind.FinishCached(copiedChars); shortenedUnwind.Finish(); currentUnwind.FinishCached(cached);
            labels.Configure(confirmation, 1, source);
            const auto* screenshotSave = reinterpret_cast<const std::byte*>(ResolveEnvironmentArchiveAt417990(*source.CaptureEnvironmentAt4180C0(), 1));
            Slot preview;
            if (std::to_integer<std::uint8_t>(screenshotSave[0x74]))
            {
                source.EnsureRootAt413BE0();
                source.CapturePreviewAt47D740(source.RootAt6198C4(), &preview);
            }
            else CopyGraphicsSurfaceReferenceAt47D740(preview, *dialog.noPreviewAt144.begin);
            // 538430直接消费已拥有Ref；进入前即撤caller资格，不能为其异常再补释放。
            source.AssignPreviewAt538430(confirmation, preview);
            (void)source.CaptureInformationAfter413BE0();
            accepted = RunNativeGuiModalReferenceAt5071D0(&dialog.slotConfirmationAt174, source.ModalServices()) == 6;
            labels.Finish();
        }
    }
    if (accepted) (void)ExecuteSaveLoadSaveNativeAt532A90(dialog, source);
    selectedUnwind.FinishUncleared();
    return 0;
}
}
