#include "SaveLoadDialogNative.Confirmation.Internal.h"
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/RuntimeCStringConcatenation.h>
#include <Runtime/SystemInformationDisplayNative.h>
#include <Runtime/SaveFileCatalog.h>
#include <Runtime/SaveLoadResourceProfile.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
using namespace SaveLoadConfirmationDetail;
std::int32_t ProcessSaveLoadLoadConfirmationNativeAt520AB0(SaveLoadDialogNativeBody184h& dialog,
    ISaveLoadDialogNativeConfirmationSource& source, IEnvironmentRootMemory& memory)
{
    const auto* save = static_cast<const std::byte*>(InlineSaveData(source));
    if (!std::to_integer<std::uint8_t>(save[0x78])) return ExecuteSaveLoadLoadNativeAt511FB0(dialog, source, memory);
    Slot page;
    ReferenceUnwind pageUnwind{page};
    Slot factory;
    ReferenceUnwind factoryUnwind{factory};
    const auto selected = dialog.sourceIndexAt160;
    auto* catalog = Catalog(source);
    if (selected == -2) CopyPageReferenceAt4FA0D0(page, catalog->autoPageAt38);
    else if (selected == -1) CopyPageReferenceAt4FA0D0(page, catalog->quickPageAt2C);
    else
    {
        const auto address = reinterpret_cast<std::uintptr_t>(catalog->regularPagesAt20.begin) + std::uint32_t(selected) * 12u;
        CopyPageReferenceAt4FA0D0(page, *reinterpret_cast<const Slot*>(address));
    }
    // 520AB0在Catalog捕获前固定分支；普通构造的page+1却在new后读取当前字段。
    CreateSaveLoadFileFactoryNativeForSelectionBranch(factory, selected, dialog.sourceIndexAt160, source, memory);
    auto* pageControl = page.controlAt08;
    bool accepted = false;
    auto* confirmation = ResolveConfirmation(dialog.slotConfirmationAt174);
    if (confirmation)
    {
        WriteConfirmationMode(confirmation, 0);
        Labels labels(source);
        const auto sourceIndex = dialog.sourceIndexAt160;
        if (sourceIndex == -2) SetRuntimeCStringAt437680(labels.page.value, " Auto", 5);
        else if (sourceIndex == -1) SetRuntimeCStringAt437680(labels.page.value, "Quick", 5);
        else FormatRuntimeCStringAt417130(labels.page.value, "%04d", SaveFilePhysicalPageNumber(*Catalog(source), sourceIndex));
        FormatRuntimeCStringAt417130(labels.slot.value, "%02d", Increment(dialog.slotIndexAt164));
        auto* item = Item(page, pageControl, dialog.slotIndexAt164);
        auto* time = ResolveEnvironmentObjectReference<SaveFileNativeTime>(item->timeAt00);
        const auto timestamp = std::bit_cast<std::int64_t>((std::uint64_t(time->highAt04) << 32u) | time->lowAt00);
        FormatSaveLoadTimestampNativeAt525480And520AB0(labels.time.value, timestamp, source);
        item = Item(page, pageControl, dialog.slotIndexAt164);
        TextSlot shortened;
        const auto& style=CurrentSaveLoadResourceProfile().confirmation.text[3];
        TruncateSaveLoadLabelNativeAt4DA8E0(&shortened.value, item->labelAt0C.GetString(), style.maxCharacters, style.maxLines, source);
        TextUnwind shortenedUnwind{shortened.value};
        TextSlot copied; CloneText(copied.value, shortened.value); TextUnwind copiedUnwind{copied.value};
        const auto* chars = copied.value.GetString();
        AssignRuntimeCStringAt410220(labels.scene.value, copied.value);
        copiedUnwind.FinishCached(chars); shortenedUnwind.Finish();
        labels.Configure(confirmation, 0, source);
        auto* raw = memory.AllocateBody(0x24);
        GraphicsSurfaceNativeBody* surface = nullptr;
        if (raw)
        {
            try { surface = source.ConstructPreviewSurfaceAt40C2E0(raw); }
            catch (...) { [&]() noexcept { memory.DeleteBody(raw, 0x24); }(); throw; }
        }
        Slot preview;
        ConstructGraphicsSurfaceReferenceAt451500(preview, surface, 1);
        ReferenceUnwind previewUnwind{preview};
        YtlReferenceControlRuntime* previewControl = nullptr;
        bool loaded;
        {
            TextSlot filename;
            ConstructSaveFileFactoryPreviewNameNativeAt47AFA0(filename.value,
                *ResolveEnvironmentObjectReference<SaveFileDeleteFactoryNativeBody0Ch>(factory), Increment(dialog.slotIndexAt164), source);
            TextUnwind filenameUnwind{filename.value};
            const auto* suffix = filename.value.GetString();
            auto* env = source.CaptureEnvironmentAt4180C0();
            auto* paths = ResolveEnvironmentObjectReference<std::byte>(env->pathsAt48);
            auto& directory = *reinterpret_cast<RuntimeCString*>(reinterpret_cast<std::uintptr_t>(paths) + 0x10u);
            {
                RuntimeCStringConcatenation path(directory, suffix, source.Strings());
                const auto* pathChars = path.GetString();
                // 5211E9在4C7210返回/字符串指针取出后才缓存；不越过名称/目录/拼接回调。
                previewControl = preview.controlAt08;
                loaded = std::uint8_t(source.LoadPreviewAt511D90(
                    ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(preview), pathChars, nullptr)) != 0;
            }
            filenameUnwind.Finish();
        }
        if (!loaded)
        {
            // 52128C..5212A1取vector+0C，即读取用途的第二个Ref；首项只用于保存确认。
            Slot fallback;
            CopyGraphicsSurfaceReferenceAt47D740(fallback, dialog.noPreviewAt144.begin[1]);
            ReferenceUnwind fallbackUnwind{fallback};
            CopyAssignGraphicsSurfaceReferenceAt465870(preview, fallback);
            fallbackUnwind.FinishClearing(); previewControl = preview.controlAt08;
        }
        Slot argument = preview;
        argument.controlAt08 = previewControl;
        if (previewControl) ++previewControl->referenceCountAt04;
        source.AssignPreviewAt538430(confirmation, argument);
        (void)source.CaptureInformationAfter413BE0();
        accepted = RunNativeGuiModalReferenceAt5071D0(&dialog.slotConfirmationAt174, source.ModalServices()) == 6;
        previewUnwind.FinishCached(previewControl);
        labels.Finish();
    }
    auto* factoryControl = factory.controlAt08;
    factoryUnwind.FinishCached(factoryControl); pageUnwind.FinishCached(pageControl);
    return accepted ? ExecuteSaveLoadLoadNativeAt511FB0(dialog, source, memory) : 0;
}
}
