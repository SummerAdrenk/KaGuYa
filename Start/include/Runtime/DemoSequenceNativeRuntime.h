#pragma once
#include <Runtime/DemoSequenceNative.h>
#include <Runtime/DemoSceneProductionRuntime.h>

namespace KaGuYa::Start::Runtime
{
class DemoSequenceNativeScope final
{
public:
    explicit DemoSequenceNativeScope(DemoSequenceNativeServices&) noexcept;
    ~DemoSequenceNativeScope() noexcept;
    DemoSequenceNativeScope(const DemoSequenceNativeScope&)=delete;
    DemoSequenceNativeScope& operator=(const DemoSequenceNativeScope&)=delete;
private:
    DemoSequenceNativeServices* previous_;
    GraphicsSurfaceNativeScope graphics_;
};
// 原CDemo+08与旧显式外观共用唯一Ref安装口；失败不额外回收已完成A4h。
void AdoptDemoSequenceNativeAt49B644(YtlReferenceSlotRuntime&,DemoSequenceNativeBodyA4h*,IEnvironmentRootMemory&);
// C++调用外观只拥有原Ref12，正文和资源全部位于唯一A4h；不保存现代备用播放器。
// 外观自身的现代分配域仍与原Scene18h不同，不能据此称Scene/Controller ABI已全迁。
class DemoSequenceNativeRuntime final : public ILogoDemoSequenceRuntime,public IEndingDemoSequenceRuntime
{
public:
    explicit DemoSequenceNativeRuntime(DemoSequenceNativeServices&);
    ~DemoSequenceNativeRuntime() noexcept override;
    void BindDemoDataAt4C2220(const YtlReferenceSlotRuntime&) override;
    std::int32_t Run(std::uint8_t) override;
    bool AdvanceTransition() override;
    void UpdateSpritesAndRedraw() override;
    DemoSequenceNativeBodyA4h* Body() const noexcept;
    YtlReferenceSlotRuntime& Reference() noexcept {return reference_;}
private:
    DemoSequenceNativeServices& services_;
    YtlReferenceSlotRuntime reference_;
};
class DemoSequenceNativeFactory final : public ILogoDemoSequenceFactory,public IEndingDemoSequenceFactory
{
public:
    explicit DemoSequenceNativeFactory(DemoSequenceNativeServices& services) noexcept:services_(services){}
    std::unique_ptr<ILogoDemoSequenceRuntime> CreateLogoDemoSequence() override;
    std::unique_ptr<IEndingDemoSequenceRuntime> CreateEndingDemoSequence() override;
private:
    DemoSequenceNativeServices& services_;
};
}
