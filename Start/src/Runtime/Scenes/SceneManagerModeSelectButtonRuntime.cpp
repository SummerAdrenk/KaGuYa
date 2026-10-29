#include <Runtime/SceneManagerModeSelectButtonRuntime.h>

#include <array>
#include <stdexcept>
#include <string>

namespace KaGuYa::Start::Runtime
{
namespace
{
struct ModeSelectButtonRecord final
{
    std::array<const char*, 7> resources{};
    POINT position{};
    std::int32_t timelineIndex = -1;
};

constexpr char kSceneOn[] =
    "\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x90\xD8\x91\xD6"
    "\x83\x56\x81\x5B\x83\x93\x83\x49\x83\x93.vrm";
constexpr char kSceneTarget[] =
    "\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x90\xD8\x91\xD6"
    "\x83\x56\x81\x5B\x83\x93\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67.vrm";
constexpr char kSceneClick[] =
    "\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x90\xD8\x91\xD6"
    "\x83\x56\x81\x5B\x83\x93\x83\x4E\x83\x8A\x83\x62\x83\x4E.vrm";
constexpr char kMusicOn[] =
    "\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x90\xD8\x91\xD6"
    "\x89\xB9\x8A\x79\x83\x49\x83\x93.vrm";
constexpr char kMusicTarget[] =
    "\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x90\xD8\x91\xD6"
    "\x89\xB9\x8A\x79\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67.vrm";
constexpr char kMusicClick[] =
    "\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x90\xD8\x91\xD6"
    "\x89\xB9\x8A\x79\x83\x4E\x83\x8A\x83\x62\x83\x4E.vrm";
constexpr char kTitleOn[] =
    "\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x90\xD8\x91\xD6"
    "\x83\x5E\x83\x43\x83\x67\x83\x8B\x83\x49\x83\x93.vrm";
constexpr char kTitleTarget[] =
    "\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x90\xD8\x91\xD6"
    "\x83\x5E\x83\x43\x83\x67\x83\x8B\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67.vrm";
constexpr char kTitleClick[] =
    "\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x90\xD8\x91\xD6"
    "\x83\x5E\x83\x43\x83\x67\x83\x8B\x83\x4E\x83\x8A\x83\x62\x83\x4E.vrm";
constexpr char kCgOn[] =
    "\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x90\xD8\x91\xD6"
    "CG\x83\x49\x83\x93.vrm";
constexpr char kCgTarget[] =
    "\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x90\xD8\x91\xD6"
    "CG\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67.vrm";
constexpr char kCgClick[] =
    "\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x90\xD8\x91\xD6"
    "CG\x83\x4E\x83\x8A\x83\x62\x83\x4E.vrm";

constexpr ModeSelectButtonRecord kEmptyRecord{};
constexpr ModeSelectButtonRecord kSceneRecord{
    {kSceneOn, kSceneTarget, kSceneClick, nullptr, nullptr, nullptr, nullptr},
    POINT{1061, 566}, -1};
constexpr ModeSelectButtonRecord kSceneMusicModeRecord{
    {kSceneOn, kSceneTarget, kSceneClick, nullptr, nullptr, nullptr, nullptr},
    POINT{1061, 621}, -1}; // 原6017C0附近第9条；mode2第二行，不复用mode0的y=566。
constexpr ModeSelectButtonRecord kMusicRecord{
    {kMusicOn, kMusicTarget, kMusicClick, nullptr, nullptr, nullptr, nullptr},
    POINT{1061, 621}, -1};
constexpr ModeSelectButtonRecord kTitleRecord{
    {kTitleOn, kTitleTarget, kTitleClick, nullptr, nullptr, nullptr, nullptr},
    POINT{1061, 676}, -1};
constexpr ModeSelectButtonRecord kCgRecord{
    {kCgOn, kCgTarget, kCgClick, nullptr, nullptr, nullptr, nullptr},
    POINT{1061, 566}, -1};

constexpr std::array<std::array<ModeSelectButtonRecord, 4>, 3> kRecords{{
    {kEmptyRecord, kSceneRecord, kMusicRecord, kTitleRecord},
    {kCgRecord, kEmptyRecord, kMusicRecord, kTitleRecord},
    {kCgRecord, kSceneMusicModeRecord, kEmptyRecord, kTitleRecord},
}};
}

SceneManagerModeSelectButtonRuntime::SceneManagerModeSelectButtonRuntime(
    const std::int32_t mode,
    ICgUiButtonRuntimeServices& buttonServices,
    IGraphicsSurfaceCompositionApi& compositionApi,
    ICgOperationSurfaceFactory& surfaceFactory)
{
    if (mode < 0 || mode >= static_cast<std::int32_t>(kRecords.size()))
        throw std::out_of_range("sample 18 mode-select button mode is outside 0..2");

    const auto& records = kRecords[static_cast<std::size_t>(mode)];
    for (std::size_t buttonIndex = 0; buttonIndex < buttons_.size(); ++buttonIndex)
    {
        auto button = std::make_shared<CgUiButtonRuntime>(
            buttonServices, compositionApi);
        const auto& record = records[buttonIndex];
        for (std::size_t slot = 0; slot < record.resources.size(); ++slot)
        {
            const char* const resourceName = record.resources[slot];
            if (resourceName == nullptr)
                continue;
            auto surface = surfaceFactory.LoadStatic(resourceName);
            if (!surface || !button->AttachNamedSurfaceAt4C1F20(
                    slot, std::string(resourceName), std::move(surface)))
            {
                // 45ADDA..45AE68：任一 0x480930 失败均抛出整数 1。
                throw 1;
            }
        }
        button->SetPositionAt24(record.position);
        button->SetUsePositionAt30At2C(false);
        button->SetTimelineIndexAt68(record.timelineIndex);
        button->ResetInteractionState();
        buttons_[buttonIndex] = std::move(button);
    }
}

void SceneManagerModeSelectButtonRuntime::ResetButtonsAt487470()
{
    // 487488..4874BB：对四项逐一写 state(+0x04)=0、latch(+0x20)=0。
    for (const auto& button : buttons_)
        button->ResetInteractionState();
}

std::vector<std::shared_ptr<CgUiButtonRuntime>>
SceneManagerModeSelectButtonRuntime::AcquireButtonsAt47B7B0()
{
    // 47B7B0 返回 CModeSelectButton+0x04 的 array-ref；调用者复制引用，
    // 因而重写版按同样顺序返回四项共享所有权，而不是复制按钮对象。
    return {buttons_.begin(), buttons_.end()};
}

const std::array<std::shared_ptr<CgUiButtonRuntime>, 4>&
SceneManagerModeSelectButtonRuntime::ButtonsAt47B7B0() const noexcept
{
    return buttons_;
}
}
