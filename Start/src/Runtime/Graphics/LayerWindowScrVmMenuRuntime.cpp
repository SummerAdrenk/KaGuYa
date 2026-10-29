#include <Runtime/LayerWindowScrVmMenuRuntime.h>

#include <exception>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
template <typename Callback>
decltype(auto) TranslateMenuFailure(Callback&& callback)
{
    try
    {
        return std::forward<Callback>(callback)();
    }
    catch (const ScrVmMenuCommandException&)
    {
        throw;
    }
    catch (const std::exception& error)
    {
        throw ScrVmMenuCommandException(error.what());
    }
}
}

LayerWindowScrVmMenuRuntime::LayerWindowScrVmMenuRuntime(
    ScrScriptRuntimeState& scriptRuntime,
    const IScrResourceNameSource& resourceNames,
    LayerWindowScrResultApplier& resultApplier,
    LayerWindowRuntime& layerWindow,
    IScrVmSoundRuntimeBoundary& soundRuntime,
    IProgramRegistrySlot7& registrySlot7,
    IMenuUnreadChoiceState& unreadChoiceState) noexcept
    : scriptRuntime_(&scriptRuntime),
      resultApplier_(&resultApplier),
      layerWindow_(&layerWindow),
      registrySlot7_(&registrySlot7),
      unreadChoiceState_(&unreadChoiceState),
      mappedSound_(resourceNames, soundRuntime)
{
}

std::size_t LayerWindowScrVmMenuRuntime::FirstStringGroupCountAt50() const
{
    return mappedSound_.FirstCount();
}

std::size_t LayerWindowScrVmMenuRuntime::SecondStringGroupCountAt48() const
{
    return mappedSound_.SecondCount();
}

void LayerWindowScrVmMenuRuntime::ApplyResultLayerList(
    const std::int32_t listIndex)
{
    const ScrContainer* const script = scriptRuntime_->LoadedScript();
    if (!script)
        throw ScrVmMenuCommandException("SCR menu has no loaded script container");

    TranslateMenuFailure([&] { resultApplier_->Apply(*script, listIndex); });
}

void LayerWindowScrVmMenuRuntime::ApplyFirstStringSlot(
    const std::int32_t value,
    const std::size_t groupIndex)
{
    TranslateMenuFailure([&] { mappedSound_.ApplyFirst(value, groupIndex); });
}

void LayerWindowScrVmMenuRuntime::ApplySecondStringSlot(
    const std::int32_t value,
    const std::size_t groupIndex)
{
    TranslateMenuFailure([&] { mappedSound_.ApplySecond(value, groupIndex); });
}

void LayerWindowScrVmMenuRuntime::SelectEffect(const std::int32_t selector)
{
    TranslateMenuFailure(
        [&] { static_cast<void>(layerWindow_->Effects().Select(selector)); });
}

std::uint16_t LayerWindowScrVmMenuRuntime::ReadSelectionWordAt10() const
{
    // GlobalParams/registry slot 7 的 WORD 表起于 +0x0C，因此 +0x10 为索引 2。
    return TranslateMenuFailure(
        [&] { return registrySlot7_->ReadWordAt0C(2u); });
}

std::uint16_t LayerWindowScrVmMenuRuntime::ReadSelectionWordAt12() const
{
    // 同一 WORD 表的 +0x12 为索引 3。
    return TranslateMenuFailure(
        [&] { return registrySlot7_->ReadWordAt0C(3u); });
}

std::optional<bool> LayerWindowScrVmMenuRuntime::ReadChoiceFlag(
    const std::int32_t operand) const
{
    return TranslateMenuFailure(
        [&] { return unreadChoiceState_->ReadChoiceFlag(operand); });
}

void LayerWindowScrVmMenuRuntime::SetChoiceFlag(const std::int32_t operand)
{
    TranslateMenuFailure([&] { unreadChoiceState_->SetChoiceFlag(operand); });
}

void LayerWindowScrVmMenuRuntime::PersistUnreadFlags()
{
    TranslateMenuFailure([&] { unreadChoiceState_->PersistUnreadFlags(); });
}
}
