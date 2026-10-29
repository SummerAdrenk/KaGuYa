#include <Runtime/MusicAppreciationButtonRuntime.h>

#include <algorithm>
#include <array>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::array<std::string_view, 3> kDialogBaseResources{
    "\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x5F\x83\x78\x81\x5B\x83\x58\x2E\x76\x72\x6D",
    "\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x5F\x98\x67\x2E\x76\x72\x6D",
    "\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x5F\x83\x5E\x83\x43\x83\x67\x83\x8B\x2E\x76\x72\x6D",
};

constexpr MusicAppreciationCheckboxStaticData kCheckbox{
    2,
    "\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x5F\x83\x49\x83\x74\x2E\x76\x72\x6D",
    "\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x5F\x83\x49\x83\x74\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D",
    "\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x5F\x83\x49\x83\x74\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D",
    "\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x5F\x83\x49\x83\x74\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D",
    "\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x5F\x91\x49\x91\xF0\x94\xCD\x88\xCD\x2E\x76\x72\x6D",
    "\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x5F\x83\x49\x83\x93\x2E\x76\x72\x6D",
    "\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x5F\x83\x49\x83\x93\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D",
    "\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x5F\x83\x49\x83\x93\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D",
    "\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x5F\x83\x49\x83\x93\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D",
    6,
    -1,
};

constexpr std::array<POINT, 18> kPositions{{
    {162, 139}, {685, 139}, {148, 194}, {671, 194},
    {134, 249}, {657, 249}, {120, 305}, {643, 305},
    {106, 361}, {629, 361}, {92, 416}, {615, 416},
    {78, 472}, {601, 472}, {64, 528}, {587, 528},
    {50, 583}, {573, 583},
}};

constexpr std::array<std::string_view, 18> kSoundResources{
    "BGM01.bgm", "BGM02.bgm", "BGM03.bgm", "BGM04.bgm", "BGM05.bgm",
    "BGM06.bgm", "BGM07.bgm", "BGM08.bgm", "BGM09.bgm", "BGM10.bgm",
    "BGM11.bgm", "BGM12.bgm", "BGM13.bgm", "BGM14.bgm", "BGM15.bgm",
    "BGM16.bgm", "BGM17.bgm", "BGM18.bgm",
};

constexpr auto MakeItems()
{
    std::array<MusicAppreciationItemStaticData, 18> result{};
    for (std::size_t index = 0; index < result.size(); ++index)
    {
        result[index] = MusicAppreciationItemStaticData{
            static_cast<std::int32_t>(index) + 2,
            kPositions[index],
            kSoundResources[index]};
    }
    return result;
}

constexpr auto kItems = MakeItems();

constexpr std::array<MusicAppreciationNavigationPair, 18> kHorizontalPairs{{
    {2, 3}, {3, 2}, {4, 5}, {5, 4}, {6, 7}, {7, 6},
    {8, 9}, {9, 8}, {10, 11}, {11, 10}, {12, 13}, {13, 12},
    {14, 15}, {15, 14}, {16, 17}, {17, 16}, {18, 19}, {19, 18},
}};

constexpr std::array<MusicAppreciationNavigationPair, 18> kUpPairs{{
    {2, 18}, {3, 19}, {4, 2}, {5, 3}, {6, 4}, {7, 5},
    {8, 6}, {9, 7}, {10, 8}, {11, 9}, {12, 10}, {13, 11},
    {14, 12}, {15, 13}, {16, 14}, {17, 15}, {18, 16}, {19, 17},
}};

constexpr std::array<MusicAppreciationNavigationPair, 18> kDownPairs{{
    {2, 4}, {3, 5}, {4, 6}, {5, 7}, {6, 8}, {7, 9},
    {8, 10}, {9, 11}, {10, 12}, {11, 13}, {12, 14}, {13, 15},
    {14, 16}, {15, 17}, {16, 18}, {17, 19}, {18, 2}, {19, 3},
}};

constexpr std::array<MusicAppreciationOperationKeyStaticData, 4> kOperationKeys{{
    {3, kHorizontalPairs, -1, 0},
    {4, kHorizontalPairs, -1, 0},
    {1, kUpPairs, 15, 2},
    {2, kDownPairs, 15, 2},
}};

const MusicAppreciationButtonStaticData kSample18StaticData{
    kDialogBaseResources,
    kCheckbox,
    kItems,
    kOperationKeys,
    2,
};
}

const MusicAppreciationButtonStaticData&
Sample18MusicAppreciationButtonStaticData() noexcept
{
    return kSample18StaticData;
}

MusicAppreciationButtonRuntime::MusicAppreciationButtonRuntime(
    IMusicAppreciationButtonServices& services,
    const MusicAppreciationButtonStaticData& staticData)
    : services_(&services), staticData_(&staticData)
{
    // 0x4A1C70 先完成 CBaseBtn::0x4A2E60，再构造 ModeSoundDialog。
    dirtyBatch_ = services_->CreateDirtyBatchAt4A2E60();
    if (!dirtyBatch_)
        throw std::invalid_argument("CSoundBtn base dirty batch is null");
    navigationState_ = &services_->NavigationStateAt4A2E60();

    dialogAt50_ = services_->CreateModeSoundDialogAt4E3900(staticData);
    if (!dialogAt50_)
        throw std::invalid_argument("CSoundBtn ModeSoundDialog is null");
    dialogAt50_->InitializeAtVslot44();

    skippedGuiChildAt40_ = staticData.skippedGuiChildAt40;
    guiChildrenAt44_ = services_->AcquireGuiChildrenAt47B7B0();
    itemCountAt3C_ = static_cast<std::int32_t>(staticData.items.size());
    if (dialogAt50_->ItemCount() != itemCountAt3C_)
    {
        throw std::invalid_argument(
            "CSoundBtn dialog item count differs from injected static table");
    }

    selectedItemAt68_ = -1;
    movieSelectionPendingAt6C_ = 0u;
    InvalidateAtVslot08(services_->ReadGraphicsTargetRectAt4180C0());
    SynchronizeAt4ABB40();
    movieSelectionPendingAt6C_ = 0u;
}

MusicAppreciationButtonRuntime::~MusicAppreciationButtonRuntime()
{
    // 0x4A70B0：+0x5C 在样本 18 构造后始终为空；随后依次释放 +0x50、+0x44，
    // 最后由基类 0x4A4BB0 释放 dirty owner。显式 reset 防止成员声明顺序掩盖该顺序。
    dialogAt50_.reset();
    guiChildrenAt44_.reset();
    dirtyBatch_.reset();
}

std::int32_t MusicAppreciationButtonRuntime::ProcessSelectionAtVslot04()
{
    std::int32_t result = services_->PollBaseSelectionAt4AA200();
    if (result == -1 && guiChildrenAt44_)
    {
        for (std::size_t index = 0; index < guiChildrenAt44_->CountAtPlus10(); ++index)
        {
            if (static_cast<std::int32_t>(index) == skippedGuiChildAt40_)
                continue;
            const std::int32_t oldState = guiChildrenAt44_->StateAt04(index);
            if (guiChildrenAt44_->ProcessAt4A98E0(index, false) >= 2)
                result = itemCountAt3C_ + static_cast<std::int32_t>(index);
            if (oldState != guiChildrenAt44_->StateAt04(index))
                InvalidateAtVslot08(guiChildrenAt44_->VisibleRect(index));
        }
    }

    if (result == -1 || result == -2)
    {
        (void)dialogAt50_->UpdateAtVslot48();
        result = dialogAt50_->SelectionAtVslot50();
        for (const RECT& rect : dialogAt50_->EventDirtyRectsAtVslots00_18_1C())
            InvalidateAtVslot08(rect);
    }

    movieSelectionPendingAt6C_ = 0u;
    if (result >= 0 && result < itemCountAt3C_)
        SelectSoundAt4C1A20(result);
    if (result >= itemCountAt3C_)
        services_->EmitExitSoundAt4BCD70();
    return result;
}

void MusicAppreciationButtonRuntime::InvalidateAtVslot08(RECT rect)
{
    // 0x4B4570 使用 retn 0x10，RECT 四个 LONG 由值传递。
    if (::IsRectEmpty(&rect) == FALSE)
        dirtyBatch_->AddAtVslot08(rect);
}

std::int32_t MusicAppreciationButtonRuntime::ItemCountAt3C() const
{
    return itemCountAt3C_;
}

std::int32_t MusicAppreciationButtonRuntime::SelectedItemAt68() const
{
    return selectedItemAt68_;
}

void MusicAppreciationButtonRuntime::WriteSelectedItemAt68(
    const std::int32_t value)
{
    selectedItemAt68_ = value;
}

std::uint8_t MusicAppreciationButtonRuntime::MovieSelectionPendingAt6C() const
{
    return movieSelectionPendingAt6C_;
}

void MusicAppreciationButtonRuntime::WriteMovieSelectionPendingAt6C(
    const std::uint8_t value)
{
    movieSelectionPendingAt6C_ = value;
}

void MusicAppreciationButtonRuntime::ClearSelectedItemStateAt04And20(
    const std::int32_t index)
{
    dialogAt50_->ClearItemStateAt04And20(index);
}

std::int32_t MusicAppreciationButtonRuntime::ItemIndexFromControlId(
    const std::int32_t controlId) const noexcept
{
    const auto iterator = std::find_if(
        staticData_->items.begin(), staticData_->items.end(),
        [controlId](const MusicAppreciationItemStaticData& item)
        {
            return item.controlId == controlId;
        });
    return iterator == staticData_->items.end()
        ? -1
        : static_cast<std::int32_t>(iterator - staticData_->items.begin());
}

const MusicAppreciationOperationKeyStaticData&
MusicAppreciationButtonRuntime::OperationKeyForAction(
    const std::int32_t actionAt04) const
{
    const auto iterator = std::find_if(
        staticData_->operationKeys.begin(), staticData_->operationKeys.end(),
        [actionAt04](const MusicAppreciationOperationKeyStaticData& key)
        {
            return key.actionAt04 == actionAt04;
        });
    if (iterator == staticData_->operationKeys.end())
        throw std::out_of_range("CSoundBtn operation-key action is not in static data");
    return *iterator;
}

std::int32_t MusicAppreciationButtonRuntime::MoveSelectionAtVslot30(
    const std::int32_t actionAt04)
{
    std::int32_t current = navigationState_->CurrentItemAtVslot2C();
    navigationState_->ResetItemAtVslot18(current);

    if (current < 0)
    {
        for (std::int32_t index = 0; index < itemCountAt3C_; ++index)
        {
            if (navigationState_->IsItemNavigableAt4B4920(index))
            {
                current = index;
                break;
            }
        }
    }
    else
    {
        const auto& operation = OperationKeyForAction(actionAt04);
        const std::int32_t origin = current;
        for (std::size_t attempt = 0; attempt < operation.navigationPairs.size(); ++attempt)
        {
            if (current < 0 || current >= itemCountAt3C_)
                throw std::out_of_range("CSoundBtn current navigation item is outside static data");
            const std::int32_t sourceControlId =
                staticData_->items[static_cast<std::size_t>(current)].controlId;
            const auto pair = std::find_if(
                operation.navigationPairs.begin(), operation.navigationPairs.end(),
                [sourceControlId](const MusicAppreciationNavigationPair& value)
                {
                    return value.sourceControlId == sourceControlId;
                });
            if (pair == operation.navigationPairs.end())
                throw std::out_of_range("CSoundBtn navigation source is absent");

            const std::int32_t candidate =
                ItemIndexFromControlId(pair->destinationControlId);
            if (candidate < 0)
                throw std::out_of_range("CSoundBtn navigation destination is absent");
            if (candidate == origin)
            {
                current = origin;
                break;
            }
            if (navigationState_->IsItemNavigableAt4B4920(candidate))
            {
                current = candidate;
                break;
            }
            current = candidate;
        }
    }

    navigationState_->WriteCurrentItemAtVslot28(current);
    return navigationState_->FocusItemAtVslot1C(current);
}

void MusicAppreciationButtonRuntime::SynchronizeAt4ABB40()
{
    dirtyBatch_->PrepareAtVslot04();
    const std::span<const RECT> rects = dirtyBatch_->RectsAtVslots18And1C();
    for (const RECT& clip : rects)
    {
        dialogAt50_->DrawAtVslot60(&clip);
        if (guiChildrenAt44_)
        {
            for (std::size_t index = 0;
                 index < guiChildrenAt44_->CountAtPlus10(); ++index)
            {
                if (static_cast<std::int32_t>(index) != skippedGuiChildAt40_)
                    guiChildrenAt44_->DrawCurrentStateAt441980(index, clip);
            }
        }
        services_->SubmitInvalidatedRect(clip);
    }
    if (!rects.empty())
        dirtyBatch_->ClearAtVslot00();
}

void MusicAppreciationButtonRuntime::SelectSoundAt4C1A20(
    const std::int32_t itemIndex)
{
    if (selectedItemAt68_ == itemIndex)
    {
        selectedItemAt68_ = -1;
        services_->ClearSoundMappingAt437680();
        services_->StopCurrentSoundAt4383B0();
        return;
    }

    selectedItemAt68_ = itemIndex;
    if (itemIndex < 0 || itemIndex >= itemCountAt3C_)
    {
        services_->ClearSoundMappingAt437680();
        services_->StopCurrentSoundAt4383B0();
        return;
    }

    const std::string_view resource =
        staticData_->items[static_cast<std::size_t>(itemIndex)].soundResource;
    if (services_->ReadExtensionAt45DA40(resource) == "mpg")
    {
        services_->ClearSoundMappingAt437680();
        services_->StopCurrentSoundAt4383B0();
        movieSelectionPendingAt6C_ = 1u;
        return;
    }
    services_->BindAndPlayAt4BCCC0(resource);
}
}
