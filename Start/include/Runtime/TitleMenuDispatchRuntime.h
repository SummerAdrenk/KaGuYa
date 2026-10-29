#pragma once

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class ITitleDialogRuntime
{
public:
    virtual ~ITitleDialogRuntime() = default;

    // TitleDialog 虚表 5BFE38+0x44 -> 50FB70；4BAE48 是调用点。
    virtual void InvokeVslot44() = 0;

    // TitleDialog 虚表 +0x48（530A40）：非零表示对话框仍在运行。
    [[nodiscard]] virtual bool Run() = 0;

    // TitleDialog 虚表 +0x50（50AC60）：返回最终菜单结果。
    [[nodiscard]] virtual std::uint32_t SelectionResult() const = 0;
};

class ITitleMenuRuntimeServices
{
public:
    virtual ~ITitleMenuRuntimeServices() = default;

    [[nodiscard]] virtual bool LoadScript(
        const char* scriptNameCp932,
        std::int32_t argument) = 0;
    virtual void ClearSceneController() = 0;
    virtual void QueueScene(const char* sceneNameCp932) = 0;
    virtual void SetRootByteAt218(std::uint8_t value) = 0;
    virtual void SetRootByteAt219(std::uint8_t value) = 0;
};

// TitleScene::CTitle 更新函数 4BADA0 中 4BB277 之后的最终菜单分发段。
// 该类刻意不包含尚未闭环的 CTitle 中间全局状态分支。
class TitleMenuDispatchRuntime final
{
public:
    TitleMenuDispatchRuntime(
        ITitleDialogRuntime& dialog,
        ITitleMenuRuntimeServices& services) noexcept;

    void Update();

private:
    ITitleDialogRuntime* dialog_ = nullptr;
    ITitleMenuRuntimeServices* services_ = nullptr;
};
}
