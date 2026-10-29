#include <Runtime/GeneralConfirmResourceProfile.h>
#include <Runtime/GameBuildProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const GeneralConfirmResourceProfile& CurrentGeneralConfirmResourceProfile()
{
    const auto* profile = CompiledGameProfile().generalConfirm;
    if (!profile) throw std::logic_error("General confirmation profile is not bound");
    return *profile;
}
const GeneralConfirmModeRow& FindGeneralConfirmMode(const GeneralConfirmResourceProfile& profile, std::int32_t mode)
{
    for (const auto& row : profile.modes)
        if (row.mode == mode) return row;
    // 不能按表长度接受5-8或残留mode9，也不能把非法mode钳成退出确认。
    throw std::out_of_range("General confirmation mode is not provided by this profile");
}
std::optional<std::int32_t> GeneralConfirmResultForEvent(const GeneralConfirmResourceProfile& profile, std::int32_t event)
{
    for (const auto& row : profile.eventResults)
        if (row.event == event) return row.result;
    // 01-13原Poll未知事件throw 1；14-18原ReadEvents返回0并阻断后续flag。
    if (profile.unknownEvent == GeneralConfirmUnknownEvent::ThrowIntOne) throw 1;
    return std::nullopt;
}
}
