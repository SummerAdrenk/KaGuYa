#include <Runtime/EnvironmentSceneRuntime.h>

#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/ParamsRuntimeRoot.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
EnvironmentSceneRuntime::EnvironmentSceneRuntime(ParamsRuntimeRootOwner& params,
    EnvironmentStartupRuntime& environment, decltype(&::MessageBoxA) messageBox) noexcept
    : params_(params), environment_(environment), messageBox_(messageBox)
{
}

std::size_t EnvironmentSceneRuntime::CurrentSceneLabelCount()
{
    // 4D0E3D：这里只取当前数量，不为随后取 Name 固定旧 child。
    const auto& labels = params_.GetOrCreateAt418480().SceneLabels();
    // 现代缺 child 保护，不以空表或原 null 标签异常代替。
    if (!labels)
        throw std::logic_error("SCENE Params SceneLabels child is missing");
    return labels->size();
}

const char* EnvironmentSceneRuntime::CurrentSceneLabelName(const std::size_t index)
{
    // 4D0E7D/4D0ECD：独立晚取当前 owner/项 Ref，直接借 CString chars，不复制或增持。
    const auto& labels = params_.GetOrCreateAt418480().SceneLabels();
    if (!labels)
        throw std::logic_error("SCENE Params SceneLabels child is missing");
    // 两次 lookup 间表可能改变；at 的越界是现代保护，不转为 CmdScene 或 null 异常。
    return labels->at(index).nameAt00.GetString();
}

const ParamsGameSystem& EnvironmentSceneRuntime::CurrentGameSystem()
{
    // 4D6090/4D6164 分别调用；不缓存循环上限或跨轮持有 GameSystem。
    const auto& gameSystem = params_.GetOrCreateAt418480().GameSystem();
    if (!gameSystem)
        throw std::logic_error("SCENE Params GameSystem child is missing");
    return *gameSystem;
}

SecretScArchive* EnvironmentSceneRuntime::CurrentSecretSc()
{
    // 4D0ED8 -> 417990：4179B1 越界或 4179BE base=0，均到 4179E6 返回 null。
    return static_cast<SecretScArchive*>(environment_.Archives().CaptureArchiveAt4180C0(4));
}

void EnvironmentSceneRuntime::ReportSceneLabelIndexOutOfRange(std::uint16_t)
{
    // 4D0EE8..4D0EFA：错误分支才先借 +10 标题 raw，再取 +2C HWND；无 CString 保活。
    const auto& errorEnvironment = environment_.CaptureAt4180C0();
    const char* const caption = errorEnvironment.titleAt10.GetString();
    const HWND window = reinterpret_cast<HWND>(errorEnvironment.windowAt2C);
    static_cast<void>(messageBox_(window, "CmdScene", caption, MB_ICONERROR));
}
}
