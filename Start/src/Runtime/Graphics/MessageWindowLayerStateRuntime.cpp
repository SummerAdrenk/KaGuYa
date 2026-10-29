#include <Runtime/MessageWindowLayerStateRuntime.h>

#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/ScrVmExecutableProfile.h>
#include <Runtime/ScrVmValueTable.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/EnvironmentPostWindowStartupRuntime.h>
#include <Runtime/DrawLayersView.h>

#include <cstddef>
#include <locale.h>
#include <mbstring.h>
#include <stdexcept>
#include <string>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
namespace
{
bool HasAllowedVoiceAt46C830(const EnvironmentSoundParamsRuntime& owner,
    const UnSaveDataStringArrayReference& names)
{
    return owner.IsVoiceArrayAllowedAt46C830(names);
}
}

MessageWindowLayerStateRuntime::MessageWindowLayerStateRuntime(
    const ScrVmValueTable& activeParams, const ScrVmExecutableProfile& executableProfile,
    const SaveDataEnvironmentArchive& save, const UnSaveDataRuntime& unsave,
    const EnvironmentSoundParamsRuntime& soundParams, DrawLayersView layers) noexcept
    : activeParams_(activeParams), executableProfile_(executableProfile), save_(&save),
      unsave_(&unsave), soundParams_(&soundParams), layers_(layers)
{
}

MessageWindowLayerStateRuntime::MessageWindowLayerStateRuntime(
    const ScrVmValueTable& activeParams, const ScrVmExecutableProfile& executableProfile,
    const EnvironmentStartupRuntime& environment, const EnvironmentPostWindowStartupRuntime& root) noexcept
    : activeParams_(activeParams), executableProfile_(executableProfile), environment_(&environment), root_(&root)
{
}

const SaveDataEnvironmentArchive& MessageWindowLayerStateRuntime::Save() const
{
    if (!environment_) return *save_;
    const auto* body = static_cast<SaveDataEnvironmentArchive*>(environment_->Archives().CaptureArchiveAt4180C0(1));
    if (!body) throw std::logic_error("CMesWin SaveData slot is empty (safe boundary)");
    return *body;
}

const UnSaveDataRuntime& MessageWindowLayerStateRuntime::UnSave() const
{
    if (!environment_) return *unsave_;
    const auto* body = static_cast<UnSaveDataRuntime*>(environment_->Archives().CaptureArchiveAt4180C0(2));
    if (!body) throw std::logic_error("CMesWin UnSaveData slot is empty (safe boundary)");
    return *body;
}

EnvironmentSoundParamsRuntime MessageWindowLayerStateRuntime::SoundParams() const
{
    if (!environment_) return *soundParams_;
    // 4192F7..419326重取当前slot1+1C，不使用SoundManager固定保活的旧SoundParams。
    return {EnvironmentSoundParamsRuntime::BorrowConstructedTag{}, Save().BorrowSoundParamsAt1C()};
}

DrawLayersView MessageWindowLayerStateRuntime::Layers() const
{
    if (!root_) return layers_;
    return root_->DrawLayersAt18();
}

std::int32_t MessageWindowLayerStateRuntime::ActiveScriptField60() const
{
    // 47BDB0 取得根 +1C0 的 CParams；与 4C73A0 使用同一当前 generation。
    return activeParams_.CurrentParamsGeneration().messageFormatIndexAt60;
}

bool MessageWindowLayerStateRuntime::MessageWindowAllowedAt419290() const
{
    // 419290 的三个阶段均短路；+38 是 bytesAt34[4]，+3C 检查指针而不是 Empty()。
    if ((Save().bytesAt34[4] & 0x10u) == 0) return true;
    if (!UnSave().ArrayReferenceAt3C().StorageAt3C()) return true;
    const auto& sound = SoundParams();
    // 41932A在取得SoundParams后再次取slot2；不沿用前一阶段的数组对象。
    return !HasAllowedVoiceAt46C830(sound, UnSave().ArrayReferenceAt3C());
}

std::int32_t MessageWindowLayerStateRuntime::EvaluateMessWinTypeAt4C73A0()
{
    // 487030:498020 -> 4C73A0。保留 -1/越界异常，不把缺名称变为样式 0。
    return activeParams_.ReadSigned(executableProfile_.ResolveNamedValue("mess_win_type"));
}

bool MessageWindowLayerStateRuntime::TextWindowHasNameAt70() const
{
    return Layers().TextAtSlotThree().HasNameAt70();
}

RECT MessageWindowLayerStateRuntime::TextWindowNameRectAt40() const
{
    return Layers().TextAtSlotThree().NameRectAt40();
}
}
