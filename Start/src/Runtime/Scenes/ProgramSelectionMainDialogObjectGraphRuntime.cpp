#include <Runtime/ProgramSelectionMainDialogObjectGraphRuntime.h>

#include <algorithm>
#include <array>
#include <bit>
#include <stdexcept>
#include <string>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr char kDietBase[] =
    "\x76\x72\x6D\x3A\x5C\x83\x5F\x83\x43\x83\x47\x83\x62\x83\x67"
    "\x83\x51\x81\x5B\x83\x80\x89\xE6\x96\xCA";
constexpr char kHotSpringBase[] =
    "\x76\x72\x6D\x3A\x5C\x89\xB7\x90\xF2\x97\xB7\x8D\x73\x83\x4C"
    "\x83\x83\x83\x89\x91\x49\x91\xF0\x89\xE6\x96\xCA";
constexpr char kHeartVegetable[] = "\x90\x53\x8D\xD8";
constexpr char kSnowLove[] = "\x90\xE1\x88\xA4";
constexpr char kBruette[] =
    "\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67";
constexpr char kKayoko[] = "\x89\xC4\x91\xE3\x8E\x71";
constexpr char kHarem[] = "\x83\x6E\x81\x5B\x83\x8C\x83\x80";
constexpr char kSoftTraining[] =
    "\x83\x5C\x83\x74\x83\x67\x83\x67\x83\x8C\x81\x5B\x83\x6A\x83\x93\x83\x4F";
constexpr char kNormalTraining[] =
    "\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x83\x67\x83\x8C\x81\x5B\x83\x6A\x83\x93\x83\x4F";
constexpr char kHardTraining[] =
    "\x83\x6E\x81\x5B\x83\x68\x83\x67\x83\x8C\x81\x5B\x83\x6A\x83\x93\x83\x4F";
constexpr char kSpecialMenu[] =
    "\x93\xC1\x8E\xEA\x83\x81\x83\x6A\x83\x85\x81\x5B";
constexpr char kTravelButton[] =
    "\x97\xB7\x8D\x73\x8F\x6F\x94\xAD\x83\x7B\x83\x5E\x83\x93";
constexpr char kSkipButton[] =
    "\x53\x4B\x49\x50\x83\x7B\x83\x5E\x83\x93";
constexpr char kWeight[] = "\x91\xCC\x8F\x64";
constexpr char kStress[] = "\x83\x58\x83\x67\x83\x8C\x83\x58";
constexpr char kTraining[] =
    "\x83\x67\x83\x8C\x81\x5B\x83\x6A\x83\x93\x83\x4F";
constexpr char kRemainingCount[] = "\x8E\x63\x82\xE8\x89\xF1\x90\x94";
constexpr char kIntimacy[] = "\x90\x65\x96\xA7\x93\x78";
constexpr char kDependence[] = "\x88\xCB\x91\xB6\x93\x78";
constexpr char kEventType[] =
    "\x83\x43\x83\x78\x83\x93\x83\x67\x83\x5E\x83\x43\x83\x76";
constexpr char kRoute[] = "\x83\x8B\x81\x5B\x83\x67";

struct TrainingRecord final
{
    std::int32_t layerEvent;
    std::int32_t weightDecrease;
    std::int32_t stressIncrease;
    std::int32_t intimacyIncrease;
    std::int32_t dependenceIncrease;
};

// 0x608C40：角色 × 训练类型 × 当前训练等级，共 27 项。
static constexpr std::array<TrainingRecord, 27> kTrainingRecords{{
    {9, 1, 1, 0, 0}, {10, 1, 1, 0, 0}, {11, 1, 1, 0, 0},
    {12, 1, 1, 3, 0}, {13, 2, 2, 4, 0}, {14, 2, 2, 5, 0},
    {15, 2, 2, 0, 3}, {16, 2, 3, 0, 4}, {17, 4, 4, 0, 5},
    {18, 1, 1, 0, 0}, {19, 2, 1, 0, 0}, {20, 2, 1, 0, 0},
    {21, 1, 1, 2, 0}, {22, 2, 2, 4, 0}, {23, 3, 2, 5, 0},
    {24, 2, 2, 0, 2}, {25, 2, 2, 0, 4}, {26, 4, 4, 0, 5},
    {27, 1, 1, 0, 0}, {28, 1, 1, 0, 0}, {29, 2, 1, 0, 0},
    {30, 1, 1, 3, 0}, {31, 2, 2, 4, 0}, {32, 3, 3, 5, 0},
    {33, 2, 2, 0, 3}, {34, 3, 3, 0, 4}, {35, 5, 4, 0, 5},
}};

[[nodiscard]] std::int32_t WrapAdd(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    const auto bits = static_cast<std::uint32_t>(left) +
        static_cast<std::uint32_t>(right);
    return std::bit_cast<std::int32_t>(bits);
}

[[nodiscard]] std::int32_t WrapSubtract(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    const auto bits = static_cast<std::uint32_t>(left) -
        static_cast<std::uint32_t>(right);
    return std::bit_cast<std::int32_t>(bits);
}

[[nodiscard]] RECT SurfaceRect(const RuntimeGraphicsSurface& surface) noexcept
{
    return RECT{
        surface.OriginX(), surface.OriginY(),
        surface.OriginX() + surface.Width(),
        surface.OriginY() + surface.Height()};
}

void UnionInto(RECT& destination, const RECT& source) noexcept
{
    RECT combined{};
    ::UnionRect(&combined, &destination, &source);
    destination = combined;
}

[[nodiscard]] std::shared_ptr<RuntimeGraphicsSurface> LoadRequired(
    ICgOperationSurfaceFactory& factory,
    const std::string& name)
{
    auto result = factory.LoadStatic(name);
    if (!result)
        throw 1;
    return result;
}

[[nodiscard]] CgOperationChildDescriptor StaticDescriptor(
    const std::int32_t eventValue,
    const std::string& name,
    const std::optional<POINT> origin = std::nullopt)
{
    return {
        CgOperationControlKind::Static,
        eventValue,
        {{{name, {}, {}, name, name, {}, {}}}, origin},
        7,
        -1};
}

[[nodiscard]] CgOperationChildDescriptor ButtonDescriptor(
    const std::int32_t eventValue,
    const std::string& prefix,
    const bool offState,
    const bool hitCheck,
    const std::int32_t hitTestSlot)
{
    const std::string target = prefix + "_target.alp";
    return {
        CgOperationControlKind::Button,
        eventValue,
        {{{prefix + "_on.alp",
           offState ? prefix + "_off.alp" : prefix + "_lock.alp",
           {}, target, target, prefix + "_click.alp",
           hitCheck ? prefix + "_hitcheck.alp" : std::string{}}},
         std::nullopt},
        hitTestSlot,
        -1};
}

[[nodiscard]] CgOperationChildDescriptor SparseButtonDescriptor(
    const std::int32_t eventValue,
    const std::string& prefix)
{
    const std::string target = prefix + "_target.alp";
    return {
        CgOperationControlKind::Button,
        eventValue,
        {{{prefix + "_on.alp", {}, {}, target, target,
           prefix + "_click.alp", {}}}, std::nullopt},
        0,
        -1};
}

[[nodiscard]] std::vector<CgOperationChildDescriptor> BuildMode00Descriptors()
{
    std::vector<CgOperationChildDescriptor> result;
    result.reserve(39u);
    for (std::size_t index = 0; index < 3u; ++index)
    {
        const std::array<const char*, 3> characters{
            kHeartVegetable, kSnowLove, kBruette};
        result.push_back(ButtonDescriptor(
            40 + static_cast<std::int32_t>(index),
            std::string(kDietBase) + characters[index], true, true, 6));
    }
    const std::array<const char*, 5> commands{
        kSoftTraining, kNormalTraining, kHardTraining,
        kSpecialMenu, kTravelButton};
    for (std::size_t index = 0; index < commands.size(); ++index)
    {
        result.push_back(ButtonDescriptor(
            43 + static_cast<std::int32_t>(index),
            std::string(kDietBase) + commands[index], false, false, 0));
    }

    constexpr char normalEvent[] =
        "\x83\x43\x83\x78\x83\x93\x83\x67\x83\x56\x83\x93\x83\x7B\x83\x8B"
        "\x5F\x92\xCA\x8F\xED\x83\x43\x83\x78\x83\x93\x83\x67\x2E\x61\x6C\x70";
    constexpr char hEvent[] =
        "\x83\x43\x83\x78\x83\x93\x83\x67\x83\x56\x83\x93\x83\x7B\x83\x8B"
        "\x5F\x48\x83\x43\x83\x78\x83\x93\x83\x67\x2E\x61\x6C\x70";
    constexpr char levelUpEvent[] =
        "\x83\x43\x83\x78\x83\x93\x83\x67\x83\x56\x83\x93\x83\x7B\x83\x8B"
        "\x5F\x83\x8C\x83\x78\x83\x8B\x83\x41\x83\x62\x83\x76\x83\x43\x83\x78\x83\x93\x83\x67\x2E\x61\x6C\x70";
    result.push_back(StaticDescriptor(36, std::string(kDietBase) + normalEvent,
        POINT{484, 490}));
    result.push_back(StaticDescriptor(37, std::string(kDietBase) + hEvent,
        POINT{484, 490}));
    result.push_back(StaticDescriptor(38, std::string(kDietBase) + levelUpEvent,
        POINT{484, 490}));

    const std::array<const char*, 3> characters{
        kHeartVegetable, kSnowLove, kBruette};
    const std::array<const char*, 3> trainingNames{
        kSoftTraining, kNormalTraining, kHardTraining};
    std::int32_t eventValue = 9;
    for (const char* const character : characters)
    {
        for (const char* const training : trainingNames)
        {
            for (std::int32_t level = 1; level <= 3; ++level)
            {
                result.push_back(StaticDescriptor(
                    eventValue++, std::string(kDietBase) + character + "_" +
                    training + "LV" + std::to_string(level) + ".alp"));
            }
        }
    }
    result.push_back(StaticDescriptor(
        5, std::string(kDietBase) +
        "_\x92\xC7\x89\xC1\x83\x56\x83\x93\x83\x7B\x83\x8B.alp"));
    return result;
}

[[nodiscard]] std::vector<CgOperationChildDescriptor> BuildMode01Descriptors()
{
    std::vector<CgOperationChildDescriptor> result;
    result.reserve(20u);
    const std::array<const char*, 5> choices{
        kHeartVegetable, kSnowLove, kBruette, kKayoko, kHarem};
    for (std::size_t index = 0; index < choices.size(); ++index)
    {
        result.push_back(ButtonDescriptor(
            17 + static_cast<std::int32_t>(index),
            std::string(kHotSpringBase) + "_" + choices[index],
            false, true, 6));
    }
    result.push_back(SparseButtonDescriptor(
        22, std::string(kHotSpringBase) + "_" + kSkipButton));

    constexpr char normalEvent[] =
        "\x83\x43\x83\x78\x83\x93\x83\x67\x83\x56\x83\x93\x83\x7B\x83\x8B"
        "\x5F\x92\xCA\x8F\xED\x83\x43\x83\x78\x83\x93\x83\x67\x2E\x61\x6C\x70";
    constexpr char hEvent[] =
        "\x83\x43\x83\x78\x83\x93\x83\x67\x83\x56\x83\x93\x83\x7B\x83\x8B"
        "\x5F\x48\x83\x43\x83\x78\x83\x93\x83\x67\x2E\x61\x6C\x70";
    constexpr char personalRoute[] =
        "\x83\x43\x83\x78\x83\x93\x83\x67\x83\x56\x83\x93\x83\x7B\x83\x8B"
        "\x5F\x8C\xC2\x90\x6C\x83\x8B\x81\x5B\x83\x67\x2E\x61\x6C\x70";
    constexpr char haremRoute[] =
        "\x83\x43\x83\x78\x83\x93\x83\x67\x83\x56\x83\x93\x83\x7B\x83\x8B"
        "\x5F\x83\x6E\x81\x5B\x83\x8C\x83\x80\x83\x8B\x81\x5B\x83\x67\x2E\x61\x6C\x70";
    const std::array<POINT, 5> positions{{
        {352, 433}, {557, 433}, {760, 355}, {962, 419}, {1168, 434}}};
    std::int32_t eventValue = 3;
    for (std::size_t index = 0; index < 4u; ++index)
    {
        for (const char* const suffix : {normalEvent, hEvent, personalRoute})
        {
            result.push_back(StaticDescriptor(
                eventValue++, std::string(kHotSpringBase) + suffix,
                positions[index]));
        }
    }
    result.push_back(StaticDescriptor(
        eventValue++, std::string(kHotSpringBase) + normalEvent, positions[4]));
    result.push_back(StaticDescriptor(
        eventValue, std::string(kHotSpringBase) + haremRoute, positions[4]));
    return result;
}

[[nodiscard]] std::string CharacterKey(
    const std::size_t index,
    const char* const suffix)
{
    const std::array<const char*, 3> characters{
        kHeartVegetable, kSnowLove, kBruette};
    return std::string(characters.at(index)) + "\x81\x40" + suffix;
}

[[nodiscard]] std::vector<GuiDialogNavigationMapping> Mode00Navigation(
    const bool forward)
{
    if (forward)
    {
        return {{40, 41}, {41, 42}, {42, 47}, {47, 43},
                {43, 44}, {44, 45}, {45, 46}, {46, 40}};
    }
    return {{40, 46}, {41, 40}, {42, 41}, {47, 42},
            {43, 47}, {44, 43}, {45, 44}, {46, 45}};
}

[[nodiscard]] std::vector<GuiDialogNavigationMapping> Mode01Navigation(
    const bool forward)
{
    if (forward)
        return {{17, 18}, {18, 19}, {19, 20}, {20, 21}, {21, 22}, {22, 17}};
    return {{17, 22}, {18, 17}, {19, 18}, {20, 19}, {21, 20}, {22, 21}};
}
}

DietTrainingPartsRuntime::DietTrainingPartsRuntime(
    ICgOperationSurfaceFactory& factory,
    IGraphicsSurfaceCompositionApi& graphics,
    IProgramSelectionMainDirtyRectSink* const dirtyRectSink)
    : graphics_(&graphics), dirtyRectSink_(dirtyRectSink)
{
    stressResourcesAt0C_.reserve(4u);
    for (std::int32_t index = 1; index <= 4; ++index)
    {
        auto resource = LoadRequired(
            factory, std::string(kDietBase) +
            "\x83\x58\x83\x67\x83\x8C\x83\x58_" +
            std::to_string(index) + ".alp");
        UnionInto(stressRectAt18_, SurfaceRect(*resource));
        stressResourcesAt0C_.push_back(std::move(resource));
    }

    overlayResourcesAt28_.reserve(16u);
    for (std::int32_t index = 1; index <= 3; ++index)
    {
        overlayResourcesAt28_.push_back(LoadRequired(
            factory, std::string(kDietBase) +
            "\x83\x67\x83\x8C\x81\x5B\x83\x6A\x83\x93\x83\x4F"
            "\x83\x8C\x83\x78\x83\x8B_" + std::to_string(index) + ".alp"));
    }
    for (std::int32_t index = 1; index <= 2; ++index)
    {
        overlayResourcesAt28_.push_back(LoadRequired(
            factory, std::string(kDietBase) +
            "\x83\x8C\x83\x78\x83\x8B\x83\x41\x83\x62\x83\x76"
            "\x89\xF1\x90\x94_" + std::to_string(index) + ".alp"));
    }
    constexpr char weightDigits[] =
        "\x76\x72\x6D\x3A\x5C\x91\xCC\x8F\x64_\x90\x94\x8E\x9A";
    for (std::int32_t digit = 0; digit <= 9; ++digit)
    {
        overlayResourcesAt28_.push_back(LoadRequired(
            factory, std::string(weightDigits) + std::to_string(digit) + ".alp"));
    }
    overlayResourcesAt28_.push_back(LoadRequired(
        factory, std::string(weightDigits) + "\x81\x7C.alp"));

    trainingEffectAt44_ = LoadRequired(factory,
        std::string(kDietBase) +
        "\x83\x67\x83\x8C\x81\x5B\x83\x6A\x83\x93\x83\x4F"
        "\x8C\xF8\x89\xCA\x95\x5C\x8E\xA6.alp");
    statusIncreaseAt50_ = LoadRequired(factory,
        std::string(kDietBase) +
        "\x83\x58\x83\x65\x81\x5B\x83\x5E\x83\x58\x8F\xE3\x8F\xB8.alp");
    statusDecreaseAt5C_ = LoadRequired(factory,
        std::string(kDietBase) +
        "\x83\x58\x83\x65\x81\x5B\x83\x5E\x83\x58\x89\xBA\x8D\x7E.alp");
    trainingCountBaseAt68_ = LoadRequired(factory,
        std::string(kDietBase) +
        "_\x83\x67\x83\x8C\x81\x5B\x83\x6A\x83\x93\x83\x4F"
        "\x83\x8C\x83\x78\x83\x8B\x89\xF1\x90\x94.alp");

    combinedRectAt34_ = stressRectAt18_;
    for (std::size_t index = 0; index < 5u; ++index)
        UnionInto(combinedRectAt34_, SurfaceRect(*overlayResourcesAt28_[index]));

    RECT digitShape{};
    for (std::size_t index = 5u; index < 15u; ++index)
        UnionInto(digitShape, SurfaceRect(*overlayResourcesAt28_[index]));
    const LONG digitWidth = digitShape.right - digitShape.left;
    const LONG digitHeight = digitShape.bottom - digitShape.top;
    UnionInto(combinedRectAt34_, RECT{581, 134, 581 + digitWidth, 134 + digitHeight});
    UnionInto(combinedRectAt34_, RECT{532, 134, 532 + digitWidth, 134 + digitHeight});
    for (const auto* const resource : {
        trainingEffectAt44_.get(), statusIncreaseAt50_.get(),
        statusDecreaseAt5C_.get(), trainingCountBaseAt68_.get()})
    {
        UnionInto(combinedRectAt34_, SurfaceRect(*resource));
    }

    outputSurfacesAt00_.reserve(3u);
    const std::int32_t width = combinedRectAt34_.right - combinedRectAt34_.left;
    const std::int32_t height = combinedRectAt34_.bottom - combinedRectAt34_.top;
    for (std::size_t index = 0; index < 3u; ++index)
    {
        auto output = factory.CreateBlank(width, height, true, true);
        if (!output)
            throw 1;
        output->SetOrigin(combinedRectAt34_.left, combinedRectAt34_.top);
        outputSurfacesAt00_.push_back(std::move(output));
    }
}

void DietTrainingPartsRuntime::ComposeAt5279C0(
    const DietTrainingPartsState& state)
{
    if (state.characterIndex < 0 ||
        static_cast<std::size_t>(state.characterIndex) >= outputSurfacesAt00_.size())
    {
        return;
    }
    RuntimeGraphicsSurface& destination =
        *outputSurfacesAt00_[static_cast<std::size_t>(state.characterIndex)];
    graphics_->ClearSurface(destination.Handle(), 0u, nullptr);

    if (state.stressLevel >= 1 && state.stressLevel <= 4)
    {
        BlitAtResourceOrigin(destination,
            *stressResourcesAt0C_[static_cast<std::size_t>(state.stressLevel - 1)]);
    }
    if (state.weight < 0)
    {
        throw std::out_of_range(
            "Diet training weight is outside the proven non-negative domain");
    }
    // 0x5279C0 在非零体重值前固定绘制资源表第 15 项“体重_数字－”。
    if (state.weight != 0)
        BlitAtResourceOrigin(destination, OverlayAt(15u));
    const std::int32_t ones = state.weight % 10;
    const std::int32_t tens = state.weight / 10;
    BlitAtAbsolutePosition(destination, OverlayAt(5u + ones), 581, 134);
    BlitAtAbsolutePosition(destination, OverlayAt(5u + tens), 532, 134);

    if (state.trainingLevel >= 0 && state.trainingLevel < 3)
    {
        BlitAtResourceOrigin(destination,
            OverlayAt(static_cast<std::size_t>(state.trainingLevel)));
    }
    if (state.remainingCount != 0)
    {
        if (state.remainingCount < 1 || state.remainingCount > 2)
        {
            throw std::out_of_range(
                "Diet training remaining count is outside the original resource table");
        }
        BlitAtResourceOrigin(destination, *trainingCountBaseAt68_);
        BlitAtResourceOrigin(destination,
            OverlayAt(static_cast<std::size_t>(state.remainingCount + 2)));
    }
    if (state.showStatusIncrease)
        BlitAtResourceOrigin(destination, *statusIncreaseAt50_);
    if (state.showStatusDecrease)
        BlitAtResourceOrigin(destination, *statusDecreaseAt5C_);
    if (state.showTrainingEffect)
        BlitAtResourceOrigin(destination, *trainingEffectAt44_);
    if (state.submitDirtyRect)
    {
        if (dirtyRectSink_ == nullptr)
        {
            throw std::logic_error(
                "Diet training dirty-rect sink is not connected");
        }
        dirtyRectSink_->SubmitRectAtVslot64(SurfaceRect(destination));
    }
}

const std::vector<std::shared_ptr<RuntimeGraphicsSurface>>&
DietTrainingPartsRuntime::OutputSurfacesAt00() const noexcept
{
    return outputSurfacesAt00_;
}

const RECT& DietTrainingPartsRuntime::CombinedRectAt18() const noexcept
{
    return combinedRectAt34_;
}

const RuntimeGraphicsSurface& DietTrainingPartsRuntime::OverlayAt(
    const std::size_t index) const
{
    return *overlayResourcesAt28_.at(index);
}

void DietTrainingPartsRuntime::BlitAtResourceOrigin(
    RuntimeGraphicsSurface& destination,
    const RuntimeGraphicsSurface& source)
{
    BlitAtAbsolutePosition(
        destination, source, source.OriginX(), source.OriginY());
}

void DietTrainingPartsRuntime::BlitAtAbsolutePosition(
    RuntimeGraphicsSurface& destination,
    const RuntimeGraphicsSurface& source,
    const std::int32_t x,
    const std::int32_t y)
{
    if (!graphics_->BlitSurface(
            destination.Handle(), source.Handle(),
            x - destination.OriginX(), y - destination.OriginY(),
            nullptr, nullptr, nullptr, 0))
    {
        throw 1;
    }
}

std::span<const CgOperationChildDescriptor>
ProgramSelectionMainDescriptorsSample18(const ProgramSelectionDialogMode mode)
{
    static const auto mode00 = BuildMode00Descriptors();
    static const auto mode01 = BuildMode01Descriptors();
    if (mode == ProgramSelectionDialogMode::Mode00)
        return mode00;
    if (mode == ProgramSelectionDialogMode::Mode01)
        return mode01;
    throw std::invalid_argument("ProgramSelection main dialog mode is invalid");
}

ProgramSelectionMainDialogObjectGraphRuntime::
ProgramSelectionMainDialogObjectGraphRuntime(
    ICgOperationSurfaceFactory& surfaceFactory,
    IGraphicsSurfaceCompositionApi& graphics,
    ICgOperationButtonInputServices& buttonInputServices,
    IProgramSelectionMainValueSource& values,
    IProgramSelectionMainDialogServices& services,
    const ProgramSelectionDialogMode mode,
    IProgramSelectionMainDirtyRectSink* const dirtyRectSink)
    : mode_(mode), values_(&values), services_(&services)
{
    if (mode_ == ProgramSelectionDialogMode::Mode00)
    {
        const std::array<const char*, 3> baseSuffixes{
            "_\x90\x53\x8D\xD8\x83\x78\x81\x5B\x83\x58.bmp",
            "_\x90\xE1\x88\xA4\x83\x78\x81\x5B\x83\x58.bmp",
            "_\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67"
            "\x83\x78\x81\x5B\x83\x58.bmp"};
        for (std::size_t index = 0; index < baseSuffixes.size(); ++index)
        {
            const auto descriptor = StaticDescriptor(
                2 + static_cast<std::int32_t>(index),
                std::string(kDietBase) + baseSuffixes[index]);
            auto resources =
                std::make_shared<CgOperationControlResourceGroupRuntime>(
                    surfaceFactory, descriptor.resources);
            AddChild(std::make_shared<CgOperationChildControlRuntime>(
                descriptor.kind, descriptor.eventValueAt04,
                descriptor.childIdAt0C, descriptor.initialResourceSlotAt24,
                std::move(resources)));
        }

        dietPartsAt74_ = std::make_shared<DietTrainingPartsRuntime>(
            surfaceFactory, graphics, dirtyRectSink);
        for (std::size_t index = 0;
             index < dietPartsAt74_->OutputSurfacesAt00().size(); ++index)
        {
            auto resources =
                std::make_shared<CgOperationControlResourceGroupRuntime>(
                    dietPartsAt74_->OutputSurfacesAt00()[index], 0u);
            AddChild(std::make_shared<CgOperationChildControlRuntime>(
                CgOperationControlKind::Static,
                6 + static_cast<std::int32_t>(index), -1, 7,
                std::move(resources)));

            DietTrainingPartsState state;
            state.characterIndex = static_cast<std::int32_t>(index);
            state.weight = values.ReadNamedValueAt4C73A0(
                CharacterKey(index, "\x91\xCC\x8F\x64"));
            state.stressLevel = values.ReadNamedValueAt4C73A0(
                CharacterKey(index, "\x83\x58\x83\x67\x83\x8C\x83\x58"));
            state.trainingLevel = values.ReadNamedValueAt4C73A0(
                CharacterKey(index,
                    "\x83\x67\x83\x8C\x81\x5B\x83\x6A\x83\x93\x83\x4F"));
            state.remainingCount = values.ReadNamedValueAt4C73A0(
                CharacterKey(index, "\x8E\x63\x82\xE8\x89\xF1\x90\x94"));
            dietPartsAt74_->ComposeAt5279C0(state);
        }
    }
    else if (mode_ == ProgramSelectionDialogMode::Mode01)
    {
        const auto baseDescriptor = StaticDescriptor(
            2, std::string(kHotSpringBase) + "_\x83\x78\x81\x5B\x83\x58.bmp");
        auto baseResources =
            std::make_shared<CgOperationControlResourceGroupRuntime>(
                surfaceFactory, baseDescriptor.resources);
        RuntimeGraphicsSurface& base = *baseResources->ResourceForSlot(0u);

        const std::int32_t time =
            values.ReadNamedValueAt4C73A0("time");
        if (time < 0 || time > 1)
            throw std::out_of_range("ProgramSelection time is outside 0..1");
        const std::array<const char*, 2> timeNames{
            "_\x8E\x9E\x8A\xD4\x91\xD1_\x92\x8B.alp",
            "_\x8E\x9E\x8A\xD4\x91\xD1_\x96\xE9.alp"};
        auto timeSurface = LoadRequired(
            surfaceFactory, std::string(kHotSpringBase) + timeNames[time]);
        if (!graphics.BlitSurface(
                base.Handle(), timeSurface->Handle(),
                timeSurface->OriginX(), timeSurface->OriginY(),
                nullptr, nullptr, nullptr, 0))
        {
            throw 1;
        }

        std::vector<std::shared_ptr<RuntimeGraphicsSurface>> digits;
        digits.reserve(10u);
        for (std::int32_t digit = 0; digit <= 9; ++digit)
        {
            digits.push_back(LoadRequired(
                surfaceFactory, std::string(kHotSpringBase) +
                "\x93\xFA\x95\x74_\x90\x94\x8E\x9A" +
                std::to_string(digit) + ".alp"));
        }
        const auto drawDigit = [&](const std::int32_t digit,
                                   const std::int32_t x,
                                   const std::int32_t y) {
            if (!graphics.BlitSurface(
                    base.Handle(), digits.at(static_cast<std::size_t>(digit))->Handle(),
                    x, y, nullptr, nullptr, nullptr, 0))
            {
                throw 1;
            }
        };
        const std::int32_t month = values.ReadNamedValueAt4C73A0("month");
        const std::int32_t date = values.ReadNamedValueAt4C73A0("date");
        if (month < 0 || date < 0)
        {
            throw std::out_of_range(
                "ProgramSelection date is outside the proven non-negative domain");
        }
        drawDigit(month % 10, 11, 27);
        drawDigit(date % 10, 79, 27);
        if (const std::int32_t tens = date / 10; tens != 0)
            drawDigit(tens, 55, 27);

        AddChild(std::make_shared<CgOperationChildControlRuntime>(
            baseDescriptor.kind, baseDescriptor.eventValueAt04,
            baseDescriptor.childIdAt0C, baseDescriptor.initialResourceSlotAt24,
            std::move(baseResources)));
    }
    else
    {
        throw std::invalid_argument("ProgramSelection main dialog mode is invalid");
    }

    for (const auto& descriptor : ProgramSelectionMainDescriptorsSample18(mode_))
    {
        auto resources =
            std::make_shared<CgOperationControlResourceGroupRuntime>(
                surfaceFactory, descriptor.resources);
        auto child = std::make_shared<CgOperationChildControlRuntime>(
            descriptor.kind, descriptor.eventValueAt04,
            descriptor.childIdAt0C, descriptor.initialResourceSlotAt24,
            std::move(resources));
        if (descriptor.kind == CgOperationControlKind::Button)
            child->BindButtonInputServicesAt4C(buttonInputServices);
        AddChild(std::move(child));
    }

    const bool mode01 = mode_ == ProgramSelectionDialogMode::Mode01;
    clickedEventsAt5C_ = CgOperationClickedEventManagerRuntime::
        CreateForProgramSelectionMainAt4DCEB0(children_, mode01);
    navigationGroupsAt24_.push_back(
        std::make_shared<GuiDialogNavigationGroupRuntime>(
            3, mode01 ? Mode01Navigation(false) : Mode00Navigation(false)));
    navigationGroupsAt24_.push_back(
        std::make_shared<GuiDialogNavigationGroupRuntime>(
            4, mode01 ? Mode01Navigation(true) : Mode00Navigation(true)));
}

void ProgramSelectionMainDialogObjectGraphRuntime::AddChild(
    const CgOperationChildControlRef& child)
{
    if (!child)
        throw std::invalid_argument("ProgramSelection main child is null");
    UnionInto(combinedRect_, child->ReadRectAtVslot3C());
    children_.push_back(child);
}

ProgramSelectionDialogMode
ProgramSelectionMainDialogObjectGraphRuntime::Mode() const noexcept
{
    return mode_;
}

const std::vector<CgOperationChildControlRef>&
ProgramSelectionMainDialogObjectGraphRuntime::
ChildrenInInsertionOrder() const noexcept
{
    return children_;
}

const std::shared_ptr<CgOperationClickedEventManagerRuntime>&
ProgramSelectionMainDialogObjectGraphRuntime::ClickedEventsAt5C() const noexcept
{
    return clickedEventsAt5C_;
}

const std::vector<GuiDialogNavigationGroupRef>&
ProgramSelectionMainDialogObjectGraphRuntime::NavigationGroupsAt24() const noexcept
{
    return navigationGroupsAt24_;
}

const RECT& ProgramSelectionMainDialogObjectGraphRuntime::CombinedRect() const noexcept
{
    return combinedRect_;
}

const std::shared_ptr<DietTrainingPartsRuntime>&
ProgramSelectionMainDialogObjectGraphRuntime::DietPartsAt74() const noexcept
{
    return dietPartsAt74_;
}

void ProgramSelectionMainDialogObjectGraphRuntime::OpenAtVslot44()
{
    if (mode_ == ProgramSelectionDialogMode::Mode00)
    {
        // 0x50C230：mode 0 比 mode 1 多出当前角色、结果和主 phase 三项写回。
        clickedEventsAt5C_->ClearAllAt532200();
        services_->WriteDialogEnabledAtVslot24(1);
        for (const auto& child : children_)
            child->InvokeAtVslot08();
        services_->PrepareNavigationAtVslot68();
        selectedCharacterAt8C_ = 0;
        RefreshMode00At5009C0(1);
        resultAt90_ = -1;
        phaseAt98_ = 0;
        return;
    }
    if (mode_ != ProgramSelectionDialogMode::Mode01)
    {
        throw std::logic_error("ProgramSelection Main mode is invalid");
    }

    // 0x50C2A0：清事件、启用、重置 child、准备导航、应用 0x501880，
    // 最后才把 +0x80 结果写成 -1。
    clickedEventsAt5C_->ClearAllAt532200();
    services_->WriteDialogEnabledAtVslot24(1);
    for (const auto& child : children_)
        child->InvokeAtVslot08();
    services_->PrepareNavigationAtVslot68();
    ApplyMode01AvailabilityAt501880();
    resultAt80_ = -1;
}

std::int32_t ProgramSelectionMainDialogObjectGraphRuntime::UpdateAtVslot48()
{
    if (mode_ == ProgramSelectionDialogMode::Mode01)
    {
        // 0x52D040：继承更新先行；17..22 直接映射为结果 0..5。
        services_->UpdateInheritedAtVslot54();
        const std::int32_t event = clickedEventsAt5C_->PollFirstAt50FDB0(1);
        if (event < 17 || event > 22)
            return 1;
        resultAt80_ = event - 17;
        return 0;
    }
    if (mode_ != ProgramSelectionDialogMode::Mode00)
        throw std::logic_error("ProgramSelection Main mode is invalid");

    // 0x52C990：phase 0 可连续处理角色按钮；训练成功后在同一帧进入 phase 1。
    services_->UpdateInheritedAtVslot54();
    for (;;)
    {
        switch (phaseAt98_)
        {
        case 0:
        {
            bool enteredTraining = false;
            for (const auto& clicked :
                 clickedEventsAt5C_->EventsInInsertionOrder())
            {
                if (clicked->ClickedFlagAt04() == 0)
                    continue;

                const std::int32_t event = clicked->EventValueAt08();
                clicked->ClearAtVslot04();
                if (event >= 40 && event <= 42)
                {
                    selectedCharacterAt8C_ = event - 40;
                    RefreshMode00At5009C0(1);
                    services_->SubmitPrimaryBoundsAtVslot64();
                    services_->EmitMappedSoundAt4BCD70(
                        ProgramSelectionMainSoundAt4BCD70::
                            SelectOrExitAt61A658);
                    continue;
                }
                if (event >= 43 && event <= 45)
                {
                    selectedTrainingAtBC_ = event - 43;
                    if (CalculateTrainingAt50BCC0() == 0)
                        return 1;
                    ++phaseAt98_;
                    enteredTraining = true;
                    services_->EmitMappedSoundAt4BCD70(
                        ProgramSelectionMainSoundAt4BCD70::
                            TrainingConfirmAt61A65C);
                    break;
                }
                if (event == 46 || event == 47)
                {
                    if (event == 47)
                        selectedCharacterAt8C_ = -1;
                    selectedTrainingAtBC_ = event - 43;
                    services_->EmitMappedSoundAt4BCD70(
                        ProgramSelectionMainSoundAt4BCD70::
                            SelectOrExitAt61A658);
                    return 0;
                }
            }
            if (!enteredTraining)
                return 1;
            continue;
        }
        case 1:
            ComposeMode00At5279C0(
                stressBeforeAtA8_, weightBeforeAtA0_, true, false, false, false);
            RefreshMode00At5009C0(0);
            services_->SubmitPrimaryBoundsAtVslot64();
            services_->DisableCapturedPrimaryLayerAt52CF2E();
            ++phaseAt98_;
            animationStateAt9C_ = 0;
            animationCounterAtB8_ = 0;
            animationDelayAtB4_ = 60;
            return 1;
        case 2:
            AdvanceMode00AnimationAt51BB50();
            return 1;
        case 3:
            static_cast<void>(CloseAtVslot4C());
            return 0;
        default:
            return 1;
        }
    }
}

void ProgramSelectionMainDialogObjectGraphRuntime::ComposeMode00At5279C0(
    const std::int32_t stress,
    const std::int32_t weight,
    const bool showTrainingEffect,
    const bool showStatusIncrease,
    const bool showStatusDecrease,
    const bool submitDirtyRect)
{
    if (!dietPartsAt74_)
        throw std::logic_error("ProgramSelection diet parts are not connected");
    DietTrainingPartsState state;
    state.characterIndex = selectedCharacterAt8C_;
    state.stressLevel = stress;
    state.trainingLevel = trainingLevelAtB0_;
    state.weight = weight;
    state.showTrainingEffect = showTrainingEffect;
    state.showStatusIncrease = showStatusIncrease;
    state.showStatusDecrease = showStatusDecrease;
    state.submitDirtyRect = submitDirtyRect;
    dietPartsAt74_->ComposeAt5279C0(state);
}

void ProgramSelectionMainDialogObjectGraphRuntime::AdvanceMode00AnimationAt51BB50()
{
    if (services_->ShouldForceAnimationCompletionAt51BB50())
    {
        if (animationStateAt9C_ < 3)
            animationStateAt9C_ = 3;
        animationDelayAtB4_ = 0;
    }
    else if (animationDelayAtB4_ != 0)
    {
        --animationDelayAtB4_;
        return;
    }

    for (;;)
    {
        switch (animationStateAt9C_)
        {
        case 0:
            if (WrapSubtract(weightBeforeAtA0_, animationCounterAtB8_) ==
                weightAfterAtA4_)
            {
                animationStateAt9C_ = 1;
                continue;
            }
            animationCounterAtB8_ = WrapAdd(animationCounterAtB8_, 1);
            ComposeMode00At5279C0(
                stressBeforeAtA8_,
                WrapSubtract(weightBeforeAtA0_, animationCounterAtB8_),
                true, false, true, true);
            animationDelayAtB4_ = 4;
            services_->EmitMappedSoundAt4BCD70(
                ProgramSelectionMainSoundAt4BCD70::WeightDecreaseAt61A670);
            return;
        case 1:
            if (weightBeforeAtA0_ != weightAfterAtA4_)
            {
                ComposeMode00At5279C0(
                    stressBeforeAtA8_, weightAfterAtA4_,
                    true, false, false, true);
                animationDelayAtB4_ = 20;
            }
            ++animationStateAt9C_;
            animationCounterAtB8_ = 0;
            return;
        case 2:
            if (WrapAdd(stressBeforeAtA8_, animationCounterAtB8_) ==
                stressAfterAtAC_)
            {
                animationStateAt9C_ = 3;
                continue;
            }
            animationCounterAtB8_ = WrapAdd(animationCounterAtB8_, 1);
            ComposeMode00At5279C0(
                WrapAdd(stressBeforeAtA8_, animationCounterAtB8_),
                weightAfterAtA4_, true, true, false, true);
            animationDelayAtB4_ = 10;
            services_->EmitMappedSoundAt4BCD70(
                ProgramSelectionMainSoundAt4BCD70::StressIncreaseAt61A674);
            return;
        case 3:
            ComposeMode00At5279C0(
                stressAfterAtAC_, weightAfterAtA4_,
                false, false, false, true);
            ++animationStateAt9C_;
            animationDelayAtB4_ = 20;
            return;
        case 4:
            ++phaseAt98_;
            return;
        default:
            return;
        }
    }
}

std::int32_t ProgramSelectionMainDialogObjectGraphRuntime::CloseAtVslot4C()
{
    if (mode_ == ProgramSelectionDialogMode::Mode00)
    {
        if (static_cast<std::uint32_t>(selectedCharacterAt8C_) >= 3u)
        {
            return static_cast<std::int32_t>(
                reinterpret_cast<std::uintptr_t>(this));
        }

        const std::size_t selected =
            static_cast<std::size_t>(selectedCharacterAt8C_);
        static_cast<void>(values_->WriteNamedValueAt4C8220(
            CharacterKey(selected, kWeight), weightAfterAtA4_));
        return values_->WriteNamedValueAt4C8220(
            CharacterKey(selected, kStress), stressAfterAtAC_);
    }
    if (mode_ == ProgramSelectionDialogMode::Mode01)
    {
        // 0x508CB0。
        return 1;
    }
    throw std::logic_error("ProgramSelection Main mode is invalid");
}

std::int32_t ProgramSelectionMainDialogObjectGraphRuntime::ResultAt80() const noexcept
{
    return resultAt80_;
}

std::int32_t ProgramSelectionMainDialogObjectGraphRuntime::ResultAt90() const noexcept
{
    return resultAt90_;
}

std::int32_t
ProgramSelectionMainDialogObjectGraphRuntime::SelectedCharacterAt8C() const noexcept
{
    return selectedCharacterAt8C_;
}

void ProgramSelectionMainDialogObjectGraphRuntime::WriteSelectedTrainingAtBC(
    const std::int32_t value) noexcept
{
    selectedTrainingAtBC_ = value;
}

std::int32_t
ProgramSelectionMainDialogObjectGraphRuntime::ResolveTrainingRecordAt50B470(
    const std::int32_t trainingChoice) const
{
    if (trainingChoice >= 3 ||
        static_cast<std::uint32_t>(selectedCharacterAt8C_) >= 3u)
    {
        return 27;
    }

    const std::int32_t trainingLevel = values_->ReadNamedValueAt4C73A0(
        CharacterKey(
            static_cast<std::size_t>(selectedCharacterAt8C_), kTraining));
    if (trainingLevel >= 3)
        return 27;

    // 原函数由两次 LEA 完成 32 位回绕运算，再以 unsigned 与 27 比较。
    const std::uint32_t record =
        static_cast<std::uint32_t>(selectedCharacterAt8C_) * 9u +
        static_cast<std::uint32_t>(trainingChoice) * 3u +
        static_cast<std::uint32_t>(trainingLevel);
    return record < kTrainingRecords.size()
        ? static_cast<std::int32_t>(record)
        : 27;
}

std::int32_t
ProgramSelectionMainDialogObjectGraphRuntime::CalculateTrainingAt50BCC0()
{
    if (selectedTrainingAtBC_ >= 3 ||
        static_cast<std::uint32_t>(selectedCharacterAt8C_) >= 3u)
    {
        return 0;
    }

    const std::size_t selected =
        static_cast<std::size_t>(selectedCharacterAt8C_);
    weightBeforeAtA0_ = values_->ReadNamedValueAt4C73A0(
        CharacterKey(selected, kWeight));
    stressBeforeAtA8_ = values_->ReadNamedValueAt4C73A0(
        CharacterKey(selected, kStress));

    const std::int32_t recordIndex =
        ResolveTrainingRecordAt50B470(selectedTrainingAtBC_);
    if (static_cast<std::uint32_t>(recordIndex) >= kTrainingRecords.size())
        return 0;
    const TrainingRecord& record =
        kTrainingRecords[static_cast<std::size_t>(recordIndex)];

    weightAfterAtA4_ = WrapSubtract(
        weightBeforeAtA0_, record.weightDecrease);
    if (weightAfterAtA4_ < 0)
        weightAfterAtA4_ = 0;

    stressAfterAtAC_ = WrapAdd(
        stressBeforeAtA8_, record.stressIncrease);
    if (static_cast<std::uint32_t>(stressAfterAtAC_) >= 4u)
        stressAfterAtAC_ = 4;

    const std::string intimacyKey = CharacterKey(selected, kIntimacy);
    std::int32_t intimacy = WrapAdd(
        values_->ReadNamedValueAt4C73A0(intimacyKey),
        record.intimacyIncrease);
    if (intimacy >= 100)
        intimacy = 100;
    static_cast<void>(
        values_->WriteNamedValueAt4C8220(intimacyKey, intimacy));

    const std::string dependenceKey = CharacterKey(selected, kDependence);
    std::int32_t dependence = WrapAdd(
        values_->ReadNamedValueAt4C73A0(dependenceKey),
        record.dependenceIncrease);
    if (dependence >= 100)
        dependence = 100;
    static_cast<void>(
        values_->WriteNamedValueAt4C8220(dependenceKey, dependence));

    trainingLevelAtB0_ = values_->ReadNamedValueAt4C73A0(
        CharacterKey(selected, kTraining));
    return 1;
}

std::int32_t
ProgramSelectionMainDialogObjectGraphRuntime::WeightBeforeAtA0() const noexcept
{
    return weightBeforeAtA0_;
}

std::int32_t
ProgramSelectionMainDialogObjectGraphRuntime::WeightAfterAtA4() const noexcept
{
    return weightAfterAtA4_;
}

std::int32_t
ProgramSelectionMainDialogObjectGraphRuntime::StressBeforeAtA8() const noexcept
{
    return stressBeforeAtA8_;
}

std::int32_t
ProgramSelectionMainDialogObjectGraphRuntime::StressAfterAtAC() const noexcept
{
    return stressAfterAtAC_;
}

std::int32_t
ProgramSelectionMainDialogObjectGraphRuntime::TrainingLevelAtB0() const noexcept
{
    return trainingLevelAtB0_;
}

std::int32_t
ProgramSelectionMainDialogObjectGraphRuntime::SelectedTrainingAtBC() const noexcept
{
    return selectedTrainingAtBC_;
}

std::int32_t ProgramSelectionMainDialogObjectGraphRuntime::PhaseAt98() const noexcept
{
    return phaseAt98_;
}

std::int32_t
ProgramSelectionMainDialogObjectGraphRuntime::AnimationStateAt9C() const noexcept
{
    return animationStateAt9C_;
}

std::int32_t
ProgramSelectionMainDialogObjectGraphRuntime::AnimationDelayAtB4() const noexcept
{
    return animationDelayAtB4_;
}

std::int32_t
ProgramSelectionMainDialogObjectGraphRuntime::AnimationCounterAtB8() const noexcept
{
    return animationCounterAtB8_;
}

void ProgramSelectionMainDialogObjectGraphRuntime::RefreshMode00At5009C0(
    const std::int32_t interactive)
{
    if (mode_ != ProgramSelectionDialogMode::Mode00)
    {
        throw std::logic_error(
            "ProgramSelection Main 0x5009C0 reached a non-mode-0 object");
    }
    if (selectedCharacterAt8C_ < 0 || selectedCharacterAt8C_ >= 3)
    {
        throw std::out_of_range(
            "ProgramSelection Main selected character is outside 0..2");
    }

    // 0x608BA0：每行依次为角色底图、动态状态图、九张训练层和角色按钮。
    static constexpr std::array<std::array<std::int32_t, 12>, 3> childRows{{
        {{2, 6, 9, 10, 11, 12, 13, 14, 15, 16, 17, 40}},
        {{3, 7, 18, 19, 20, 21, 22, 23, 24, 25, 26, 41}},
        {{4, 8, 27, 28, 29, 30, 31, 32, 33, 34, 35, 42}},
    }};

    for (const auto& child : children_)
        child->WriteAtVslot4C(0);

    const auto& selectedRow =
        childRows[static_cast<std::size_t>(selectedCharacterAt8C_)];
    ChildByEventValue(selectedRow[0])->WriteAtVslot4C(1);
    ChildByEventValue(selectedRow[1])->WriteAtVslot4C(1);

    for (std::int32_t character = 0; character < 3; ++character)
    {
        auto button = ChildByEventValue(40 + character);
        button->WriteAtVslot4C(1);
        button->WriteAtVslot44(
            interactive != 0 && character != selectedCharacterAt8C_);
    }

    if (interactive != 0)
    {
        const std::int32_t stress = values_->ReadNamedValueAt4C73A0(
            CharacterKey(static_cast<std::size_t>(selectedCharacterAt8C_), kStress));
        const bool stressBelowFour =
            static_cast<std::uint32_t>(stress) < 4u;

        ChildByEventValue(43)->WriteAtVslot4C(1);
        for (const std::int32_t event : {44, 45})
        {
            auto child = ChildByEventValue(event);
            child->WriteAtVslot4C(1);
            child->WriteAtVslot44(stressBelowFour);
        }

        const std::int32_t eventType = values_->ReadNamedValueAt4C73A0(
            CharacterKey(
                static_cast<std::size_t>(selectedCharacterAt8C_), kEventType));
        const bool specialEventAvailable = !stressBelowFour &&
            static_cast<std::uint32_t>(eventType) <= 2u;
        auto specialButton = ChildByEventValue(46);
        specialButton->WriteAtVslot4C(1);
        specialButton->WriteAtVslot44(specialEventAvailable);
        if (specialEventAvailable)
            ChildByEventValue(36 + eventType)->WriteAtVslot4C(1);
    }

    bool travelAvailable = false;
    for (std::size_t character = 0; character < 3u; ++character)
    {
        if (values_->ReadNamedValueAt4C73A0(
                CharacterKey(character, kRoute)) != 100)
        {
            continue;
        }
        if (values_->ReadNamedValueAt4C73A0(
                CharacterKey(character, kRemainingCount)) != 0)
        {
            continue;
        }
        if (values_->ReadNamedValueAt4C73A0(
                CharacterKey(character, kIntimacy)) >= 50 ||
            values_->ReadNamedValueAt4C73A0(
                CharacterKey(character, kDependence)) >= 50)
        {
            travelAvailable = true;
            break;
        }
    }
    auto travelButton = ChildByEventValue(47);
    travelButton->WriteAtVslot4C(1);
    travelButton->WriteAtVslot44(interactive != 0 && travelAvailable);

    const std::size_t selected =
        static_cast<std::size_t>(selectedCharacterAt8C_);
    if (values_->ReadNamedValueAt4C73A0(CharacterKey(selected, kRoute)) == 100 &&
        values_->ReadNamedValueAt4C73A0(CharacterKey(selected, kWeight)) == 0 &&
        (values_->ReadNamedValueAt4C73A0(CharacterKey(selected, kIntimacy)) >= 50 ||
         values_->ReadNamedValueAt4C73A0(CharacterKey(selected, kDependence)) >= 50))
    {
        ChildByEventValue(5)->WriteAtVslot4C(1);
    }

    // 0x5009C0 最后查询可选 ID 39；样本 18 的本对象图没有该 child。
    if (const auto optional = FindChildByEventValue(39))
        optional->WriteAtVslot4C(1);
}

void ProgramSelectionMainDialogObjectGraphRuntime::ApplyMode01AvailabilityAt501880()
{
    struct AvailabilityEntry final
    {
        const char* keyCp932;
        std::int32_t buttonEvent;
        std::int32_t firstLayerEvent;
        std::int32_t layerCount;
    };
    static constexpr std::array<AvailabilityEntry, 5> entries{{
        {"\x90\x53\x8D\xD8\x81\x40\x83\x43\x83\x78\x83\x93\x83\x67\x83\x5E\x83\x43\x83\x76", 17, 3, 3},
        {"\x90\xE1\x88\xA4\x81\x40\x83\x43\x83\x78\x83\x93\x83\x67\x83\x5E\x83\x43\x83\x76", 18, 6, 3},
        {"\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67\x81\x40\x83\x43\x83\x78\x83\x93\x83\x67\x83\x5E\x83\x43\x83\x76", 19, 9, 3},
        {"\x89\xC4\x91\xE3\x8E\x71\x81\x40\x83\x43\x83\x78\x83\x93\x83\x67\x83\x5E\x83\x43\x83\x76", 20, 12, 3},
        {"\x83\x6E\x81\x5B\x83\x8C\x83\x80\x81\x40\x83\x43\x83\x78\x83\x93\x83\x67\x83\x5E\x83\x43\x83\x76", 21, 15, 2},
    }};

    for (const auto& entry : entries)
    {
        const std::int32_t value =
            values_->ReadNamedValueAt4C73A0(entry.keyCp932);
        ChildByEventValue(entry.buttonEvent)->WriteAtVslot44(value >= 0);
        for (std::int32_t index = 0; index < entry.layerCount; ++index)
        {
            ChildByEventValue(entry.firstLayerEvent + index)->WriteAtVslot44(
                value == index);
        }
    }
}

CgOperationChildControlRef
ProgramSelectionMainDialogObjectGraphRuntime::FindChildByEventValue(
    const std::int32_t eventValue) const noexcept
{
    const auto it = std::ranges::find_if(
        children_, [eventValue](const auto& child) {
            return child && child->EventValueAt04() == eventValue;
        });
    return it == children_.end() ? CgOperationChildControlRef{} : *it;
}

CgOperationChildControlRef
ProgramSelectionMainDialogObjectGraphRuntime::ChildByEventValue(
    const std::int32_t eventValue) const
{
    auto child = FindChildByEventValue(eventValue);
    if (!child)
        throw std::out_of_range("ProgramSelection Main child is not owned");
    return child;
}
}
