#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include <Runtime/SceneControllerNative.h>

namespace KaGuYa::Start::Runtime
{
struct SceneControllerNativeBody28h;
class ISceneTask
{
public:
    virtual ~ISceneTask() = default;
    virtual void Run(std::int32_t argument) = 0;
};

class ISceneFactory
{
public:
    virtual ~ISceneFactory() = default;
    [[nodiscard]] virtual std::shared_ptr<ISceneTask> Create(
        const char* sceneNameCp932) = 0;
};

// TSceneController<const char*>（48E100）的两个容器和消费顺序。
class SceneControllerRuntime final
{
public:
    // 调度器借原28h虚槽时绑定已有factory桥接，不创建另一任务/控制器。
    class NativeCallScope final
    {
    public:
        explicit NativeCallScope(SceneControllerRuntime&) noexcept;
        ~NativeCallScope() noexcept;
        NativeCallScope(const NativeCallScope&)=delete;
        NativeCallScope& operator=(const NativeCallScope&)=delete;
    private:
        void* previous_;
        SceneControllerNativeScope controller_;
    };
    explicit SceneControllerRuntime(ISceneFactory& factory);
    ~SceneControllerRuntime() noexcept;
    SceneControllerRuntime(const SceneControllerRuntime&)=delete;
    SceneControllerRuntime& operator=(const SceneControllerRuntime&)=delete;

    // 47F920：先弹出一个 active 场景，再把名称压入 pending 尾端。
    void Queue(const char* sceneNameCp932);

    // 4BADA0 的标题模态分支：立即创建并追加到 active，不触碰 pending，
    // 也不弹出现有 active 项。该路径与 47F920 的 Queue 不是同一语义。
    void PushActiveScene(const char* sceneNameCp932);

    // 4C1760：先释放一个 active 尾项，再弹出并立即构造一个 pending 尾项。
    // 不调用新任务的 Run；工厂异常不恢复已经弹出的名称。
    void PopThenActivatePendingAt4C1760();

    // 48E100：active 优先；否则从 pending 尾端创建一个场景。两者同时为空时返回假。
    [[nodiscard]] bool Run(std::int32_t argument);

    // 4C1460：先清空 active，再把 pending end 恢复到 begin。
    void Clear() noexcept;

    [[nodiscard]] std::size_t PendingCount() const noexcept;
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
    // 当前原生正文借用；不增持，不保存另一份容器状态。
    [[nodiscard]] const SceneControllerNativeBody28h& NativeBody() const noexcept;
    [[nodiscard]] TaskNativeBody4h* BorrowNativeTask() noexcept;

private:
    // 唯一业务状态在原28h与两个原vector；现有task/factory桥接不进入正文。
    struct NativeStorage;
    std::unique_ptr<NativeStorage> native_;
};
}
