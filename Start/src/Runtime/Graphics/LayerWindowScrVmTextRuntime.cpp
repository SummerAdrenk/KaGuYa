#include <Runtime/LayerWindowScrVmTextRuntime.h>

#include <exception>

namespace KaGuYa::Start::Runtime
{
LayerWindowScrVmTextRuntime::LayerWindowScrVmTextRuntime(
    ScrScriptRuntimeState& scriptRuntime,
    const IScrResourceNameSource& resourceNames,
    LayerWindowScrResultApplier& resultApplier,
    LayerWindowRuntime& layerWindow,
    IScrVmSoundRuntimeBoundary& soundRuntime) noexcept
    : scriptRuntime_(&scriptRuntime),
      resultApplier_(&resultApplier),
      layerWindow_(&layerWindow),
      mappedSound_(resourceNames, soundRuntime)
{
}

std::size_t LayerWindowScrVmTextRuntime::FirstStringGroupCountAt50() const
{
    return mappedSound_.FirstCount();
}

std::size_t LayerWindowScrVmTextRuntime::SecondStringGroupCountAt48() const
{
    return mappedSound_.SecondCount();
}

void LayerWindowScrVmTextRuntime::ApplyResultLayerList(
    const std::int32_t listIndex)
{
    const ScrContainer* const script = scriptRuntime_->LoadedScript();
    if (!script)
    {
        throw ScrVmTextCommandException(
            "SCR text has no loaded script container");
    }

    try
    {
        resultApplier_->Apply(*script, listIndex);
    }
    catch (const std::exception& error)
    {
        throw ScrVmTextCommandException(error.what());
    }
}

void LayerWindowScrVmTextRuntime::ApplyFirstStringSlot(
    const std::int32_t value,
    const std::size_t groupIndex)
{
    try
    {
        mappedSound_.ApplyFirst(value, groupIndex);
    }
    catch (const ScrVmTextCommandException&)
    {
        throw;
    }
    catch (const std::exception& error)
    {
        throw ScrVmTextCommandException(error.what());
    }
}

void LayerWindowScrVmTextRuntime::ApplySecondStringSlot(
    const std::int32_t value,
    const std::size_t groupIndex)
{
    try
    {
        mappedSound_.ApplySecond(value, groupIndex);
    }
    catch (const ScrVmTextCommandException&)
    {
        throw;
    }
    catch (const std::exception& error)
    {
        throw ScrVmTextCommandException(error.what());
    }
}

void LayerWindowScrVmTextRuntime::SelectEffect(const std::int32_t selector)
{
    static_cast<void>(layerWindow_->Effects().Select(selector));
}

}
