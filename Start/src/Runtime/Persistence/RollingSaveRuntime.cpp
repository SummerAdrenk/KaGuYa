#include <Runtime/RollingSaveRuntime.h>

#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/SaveFileCatalog.h>
#include <Runtime/SaveLoadFileFactoryRuntime.h>
#include <Runtime/ScrScriptRuntime.h>
#include <Runtime/RuntimeCStringResult.h>
#include <Runtime/SaveLoadResourceProfile.h>

#include <bit>
#include <cstdio>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原5B2AC8/5B2AA4的CP932字节，分别为自动/快速存档标题加两位序号。
constexpr char AutoTitle[] = "\x83\x49\x81\x5b\x83\x67\x83\x5a\x81\x5b\x83\x75%02d";
constexpr char QuickTitle[] = "\x83\x4e\x83\x43\x83\x62\x83\x4e\x83\x5a\x81\x5b\x83\x75%02d";

std::int32_t IncrementDword(std::int32_t value) noexcept
{
    return std::bit_cast<std::int32_t>(std::bit_cast<std::uint32_t>(value) + 1u);
}

std::string FormatOneInteger(const char* pattern, std::int32_t value)
{
    const int size = std::snprintf(nullptr, 0, pattern, value);
    if (size < 0) throw std::runtime_error("rolling save format failed");
    std::string text(static_cast<std::size_t>(size) + 1u, '\0');
    if (std::snprintf(text.data(), text.size(), pattern, value) != size)
        throw std::runtime_error("rolling save format changed size");
    text.resize(static_cast<std::size_t>(size));
    return text;
}
}

RollingSaveCatalogRefreshRuntime::RollingSaveCatalogRefreshRuntime(
    SaveFileCatalog& catalog, IStartFileApi& files,
    const SaveDataEnvironmentArchive& save, const std::string& directoryCp932,
    IScrVmProgramRandSource& random, const SaveFileFormatProfile& profile) noexcept
    : catalog_(catalog), files_(files), save_(save), directory_(directoryCp932),
      random_(random), profile_(profile) {}

void RollingSaveCatalogRefreshRuntime::RefreshRollingItemAt48E5F0(
    RollingSaveKind kind, std::int32_t item)
{
    // 48E649先构造临时CParams；失败读取的部分发布仅留在此临时对象。
    ScrVmParamsGeneration temporary;
    ScrSaveFileArchiveStateRuntime state(temporary);
    SaveFileContainerRuntime container(files_, state, save_, directory_, random_, profile_);
    const auto pattern = directory_ + (kind == RollingSaveKind::Quick ? "qsave%02d.dat" : "asave%02d.dat");
    const auto path = FormatOneInteger(pattern.c_str(), IncrementDword(item));
    if (!container.ReadAt4C7420(path, std::uint8_t{1})) return;

    // 48E724/48E9D5先分配timestamp正文，之后才查询并替换目标项。
    const SaveFileNativeTime time{static_cast<std::uint32_t>(temporary.GetSaveTimestamp()),
        static_cast<std::uint32_t>(temporary.GetSaveTimestamp() >> 32u)};
    auto* timestamp = CreateSaveFileTimeAt470A40(time);
    auto& page = kind == RollingSaveKind::Quick ? catalog_.QuickPage() : catalog_.AutoPage();
    auto& entry = page.Items().at(static_cast<std::size_t>(item));
    ReplaceSaveFileTimeAt470A6E(entry.timeAt00, timestamp);
    RuntimeCStringResult label([&](RuntimeCString* out)
    {
        const auto& description = temporary.SaveParamsAt08().saveDescriptionAt04;
        ConstructRuntimeCStringLeftAt419D00(out, description, description.GetLength());
    });
    if (label.Value().GetLength() == 0)
    {
        const RuntimeCStringResult title([&](RuntimeCString* out)
        {
            const auto& source = temporary.SaveParamsAt08().saveTitleAt08;
            ConstructRuntimeCStringLeftAt419D00(out, source, source.GetLength());
        });
        AssignRuntimeCStringAt410220(label.Value(), title.Value());
    }
    AssignRuntimeCStringAt410220(entry.labelAt0C, label.Value());

    // 48EFE0数timestamp Ref非空项，quick/auto在增量刷新后也可大于1。
    UpdateSaveFilePageCountAt48EFE0(page);
}

RollingSaveRuntime::RollingSaveRuntime(ScrScriptRuntimeState& script,
    ScrVmParamsGenerationBinding generation, SaveFileCatalog& catalog,
    const SaveDataEnvironmentArchive& save, IStartFileApi& files,
    ISaveLoadPreviewServices& preview, ISaveLoadDeleteFileApi& deletion,
    const std::string& directoryCp932, IScrVmProgramRandSource& random,
    const SaveFileFormatProfile& profile, IRollingSaveConfirmationServices& confirmation)
    : script_(script), generation_(std::move(generation)), catalog_(catalog), save_(save),
      preview_(preview), deletion_(deletion), directory_(directoryCp932), confirmation_(confirmation),
      archive_(generation_), container_(files, archive_, save_, directory_, random, profile),
      refresh_(catalog_, files, save_, directory_, random, profile)
{
    if (!script_.UsesParamsGenerationSlot(generation_))
        throw std::invalid_argument("rolling save script and archive must share generation");
}

std::int32_t& RollingSaveRuntime::Latest(RollingSaveKind kind) noexcept
{
    return kind == RollingSaveKind::Quick ? catalog_.quickLatestItemAt04 : catalog_.autoLatestItemAt08;
}

SaveLoadFileFactoryRuntime RollingSaveRuntime::CreateFactory(RollingSaveKind kind)
{
    return SaveLoadFileFactoryRuntime(container_, preview_, deletion_, directory_,
        kind == RollingSaveKind::Quick ? "qsave" : "asave",
        CurrentSaveLoadResourceProfile().catalog.slotsPerPage, Latest(kind));
}

std::int32_t RollingSaveRuntime::Roll(SaveLoadFileFactoryRuntime& factory,
    RollingSaveKind kind, std::uint8_t captureFresh)
{
    const auto oldLatest = Latest(kind);
    Latest(kind) = IncrementDword(Latest(kind));
    if (Latest(kind) >= CurrentSaveLoadResourceProfile().catalog.slotsPerPage) Latest(kind) = 0;
    RuntimeCStringResult oldTitle([&](RuntimeCString* out)
    {
        const auto& source = generation_->Current().SaveParamsAt08().saveTitleAt08;
        ConstructRuntimeCStringLeftAt419D00(out, source, source.GetLength());
    });
    RuntimeCStringResult title([](RuntimeCString* out) { InitializeEmptyRuntimeCString(*out); });
    FormatRuntimeCStringAt417130(title.Value(), kind == RollingSaveKind::Quick ? QuickTitle : AutoTitle,
        IncrementDword(Latest(kind)));
    script_.SetCurrentTitle(RuntimeStringView(title.Value()));
    const std::int32_t previewMode = save_.bytesAt74[0] == 0 ? 0 : (captureFresh != 0 ? 2 : 1);
    std::int32_t result = 0;
    if (factory.SaveSlotAt488CF0(IncrementDword(Latest(kind)), previewMode) != 0)
    {
        // 原失败分支重新读latest。删除异常保留新latest和临时标题，不执行恢复。
        factory.DeleteSlotAt476460(IncrementDword(Latest(kind)), true);
        Latest(kind) = oldLatest;
        result = 1;
    }
    // 48CF90接收C字符串。恢复只在正常到达此处时执行，其异常仍外传。
    script_.SetCurrentTitle(RuntimeStringView(oldTitle.Value()));
    return result;
}

std::int32_t RollingSaveRuntime::RollAutoAt4883F0(std::uint8_t captureFresh)
{
    auto factory = CreateFactory(RollingSaveKind::Auto);
    return Roll(factory, RollingSaveKind::Auto, captureFresh);
}

std::int32_t RollingSaveRuntime::RollQuickAt4888C0(std::uint8_t captureFresh)
{
    auto factory = CreateFactory(RollingSaveKind::Quick);
    return Roll(factory, RollingSaveKind::Quick, captureFresh);
}

void RollingSaveRuntime::AutoSaveAt46B280(std::uint8_t captureFresh)
{
    auto factory = CreateFactory(RollingSaveKind::Auto);
    SetRuntimeCStringAt437680(generation_->Current().SaveParamsAt08().saveDescriptionAt04, "", 0);
    if (Roll(factory, RollingSaveKind::Auto, captureFresh) == 0)
        refresh_.RefreshRollingItemAt48E5F0(RollingSaveKind::Auto, Latest(RollingSaveKind::Auto));
}

void RollingSaveRuntime::QuickSaveAt485FC0(std::int32_t captureFresh)
{
    if (save_.bytesAt77[2] != 0)
    {
        bool accepted;
        {
            auto dialog = confirmation_.CreateQuickSaveConfirmationAt4E3330();
            if (!dialog) throw std::logic_error("quick save confirmation returned no object");
            dialog->CaptureDisplayAt48CA30();
            accepted = dialog->RunModalAt5071D0() == 6;
        }
        if (!accepted) return;
    }
    auto factory = CreateFactory(RollingSaveKind::Quick);
    SetRuntimeCStringAt437680(generation_->Current().SaveParamsAt08().saveDescriptionAt04, "", 0);
    if (Roll(factory, RollingSaveKind::Quick, static_cast<std::uint8_t>(captureFresh)) == 0)
        refresh_.RefreshRollingItemAt48E5F0(RollingSaveKind::Quick, Latest(RollingSaveKind::Quick));
}
}
