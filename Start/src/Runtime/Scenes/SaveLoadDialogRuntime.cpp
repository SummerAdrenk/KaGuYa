#include <Runtime/SaveLoadDialogRuntime.h>

#include <bit>
#include <cstddef>
#include <cstdio>
#include <ctime>
#include <mbctype.h>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t WrapAdd(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left) +
        std::bit_cast<std::uint32_t>(right));
}

std::string FormatDecimal(const char* format, const std::int32_t value)
{
    char buffer[32]{};
    const int length = std::snprintf(buffer, sizeof(buffer), format, value);
    if (length < 0 || static_cast<std::size_t>(length) >= sizeof(buffer))
        throw std::runtime_error("Save/load decimal label formatting failed");
    return std::string(buffer, static_cast<std::size_t>(length));
}

std::string FormatSaveLoadTimestamp(const std::int64_t rawTime)
{
    const __time64_t time = static_cast<__time64_t>(rawTime);

    // 0x525711..0x525850 对五个字段分别调用 localtime64_s；失败默认值也不同。
    std::tm value{};
    std::int32_t minute = -1;
    if (_localtime64_s(&value, &time) == 0)
        minute = value.tm_min;

    value = {};
    std::int32_t hour = -1;
    if (_localtime64_s(&value, &time) == 0)
        hour = value.tm_hour;

    value = {};
    std::int32_t day = 0;
    if (_localtime64_s(&value, &time) == 0)
        day = value.tm_mday;

    value = {};
    std::int32_t month = 0;
    if (_localtime64_s(&value, &time) == 0)
        month = value.tm_mon + 1;

    value = {};
    std::int32_t year = 0;
    if (_localtime64_s(&value, &time) == 0)
        year = value.tm_year + 1900;

    char buffer[64]{};
    const int length = std::snprintf(
        buffer,
        sizeof(buffer),
        "%02d/%02d/%02d\r\n   %02d:%02d",
        year % 100,
        month,
        day,
        hour,
        minute);
    if (length < 0 || static_cast<std::size_t>(length) >= sizeof(buffer))
        throw std::runtime_error("Save/load timestamp formatting failed");
    return std::string(buffer, static_cast<std::size_t>(length));
}

std::string TruncateSceneLabelAt4DA8E0(
    const std::string_view source,
    const std::int32_t maximumWidth,
    const std::int32_t maximumLines)
{
    std::string result;
    std::size_t position = 0;
    std::int32_t line = 0;
    std::int32_t width = 0;
    std::size_t previousCharacterBytes = 0;
    bool ellipsisWritten = false;

    while (position < source.size() && line < maximumLines)
    {
        const auto byte = static_cast<unsigned char>(source[position]);
        if (byte != '\r' && byte != '\n')
        {
            if (width >= maximumWidth)
            {
                if (!ellipsisWritten)
                {
                    result.resize(result.size() - previousCharacterBytes);
                    // 18 号原版 Start.exe:0x5BDC38 的原始字节为 81 63，即 CP932 的“…”词素。
                    result.append("\x81\x63", 2);
                    ellipsisWritten = true;
                }

                // 0x4DAB52：截断后按单字节跳过到本行换行符，不解释后续 lead byte。
                ++position;
                continue;
            }

            if (_ismbblead(byte) != 0)
            {
                result.push_back(static_cast<char>(byte));
                // 原字符串索引允许 position + 1 == length，此时读取结尾 NUL。
                result.push_back(
                    position + 1 < source.size() ? source[position + 1] : '\0');
                position += 2;
                width += 2;
                previousCharacterBytes = 2;
            }
            else
            {
                result.push_back(static_cast<char>(byte));
                ++position;
                ++width;
                previousCharacterBytes = 1;
            }
            continue;
        }

        result.push_back(static_cast<char>(byte));
        ++position;
        if (byte == '\n')
        {
            width = 0;
            ellipsisWritten = false;
            ++line;
        }
    }
    return result;
}
}

void SaveLoadDialogTransitionOperation::Reset() noexcept
{
    valueAt00 = 0;
    sourceHandleAt04 = 0;
    valueAt08 = 0;
    valueAt0C = 0;
    valueAt10 = 0;
    valueAt14 = 0;
    valueAt18 = 0;
    selectorAt1C = -1;
    valueAt20 = 0;
    progressAt24 = 0;
    maximumAt28 = 255;
    valueAt2C = 0;
}

SaveLoadDialogRuntime::SaveLoadDialogRuntime(
    ISaveLoadDialogRuntimeServices& services,
    const std::int32_t initialPageIndex,
    SaveLoadDialogFrameRef transitionFrameAt94,
    SaveLoadDialogFrameRef backgroundFrameAtA0,
    SaveLoadDialogFrameRef savePreviewFrameAt144,
    SaveLoadDeleteConfirmationRef deleteConfirmationAt168,
    SaveLoadSlotConfirmationRef slotConfirmationAt174,
    const RECT& dialogBoundsAt78,
    std::vector<SaveLoadDialogChildRef> children)
    : services_(&services),
      children_(std::move(children)),
      dialogBoundsAt78_(dialogBoundsAt78),
      transitionFrameAt94_(std::move(transitionFrameAt94)),
      backgroundFrameAtA0_(std::move(backgroundFrameAtA0)),
      savePreviewFrameAt144_(std::move(savePreviewFrameAt144)),
      deleteConfirmationAt168_(std::move(deleteConfirmationAt168)),
      slotConfirmationAt174_(std::move(slotConfirmationAt174))
{
    state_.pageModeAtE4 = initialPageIndex;
    for (const auto& child : children_)
    {
        if (!child)
            throw std::invalid_argument("Save/load dialog child is null");
    }
}

SaveLoadDialogRuntime::SaveLoadDialogRuntime(
    ISaveLoadDialogRuntimeServices& services,
    ISaveLoadDialogObjectGraph& objectGraph,
    const std::int32_t initialPageIndex,
    SaveLoadDialogFrameRef transitionFrameAt94,
    SaveLoadDialogFrameRef backgroundFrameAtA0,
    SaveLoadDialogFrameRef savePreviewFrameAt144,
    SaveLoadDeleteConfirmationRef deleteConfirmationAt168,
    SaveLoadSlotConfirmationRef slotConfirmationAt174,
    const RECT& dialogBoundsAt78)
    : services_(&services),
      objectGraph_(&objectGraph),
      dialogBoundsAt78_(dialogBoundsAt78),
      transitionFrameAt94_(std::move(transitionFrameAt94)),
      backgroundFrameAtA0_(std::move(backgroundFrameAtA0)),
      savePreviewFrameAt144_(std::move(savePreviewFrameAt144)),
      deleteConfirmationAt168_(std::move(deleteConfirmationAt168)),
      slotConfirmationAt174_(std::move(slotConfirmationAt174))
{
    state_.pageModeAtE4 = initialPageIndex;
}

void SaveLoadDialogRuntime::BeginAtVslot44()
{
    // 0x50E8B0：先捕获当前画面，再按固定顺序重置事件、控件和页面。
    capturedFrameAt88_ = services_->CapturePrimarySceneResourceAt4ACDD0();
    services_->ClearClickedEventsAt532200();
    services_->WriteDialogEnabledAtVslot24(1);
    services_->ResetInputAtVslot3C();
    services_->ResetChildrenAt522E90();
    InitializeCurrentPage(state_.pageModeAtE4);
    OpenCurrentChildren();

    state_.completionResultAt68 = 0;
    state_.phaseAt6C = SaveLoadDialogPhase::BeginTransition;
    services_->ClearSharedInputStateAt50EA19();
}

bool SaveLoadDialogRuntime::PollAtVslot48()
{
    // 0x52F7A0：合法状态只有 0..8；-1 和其余值均直接返回 0。
    switch (state_.phaseAt6C)
    {
    case SaveLoadDialogPhase::BeginTransition:
        state_.phaseAt6C = SaveLoadDialogPhase::EnterInteractive;
        state_.transitionSubstateAt70 = 0;
        [[fallthrough]];

    case SaveLoadDialogPhase::EnterInteractive:
    case SaveLoadDialogPhase::ReturnInteractive:
        AdvanceTransitionAt51CA80();
        return true;

    case SaveLoadDialogPhase::ProcessChildren:
        ProcessChildrenAt522350();
        services_->SubmitDirtyRectsAtVslot58();
        return true;

    case SaveLoadDialogPhase::Complete:
        state_.phaseAt6C = SaveLoadDialogPhase::Finished;
        return false;

    case SaveLoadDialogPhase::RequestReturnToTitle:
        if (!RequestReturnToTitleAt51E320())
            state_.phaseAt6C = SaveLoadDialogPhase::ProcessChildren;
        services_->SubmitDirtyRectsAtVslot58();
        return true;

    case SaveLoadDialogPhase::ProcessDeleteCommand:
        ProcessDeleteCommandAt51B8F0();
        state_.phaseAt6C = SaveLoadDialogPhase::ProcessChildren;
        services_->SubmitDirtyRectsAtVslot58();
        return true;

    case SaveLoadDialogPhase::ProcessSaveCommand:
        ProcessSaveCommandAt525480();
        state_.phaseAt6C = SaveLoadDialogPhase::ProcessChildren;
        services_->SubmitDirtyRectsAtVslot58();
        return true;

    case SaveLoadDialogPhase::ProcessLoadCommand:
        if (ProcessLoadCommandAt520AB0())
        {
            // 0x52F846..0x52F85F：成功后把 +0xA0 的 bg_black 引用复制到 +0x88。
            capturedFrameAt88_ = backgroundFrameAtA0_;
            state_.phaseAt6C = SaveLoadDialogPhase::ReturnInteractive;
            state_.transitionSubstateAt70 = 0;
        }
        else
        {
            state_.phaseAt6C = SaveLoadDialogPhase::ProcessChildren;
        }
        services_->SubmitDirtyRectsAtVslot58();
        return true;

    case SaveLoadDialogPhase::Finished:
    default:
        return false;
    }
}

bool SaveLoadDialogRuntime::RequestReturnToTitleAt51E320()
{
    if (services_->ReadReturnTitleConfirmationFlagAtRootIndex1Offset7D())
    {
        auto confirmation =
            services_->CreateReturnTitleConfirmationAt4E3330();
        if (!confirmation)
        {
            throw std::logic_error(
                "Save/load return-title confirmation is missing");
        }

        services_->SynchronizeRuntimeAt413BE0();
        const bool accepted = confirmation->RunAt5071D0() == 6;

        // 0x51E41F..0x51E448 在判断返回值和投递消息之前释放局部引用。
        confirmation.reset();
        if (!accepted)
            return false;
    }

    // 0x51E466..0x51E48E：PostMessageA 的返回值不参与结果判定。
    static_cast<void>(::PostMessageA(
        services_->MainWindowAtRootOffset2C(),
        WM_COMMAND,
        0x8015u,
        0));
    services_->WriteSlot2ValueAt04(0);
    return true;
}

void SaveLoadDialogRuntime::ProcessDeleteCommandAt51B8F0()
{
    bool confirmationRequired =
        services_->ReadDeleteConfirmationFlagAtRootIndex1Offset80();
    if (state_.slotIndexAt164 >= 0)
    {
        confirmationRequired =
            services_->ReadDeleteConfirmationFlagAtSlot1Offset7F();
    }

    if (!confirmationRequired)
    {
        static_cast<void>(ExecuteDeleteAt506CE0());
        return;
    }

    if (!deleteConfirmationAt168_)
    {
        throw std::logic_error(
            "Save/load delete confirmation is missing");
    }

    if (state_.pageModeAtE4 == 2)
    {
        deleteConfirmationAt168_->WriteModeAt80(2);
    }
    else if (state_.pageModeAtE4 == 3)
    {
        deleteConfirmationAt168_->WriteModeAt80(3);
    }
    else
    {
        deleteConfirmationAt168_->WriteModeAt80(
            state_.slotIndexAt164 < 0 ? 1 : 0);
        const auto selector = ResolveDialogControl(
            (state_.pageModeAtE4 != 0) + 2);
        if (!selector)
        {
            // 0x51B9F1..0x51BA17 直接读取 selector+0x7C；缺失代表
            // 对象图损坏，不能把它解释成“用户取消”。
            throw std::logic_error(
                "Save/load delete selector control is missing");
        }

        const std::int32_t pageNumber = selector->ReadValueAt7C() + 1;
        if (state_.slotIndexAt164 >= 0)
        {
            deleteConfirmationAt168_->PreparePageAndSlotAt502B10(
                pageNumber,
                state_.slotIndexAt164 + 1);
        }
        else
        {
            deleteConfirmationAt168_->PreparePageAt502950(pageNumber);
        }
    }

    services_->SynchronizeRuntimeAt413BE0();
    if (deleteConfirmationAt168_->RunAt5071D0() == 6)
        static_cast<void>(ExecuteDeleteAt506CE0());
}

bool SaveLoadDialogRuntime::ExecuteDeleteAt506CE0()
{
    SaveLoadDialogControlRef target;
    SaveLoadDeleteFactoryOwner factory;

    if (state_.pageModeAtE4 == 2)
    {
        factory = services_->CreateQuickSaveDeleteFactoryAt45B800();
        if (!factory)
            throw std::logic_error("Quick-save delete factory is missing");
        factory->DeleteAllAt5022E0();
        SaveFileCatalog& catalog = services_->MutableSaveFileCatalogAtSlot6();
        ClearSaveFilePageAt502510(catalog.QuickPage());
        catalog.quickLatestItemAt04 = -1;
        target = ResolveDialogControl(4);
        factory.reset();
    }
    else if (state_.pageModeAtE4 == 3)
    {
        factory = services_->CreateAutoSaveDeleteFactoryAt45B800();
        if (!factory)
            throw std::logic_error("Auto-save delete factory is missing");
        factory->DeleteAllAt5022E0();
        SaveFileCatalog& catalog = services_->MutableSaveFileCatalogAtSlot6();
        ClearSaveFilePageAt502510(catalog.AutoPage());
        catalog.autoLatestItemAt08 = -1;
        target = ResolveDialogControl(5);
        factory.reset();
    }
    else
    {
        target = ResolveDialogControl(
            (state_.pageModeAtE4 != 0) + 2);
        if (!target)
            return false;

        const std::int32_t pageIndex = target->ReadValueAt7C();
        factory = services_->CreateRegularSaveFactoryAt4DCDB0(
            pageIndex + 1);
        if (!factory)
            throw std::logic_error("Regular-save delete factory is missing");
        if (state_.slotIndexAt164 >= 0)
        {
            factory->DeleteSlotAt476460(
                state_.slotIndexAt164 + 1,
                true);
        }
        else
        {
            for (std::int32_t slotNumber = 1; slotNumber <= 8; ++slotNumber)
            {
                factory->DeleteSlotAt476460(slotNumber, true);
            }
        }

        SaveFileCatalog& catalog = services_->MutableSaveFileCatalogAtSlot6();
        if (pageIndex < 0 ||
            static_cast<std::size_t>(pageIndex) >=
                catalog.RegularPages().size())
        {
            throw std::logic_error(
                "Save/load regular page index is outside the catalog");
        }

        SaveFileCatalogPage& page =
            catalog.RegularPages()[static_cast<std::size_t>(pageIndex)];
        if (page.Items().size() != 8)
        {
            throw std::logic_error(
                "Save/load regular page does not contain eight slots");
        }

        if (state_.slotIndexAt164 >= 0)
        {
            if (state_.slotIndexAt164 >= 8)
            {
                throw std::logic_error(
                    "Save/load regular slot index is outside the page");
            }
            ClearSaveFileItemAt5024C0(page.Items()[static_cast<std::size_t>(state_.slotIndexAt164)]);
            UpdateSaveFilePageCountAt48EFE0(page);
        }
        else
        {
            ClearSaveFilePageAt502510(page);
        }

        services_->SynchronizeRuntimeAt413BE0();
        services_->RefreshSaveSelectionAt48DBD0();
        factory.reset();
    }

    if (!target)
        return false;

    const std::int32_t previousValue = target->ReadValueAt7C();
    target->WriteValueAt7C(-1);
    target->NotifySelectionAtVslot8C(previousValue, 1, 0);
    return true;
}

void SaveLoadDialogRuntime::ProcessSaveCommandAt525480()
{
    std::int32_t pageIndex = -1;
    bool occupied = false;
    const auto selector = ResolveDialogControl(2);
    if (selector)
    {
        pageIndex = selector->ReadValueAt7C();
        SaveFileCatalog& catalog = services_->MutableSaveFileCatalogAtSlot6();
        if (pageIndex < 0 ||
            static_cast<std::size_t>(pageIndex) >=
                catalog.RegularPages().size())
        {
            throw std::logic_error(
                "Save/load save page index is outside the catalog");
        }
        const SaveFileCatalogPage& page =
            catalog.RegularPages()[static_cast<std::size_t>(pageIndex)];
        if (state_.slotIndexAt164 < 0 ||
            static_cast<std::size_t>(state_.slotIndexAt164) >= page.Items().size())
        {
            throw std::logic_error(
                "Save/load save slot index is outside the page");
        }
        occupied = page.Items()[static_cast<std::size_t>(state_.slotIndexAt164)].Available();
    }

    bool confirmationRequired =
        services_->ReadSaveConfirmationFlagAtRootIndex1Offset77();
    if (occupied)
    {
        confirmationRequired =
            services_->ReadOverwriteConfirmationFlagAtSlot1Offset7B();
    }
    if (!confirmationRequired)
    {
        static_cast<void>(ExecuteSaveAt532A90());
        return;
    }

    // 0x5255F9..0x525641：+0x174 缺失时直接结束，并不抛出或保存。
    if (!slotConfirmationAt174_)
        return;

    bool accepted = false;
    {
        slotConfirmationAt174_->WriteModeAtVslot98(occupied ? 2 : 1);

        services_->SynchronizeRuntimeAt413BE0();
        const SaveLoadSceneLabelResolution scene =
            services_->ResolveCurrentSaveSceneLabelAt525480And532A90();
        if (!scene.lookupSucceeded || !scene.labelObjectPresent)
            throw 1;

        const std::string pageLabel = FormatDecimal("%04d", pageIndex + 1);
        const std::string slotLabel =
            FormatDecimal("%02d", state_.slotIndexAt164 + 1);
        const std::string timestampLabel =
            FormatSaveLoadTimestamp(services_->ReadCurrentTimeAt56A321());
        const std::string sceneLabel =
            TruncateSceneLabelAt4DA8E0(scene.label, 40, 2);

        slotConfirmationAt174_->ConfigureAt535FF0(
            pageLabel,
            slotLabel,
            timestampLabel,
            sceneLabel,
            true);

        SaveLoadDialogFrameRef preview;
        if (services_->ReadSaveScreenshotFlagAtSlot1Offset74())
        {
            services_->SynchronizeRuntimeAt413BE0();
            preview = services_->CaptureSavePreviewFrameAt47D740();
        }
        else
            preview = savePreviewFrameAt144_;
        slotConfirmationAt174_->AssignPreviewFrameAt538430(preview);

        services_->SynchronizeRuntimeAt413BE0();
        accepted = slotConfirmationAt174_->RunAt5071D0() == 6;
    }

    if (accepted)
        static_cast<void>(ExecuteSaveAt532A90());
}

bool SaveLoadDialogRuntime::ExecuteSaveAt532A90()
{
    services_->SynchronizeRuntimeAt413BE0();
    const SaveLoadSceneLabelResolution scene =
        services_->ResolveCurrentSaveSceneLabelAt525480And532A90();
    if (!scene.lookupSucceeded)
        return false;
    if (!scene.labelObjectPresent)
        throw 1;

    services_->WriteCurrentSaveSceneLabelAtRootIndex0Offset04(scene.label);

    const auto target = ResolveDialogControl(2);
    if (!target)
        return false;

    const std::int32_t pageIndex = target->ReadValueAt7C();
    SaveLoadRegularSaveFactoryOwner factory =
        services_->CreateRegularSaveFactoryAt4DCDB0(pageIndex + 1);
    if (!factory)
        throw std::logic_error("Regular-save factory is missing");

    const std::int32_t slotNumber = state_.slotIndexAt164 + 1;
    const bool writeScreenshot =
        services_->ReadSaveScreenshotFlagAtSlot1Offset74();
    if (factory->SaveSlotAt488CF0(slotNumber, writeScreenshot) != 0)
    {
        services_->ShowSaveFailureMessageAt532A90();
        factory->DeleteSlotAt476460(slotNumber, true);
    }

    services_->SynchronizeRuntimeAt413BE0();
    services_->ReloadRegularSaveCatalogItemAt48EE00(
        pageIndex,
        state_.slotIndexAt164);

    const std::int32_t previousValue = target->ReadValueAt7C();
    target->WriteValueAt7C(-1);
    target->NotifySelectionAtVslot8C(previousValue, 1, 0);

    // 0x532E97..0x532F0F：factory 在最终控件通知之后才析构。
    factory.reset();
    return true;
}

bool SaveLoadDialogRuntime::ProcessLoadCommandAt520AB0()
{
    // 0x520AE5..0x520B34：关闭读档确认时完全跳过 catalog 与预览链。
    if (!services_->ReadLoadConfirmationFlagAtRootIndex1Offset78())
        return ExecuteLoadAt511FB0();

    bool accepted = false;
    {
        SaveFileCatalog& catalog = services_->MutableSaveFileCatalogAtSlot6();
        const SaveFileCatalogPage* page = nullptr;
        std::string pageLabel;
        if (state_.sourceIndexAt160 == -2)
        {
            page = &catalog.AutoPage();
            pageLabel = " Auto";
        }
        else if (state_.sourceIndexAt160 == -1)
        {
            page = &catalog.QuickPage();
            pageLabel = "Quick";
        }
        else
        {
            if (state_.sourceIndexAt160 < 0 ||
                static_cast<std::size_t>(state_.sourceIndexAt160) >=
                    catalog.RegularPages().size())
            {
                throw std::logic_error(
                    "Save/load source page is outside the catalog");
            }
            page = &catalog.RegularPages()[
                static_cast<std::size_t>(state_.sourceIndexAt160)];
            pageLabel = FormatDecimal("%04d", state_.sourceIndexAt160 + 1);
        }

        if (state_.slotIndexAt164 < 0 ||
            static_cast<std::size_t>(state_.slotIndexAt164) >=
                page->Items().size())
        {
            throw std::logic_error(
                "Save/load source slot is outside the catalog page");
        }

        SaveLoadFileFactoryOwner factory =
            CreateLoadFactory(state_.sourceIndexAt160);
        if (!factory)
            throw std::logic_error("Load factory is missing");

        // 0x520CA5：factory 和 catalog 引用已经建立后，才检查共用确认对象。
        if (!slotConfirmationAt174_)
            return false;

        slotConfirmationAt174_->WriteModeAtVslot98(0);
        const SaveFileCatalogItem& item =
            page->Items()[static_cast<std::size_t>(state_.slotIndexAt164)];
        const std::string slotLabel =
            FormatDecimal("%02d", state_.slotIndexAt164 + 1);
        const std::string timestampLabel = FormatSaveLoadTimestamp(
            static_cast<std::int64_t>(item.LastWriteTime()));
        const std::string sceneLabel =
            TruncateSceneLabelAt4DA8E0(RuntimeStringBorrow(item.labelAt0C).Snapshot(), 40, 2);
        slotConfirmationAt174_->ConfigureAt535FF0(
            pageLabel,
            slotLabel,
            timestampLabel,
            sceneLabel,
            false);

        SaveLoadDialogFrameRef preview =
            services_->CreateSavePreviewSurfaceAt40C2E0();
        if (!preview)
            throw std::logic_error("Load preview surface is missing");
        const std::string previewName = factory->BuildPreviewFileNameAt47AFA0(
            state_.slotIndexAt164 + 1);
        const std::string previewPath =
            services_->BuildSavePreviewPathAt4C7210(previewName);
        if (!services_->TryLoadSavePreviewAt511D90(preview, previewPath))
            preview = savePreviewFrameAt144_;
        slotConfirmationAt174_->AssignPreviewFrameAt538430(preview);

        services_->SynchronizeRuntimeAt413BE0();
        accepted = slotConfirmationAt174_->RunAt5071D0() == 6;
    }

    return accepted && ExecuteLoadAt511FB0();
}

bool SaveLoadDialogRuntime::ExecuteLoadAt511FB0()
{
    SaveLoadFileFactoryOwner factory =
        CreateLoadFactory(state_.sourceIndexAt160);
    if (!factory)
        throw std::logic_error("Load factory is missing");

    const std::int32_t error =
        factory->LoadSlotAt480830(state_.slotIndexAt164 + 1);
    if (error != 0)
    {
        services_->ShowLoadFailureMessageAt511FB0();
        return false;
    }

    state_.completionResultAt68 = 1;
    return true;
}

SaveLoadFileFactoryOwner SaveLoadDialogRuntime::CreateLoadFactory(
    const std::int32_t sourceIndex)
{
    if (sourceIndex == -2)
        return services_->CreateAutoLoadFactoryAt4DA870And511FB0();
    if (sourceIndex == -1)
        return services_->CreateQuickLoadFactoryAt4DC5F0();
    if (sourceIndex < 0)
        throw std::logic_error("Save/load source index is invalid");
    return services_->CreateRegularLoadFactoryAt4DCDB0(sourceIndex + 1);
}

void SaveLoadDialogRuntime::SubmitResolvedControlRect(
    const SaveLoadDialogEventTargetRef& control)
{
    if (!control)
        return;

    control->InvokeAtVslot08();
    services_->SubmitControlRectAtVslot64(control->ReadRectAtVslot3C());
}

void SaveLoadDialogRuntime::ProcessChildrenAt522350()
{
    // 0x522350：只有同时通过 +0x28 与 +0x20 的 child 才更新并分发事件。
    for (const auto& child : CurrentChildren())
    {
        if (!child->GateAtVslot28() || !child->StateAtVslot20())
            continue;

        child->UpdateAtVslot48();
        const std::int32_t event = child->EventAtVslot50();

        if (event == 11)
        {
            state_.phaseAt6C = SaveLoadDialogPhase::ReturnInteractive;
            state_.transitionSubstateAt70 = 0;
            return;
        }
        if (event == 10)
        {
            state_.phaseAt6C = SaveLoadDialogPhase::RequestReturnToTitle;
            state_.transitionSubstateAt70 = 0;
            SubmitResolvedControlRect(child->ResolveEventTargetAtVslot14(10));
            return;
        }
        if (event == 12)
        {
            bool allowed = false;
            if (state_.pageModeAtE4 == 2)
            {
                allowed = services_->ReadSlot6ValueAt04() >= 0;
            }
            else if (state_.pageModeAtE4 == 3)
            {
                allowed = services_->ReadSlot6ValueAt08() >= 0;
            }
            else
            {
                const auto selector = ResolveDialogControl(
                    (state_.pageModeAtE4 != 0) + 2);
                if (!selector)
                {
                    // 0x52248D..0x5224B5 没有 null 分支；缺少该控件说明
                    // SaveLoadDialog 对象图不完整，不能伪装成“当前项不可删除”。
                    throw std::logic_error(
                        "Save/load selector control is missing");
                }
                allowed = services_->IsSlot6EntryAt20Present(
                    selector->ReadValueAt7C());
            }

            if (!allowed)
                continue;

            state_.phaseAt6C = SaveLoadDialogPhase::ProcessDeleteCommand;
            state_.transitionSubstateAt70 = 0;
            state_.slotIndexAt164 = -1;
            SubmitResolvedControlRect(child->ResolveEventTargetAtVslot14(12));
            return;
        }
        if (event >= 41 && event <= 48)
        {
            state_.phaseAt6C = SaveLoadDialogPhase::ProcessDeleteCommand;
            state_.slotIndexAt164 = event - 41;
            state_.transitionSubstateAt70 = 0;
            return;
        }
        if (event >= 25 && event <= 32)
        {
            if (state_.pageModeAtE4 == 0)
            {
                state_.phaseAt6C = SaveLoadDialogPhase::ProcessSaveCommand;
                state_.sourceIndexAt160 = -3;
            }
            else
            {
                if (state_.pageModeAtE4 == 2)
                {
                    state_.sourceIndexAt160 = -1;
                }
                else if (state_.pageModeAtE4 == 3)
                {
                    state_.sourceIndexAt160 = -2;
                }
                else
                {
                    const auto selector =
                        ResolveDialogControl(3);
                    if (!selector)
                        return;
                    state_.sourceIndexAt160 = selector->ReadValueAt7C();
                }
                state_.phaseAt6C = SaveLoadDialogPhase::ProcessLoadCommand;
            }
            state_.transitionSubstateAt70 = 0;
            state_.slotIndexAt164 = event - 25;
            return;
        }

        if (event == 9)
        {
            state_.pageModeAtE4 = services_->ReadSlot6PageModeAt14();
            InitializeCurrentPage(state_.pageModeAtE4);
            OpenCurrentChildren();
            services_->SubmitDialogBoundsAtVslot00Then08(dialogBoundsAt78_);
            return;
        }
        if (event == 13)
        {
            state_.pageModeAtE4 = 0;
            InitializeCurrentPage(0);
            OpenCurrentChildren();
            services_->SubmitDialogBoundsAtVslot00Then08(dialogBoundsAt78_);
            return;
        }
        if (event >= 14 && event <= 16)
        {
            state_.pageModeAtE4 = event - 13;
            services_->WriteSlot6PageModeAt14(state_.pageModeAtE4);
            InitializeCurrentPage(state_.pageModeAtE4);
            OpenCurrentChildren();
            services_->SubmitDialogBoundsAtVslot00Then08(dialogBoundsAt78_);
            return;
        }
        if (event == 52)
        {
            const std::int32_t alternate = state_.pageModeAtE4 != 0;
            if (const auto control =
                    ResolveDialogControl(alternate))
            {
                control->WriteAtVslot1C(0);
            }

            std::int32_t selection = 0;
            if (const auto control =
                    ResolveDialogControl(alternate + 2))
            {
                control->WriteAtVslot1C(0);
                selection = control->ReadValueAt7C();
            }

            if (const auto control =
                    ResolveDialogControl(alternate + 6))
            {
                control->WriteAtVslot1C(1);
                control->WriteAtVslot24(1);
                control->WriteValueAt7C(selection);
                control->OpenAtVslot44();
            }
            services_->SubmitDialogBoundsAtVslot00Then08(dialogBoundsAt78_);
            continue;
        }
        if (event == 53 || event == 54)
        {
            const std::int32_t alternate = state_.pageModeAtE4 != 0;
            if (const auto control =
                    ResolveDialogControl(alternate))
            {
                control->WriteAtVslot1C(1);
            }

            std::int32_t selection = 0;
            if (const auto control =
                    ResolveDialogControl(alternate + 6))
            {
                control->WriteAtVslot1C(0);
                control->WriteAtVslot24(0);
                if (event == 54)
                    selection = control->ReadValueAt84();
            }

            if (const auto control =
                    ResolveDialogControl(alternate + 2))
            {
                control->WriteAtVslot1C(1);
                if (event == 54)
                {
                    control->WriteValueAt7C(selection);
                    control->OpenAtVslot44();
                }
            }
            services_->SubmitDialogBoundsAtVslot00Then08(dialogBoundsAt78_);
        }
    }
}

void SaveLoadDialogRuntime::AdvanceTransitionAt51CA80()
{
    // 0x51CA80：子状态 0 建立 operation，1 推进并提交，2 回写主状态。
    switch (state_.transitionSubstateAt70)
    {
    case 0:
    {
        const bool entering =
            state_.phaseAt6C == SaveLoadDialogPhase::EnterInteractive;
        const SaveLoadDialogFrameRef& sourceFrame =
            entering ? transitionFrameAt94_ : capturedFrameAt88_;

        if (entering)
            services_->CompositeTransitionFrameAtVslot5C(transitionFrameAt94_, 0);

        state_.transitionAtAC.Reset();
        state_.transitionAtAC.selectorAt1C = entering ? 57 : 58;
        state_.transitionAtAC.sourceHandleAt04 =
            services_->ResolveTransitionSourceAt444770(sourceFrame, 0);
        state_.nextPhaseAt74 = entering
            ? static_cast<std::int32_t>(SaveLoadDialogPhase::ProcessChildren)
            : static_cast<std::int32_t>(SaveLoadDialogPhase::Complete);
        state_.transitionAtAC.valueAt2C =
            services_->ResolveTransitionStepAt497900();
        state_.transitionSubstateAt70 = 1;
        return;
    }

    case 1:
    {
        auto& operation = state_.transitionAtAC;
        if (operation.selectorAt1C == -1 ||
            operation.progressAt24 >= operation.maximumAt28)
        {
            state_.transitionSubstateAt70 = 2;
        }

        if (services_->PollTransitionAccelerationAt4C3430() ||
            services_->PollTransitionAccelerationAt4AA970())
        {
            operation.progressAt24 = WrapAdd(operation.progressAt24, 256);
            if (operation.progressAt24 >= operation.maximumAt28)
                operation.progressAt24 = operation.maximumAt28;
        }

        services_->RenderTransitionAt449DD0(operation);
        operation.progressAt24 =
            WrapAdd(operation.progressAt24, operation.valueAt2C);
        if (operation.progressAt24 >= operation.maximumAt28)
            operation.progressAt24 = operation.maximumAt28;
        services_->CommitTransitionFrameAt4B02E0(0);
        return;
    }

    case 2:
        state_.phaseAt6C =
            static_cast<SaveLoadDialogPhase>(state_.nextPhaseAt74);
        return;

    default:
        return;
    }
}

void SaveLoadDialogRuntime::EndAtVslot4C()
{
    // 0x508CF0：不倒序，也不因前一项返回值而短路。
    for (const auto& child : CurrentChildren())
        child->EndAtVslot4C();
}

const std::vector<SaveLoadDialogChildRef>&
SaveLoadDialogRuntime::CurrentChildren() const noexcept
{
    return objectGraph_ != nullptr
        ? objectGraph_->ChildrenInInsertionOrder()
        : children_;
}

void SaveLoadDialogRuntime::InitializeCurrentPage(const std::int32_t mode)
{
    if (objectGraph_ != nullptr)
    {
        objectGraph_->InitializePageAt5330A0(mode);
        return;
    }
    services_->InitializePageAt5330A0(mode);
}

void SaveLoadDialogRuntime::OpenCurrentChildren()
{
    if (objectGraph_ != nullptr)
    {
        objectGraph_->OpenChildrenAt532560();
        return;
    }
    services_->RefreshPageAt532560();
}

SaveLoadDialogControlRef SaveLoadDialogRuntime::ResolveDialogControl(
    const std::int32_t slot)
{
    return objectGraph_ != nullptr
        ? objectGraph_->ResolveDialogControlAtVslot90(slot)
        : services_->ResolveDialogControlAtVslot90(slot);
}

std::uint32_t SaveLoadDialogRuntime::CompletionResultAtVslot50() const
{
    // 0x50ABB0：原函数只读取 +0x68。
    return state_.completionResultAt68;
}

const SaveLoadDialogControlState& SaveLoadDialogRuntime::ControlState() const noexcept
{
    return state_;
}

SaveLoadDialogControlState& SaveLoadDialogRuntime::MutableControlState() noexcept
{
    return state_;
}

const SaveLoadDialogFrameRef& SaveLoadDialogRuntime::CapturedFrameAt88() const noexcept
{
    return capturedFrameAt88_;
}

const SaveLoadDialogFrameRef& SaveLoadDialogRuntime::TransitionFrameAt94() const noexcept
{
    return transitionFrameAt94_;
}

const SaveLoadDialogFrameRef& SaveLoadDialogRuntime::BackgroundFrameAtA0() const noexcept
{
    return backgroundFrameAtA0_;
}

std::int32_t SaveLoadDialogRuntime::PageModeAtE4() const noexcept
{
    return state_.pageModeAtE4;
}

void SaveLoadDialogRuntime::ReleaseConfirmationsAt4F9030() noexcept
{
    // 4F905C..4F90E8：+0x174 必须先于 +0x168 释放。
    slotConfirmationAt174_.reset();
    deleteConfirmationAt168_.reset();
}

void SaveLoadDialogRuntime::ReleasePreviewReferenceAt4F9030() noexcept
{
    // 重写版直接保存 +0x144 首项引用；在原 +0x144 vector 前撤销该别名。
    savePreviewFrameAt144_.reset();
}

void SaveLoadDialogRuntime::ReleaseSurfaceFramesAt4F9030() noexcept
{
    // 4F91D9..4F92AB：原顺序固定为 +0xA0、+0x94、+0x88。
    backgroundFrameAtA0_.reset();
    transitionFrameAt94_.reset();
    capturedFrameAt88_.reset();
}
}
