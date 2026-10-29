#pragma once

#include <Runtime/YtlReferenceArrayRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct ParamsGameSystem;
class ParamsRuntimeRootOwner;

class IDemoParamsSource
{
public:
    virtual ~IDemoParamsSource() = default;
    [[nodiscard]] virtual const ParamsGameSystem* CaptureGameSystemAt418480() = 0;
};

// 每次显式 Capture 都经过同一 Params owner；不缓存 GS，也不增持现代 shared_ptr。
class ParamsDemoRootSource final : public IDemoParamsSource
{
public:
    explicit ParamsDemoRootSource(ParamsRuntimeRootOwner& owner) noexcept : owner_(owner) {}
    [[nodiscard]] const ParamsGameSystem* CaptureGameSystemAt418480() override;
private:
    ParamsRuntimeRootOwner& owner_;
};

// receiver 已由 caller 选定，循环不再取 root；out 必须是不拥有资源的返回槽。
// miss 清三个字段；命中取得真实 DemoData 的一个引用，不生成值快照。
void FindParamsDemoAt4ACC40(const ParamsGameSystem* gameSystem,
    const char* searchName, YtlReferenceSlotRuntime& out);

// Logo/Ending 局部 Ref 的正常与异常释放均有终止边界，payload 后仍重读当前 control。
void ReleaseParamsDemoLookupAt4A32A0(YtlReferenceSlotRuntime& reference) noexcept;
}
