#pragma once

#include <Windows.h>
#include <exception>
#include <Runtime/GameNamePolicy.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class EnvironmentRootOwner;
class ParamsRuntimeRootOwner;

enum class GameResourceNamePromptStatus { Accepted, Cancelled, Error };
enum class GameResourceNamePromptContext { Title, LegacySettings };

struct GameResourceNamePromptResult final
{
    GameResourceNamePromptStatus status = GameResourceNamePromptStatus::Error;
    DWORD win32Error = ERROR_SUCCESS;
    std::exception_ptr exception;
};

struct GameResourceNamePromptDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    ParamsRuntimeRootOwner& params;
    HINSTANCE resourceInstance;
    GameNameByteLimits limits;
    GameResourceNamePromptContext context = GameResourceNamePromptContext::Title;
};

// 在已初始化主窗口的UI线程调用；模态期间借用的owner须保持存活，不持根锁。
// 使用当前游戏RT_DIALOG/112，是否提示由调用者策略决定；字节限制来自游戏数据。
// LegacySettings隐藏1200，接受时清skip；不改变Title现有勾选提交规则。
// 只有Accepted提交内存SaveData，不在此写盘；错误不能被当作取消。
[[nodiscard]] GameResourceNamePromptResult ShowGameResourceNamePrompt(
    const GameResourceNamePromptDependencies& dependencies) noexcept;
}
