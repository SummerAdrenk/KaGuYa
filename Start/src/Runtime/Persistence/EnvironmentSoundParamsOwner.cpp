#include <Runtime/EnvironmentSoundParamsOwner.h>

namespace KaGuYa::Start::Runtime
{
EnvironmentSoundParamsOwner::EnvironmentSoundParamsOwner(const YtlReferenceSlotRuntime& source)
    : reference_(source), runtime_(EnvironmentSoundParamsRuntime::BorrowConstructedTag{}, reference_.Get())
{
}

EnvironmentSoundParamsOwner::EnvironmentSoundParamsOwner(const ParamsGameSystem& gameSystem,
    ISettingTagErrorSource& errors)
    : reference_(gameSystem, errors), runtime_(EnvironmentSoundParamsRuntime::BorrowConstructedTag{}, reference_.Get())
{
}

EnvironmentSoundParamsOwner::EnvironmentSoundParamsOwner(ISoundParamsSettingsSource& source,
    ISettingTagErrorSource& errors)
    : reference_(source, errors), runtime_(EnvironmentSoundParamsRuntime::BorrowConstructedTag{}, reference_.Get())
{
}

EnvironmentSoundParamsArchive& EnvironmentSoundParamsOwner::Archive() noexcept
{
    return reference_.Get();
}

const EnvironmentSoundParamsArchive& EnvironmentSoundParamsOwner::Archive() const noexcept
{
    return reference_.Get();
}

EnvironmentSoundParamsRuntime& EnvironmentSoundParamsOwner::Runtime() noexcept
{
    return runtime_;
}

const EnvironmentSoundParamsRuntime& EnvironmentSoundParamsOwner::Runtime() const noexcept
{
    return runtime_;
}
}
