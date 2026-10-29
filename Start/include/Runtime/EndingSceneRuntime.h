#pragma once

#include <Runtime/SceneControllerRuntime.h>
#include <Runtime/ParamsDemoLookupRuntime.h>
#include <Runtime/RuntimeCString.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class IEndingDemoSequenceRuntime
{
public:
    virtual ~IEndingDemoSequenceRuntime() = default;

    // 49DA58..49DACE：名称来自 root 当前对象 +0x3C，查询结果写入 CDemoSeq+0x98。
    virtual void BindDemoDataAt4C2220(const YtlReferenceSlotRuntime& source) = 0;

    // CDemoSeq::Run(4BDAE0)：参数来自 CEnding+0x14 的完成请求 byte。
    [[nodiscard]] virtual std::int32_t Run(
        std::uint8_t completionRequested) = 0;

    // 4C38A0：状态 1 的过渡推进；仅非零返回值结束该状态。
    [[nodiscard]] virtual bool AdvanceTransition() = 0;
    // 4A7690 -> 4C3D50：更新可见精灵的运动/动画，再重绘 dirty rect。
    virtual void UpdateSpritesAndRedraw() = 0;
};

class IEndingSceneRuntimeServices
{
public:
    virtual ~IEndingSceneRuntimeServices() = default;

    // 49DA53..49DA90：读取 root 当前对象 +0x3C 所持字符串。
    [[nodiscard]] virtual const RuntimeCString* CaptureDemoNameSlotAt49DA53() = 0;

    // 49DB2B..49DBDC：先执行4384D0；再先取Env2、后取root，以旧Env2+30调用413270。
    virtual void StopRootSlot24Sounds() = 0;
    virtual void StopRootSlot24SoundsUsingDescriptor2ListAt30() = 0;

    // 构造写 0/1，正常结束写 1；名称保留已确认的字段偏移，不推测业务含义。
    virtual void WriteDescriptor2DwordAt04(std::uint32_t value) = 0;
    virtual void WriteDescriptor2DwordAt14(std::uint32_t value) = 0;

    // 4B5FC0 的三个短路输入条件由输入和全局状态共同计算。
    [[nodiscard]] virtual bool ShouldRequestCompletion() = 0;

    // CDemoSeq 返回 -1 时，通过 47F920 排入固定“タイトル”场景。
    virtual void QueueScene(const char* sceneNameCp932) = 0;

    // 4B5E30 的收尾顺序：ClearScreen(0)、InvalidatedRect(0)、4C1760。
    virtual void ClearScreen(std::int32_t value) = 0;
    virtual void InvalidateScreenRect(const void* rect) = 0;
    virtual void AdvanceActiveSceneFromPending() = 0;
};

// DemoScene::CEnding（49DA10、4B5FC0、4B5E30）的可证明行为边界。
class EndingSceneRuntime final : public ISceneTask
{
public:
    EndingSceneRuntime(
        IEndingDemoSequenceRuntime& sequence,
        IEndingSceneRuntimeServices& services, IDemoParamsSource& params);

    // ITask 包装 4C3630 忽略传入参数，并固定执行 Update + 空 Draw。
    void Run(std::int32_t argument) override;

    void SetInputCompletionCheckEnabled(bool enabled) noexcept;

    [[nodiscard]] std::uint32_t State() const noexcept;
    [[nodiscard]] bool CompletionRequested() const noexcept;
    [[nodiscard]] bool InputCompletionCheckEnabled() const noexcept;

private:
    void Update();
    void Finish();

    IEndingDemoSequenceRuntime* sequence_ = nullptr;
    IEndingSceneRuntimeServices* services_ = nullptr;
    std::uint32_t state_ = 0u;
    bool completionRequested_ = false;
    bool inputCompletionCheckEnabled_ = true;
};
}
