#pragma once

#include <Runtime/MessageWindowLayerRuntime.h>

namespace KaGuYa::Start::Runtime
{
class ScrVmValueTable;
class ScrVmExecutableProfile;
class SaveDataEnvironmentArchive;
class UnSaveDataRuntime;
class EnvironmentSoundParamsRuntime;
class EnvironmentStartupRuntime;
class EnvironmentPostWindowStartupRuntime;

// 487030 的实际状态源：借用同一 CParams view、命名表、save/unsave 和声音 owner。
// 419290 -> 5B2730+60 -> 46C830，不使用临时 gate bool 或构造时快照。
// 所有引用必须长于本 adapter；soundParams 必须属于传入 save 的同一声音配置 owner。
// +70/+40 每次访问真实 CDrawManager slot 3；缺槽、空槽或类型错误明确失败。
// 不以 ScrVmTextState、TextDisplayScene 或 Graphics 文本矩形冒充该对象。
class MessageWindowLayerStateRuntime final : public IMessageWindowLayerState
{
public:
    MessageWindowLayerStateRuntime(
        const ScrVmValueTable& activeParams,
        const ScrVmExecutableProfile& executableProfile,
        const SaveDataEnvironmentArchive& save,
        const UnSaveDataRuntime& unsave,
        const EnvironmentSoundParamsRuntime& soundParams,
        DrawLayersView layers) noexcept;
    MessageWindowLayerStateRuntime(const ScrVmValueTable& activeParams,
        const ScrVmExecutableProfile& executableProfile,
        const EnvironmentStartupRuntime& environment,
        const EnvironmentPostWindowStartupRuntime& root) noexcept;

    [[nodiscard]] std::int32_t ActiveScriptField60() const override;
    [[nodiscard]] bool MessageWindowAllowedAt419290() const override;
    [[nodiscard]] std::int32_t EvaluateMessWinTypeAt4C73A0() override;
    [[nodiscard]] bool TextWindowHasNameAt70() const override;
    [[nodiscard]] RECT TextWindowNameRectAt40() const override;

private:
    const SaveDataEnvironmentArchive& Save() const;
    const UnSaveDataRuntime& UnSave() const;
    EnvironmentSoundParamsRuntime SoundParams() const;
    DrawLayersView Layers() const;
    const ScrVmValueTable& activeParams_;
    const ScrVmExecutableProfile& executableProfile_;
    const EnvironmentStartupRuntime* environment_ = nullptr;
    const EnvironmentPostWindowStartupRuntime* root_ = nullptr;
    const SaveDataEnvironmentArchive* save_ = nullptr;
    const UnSaveDataRuntime* unsave_ = nullptr;
    const EnvironmentSoundParamsRuntime* soundParams_ = nullptr;
    DrawLayersView layers_;
};
}
