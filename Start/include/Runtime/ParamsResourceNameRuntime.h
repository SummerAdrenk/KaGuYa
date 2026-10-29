#pragma once

#include <Runtime/ScrResourceNameSource.h>
#include <Runtime/ParamsPatternSource.h>

namespace KaGuYa::Start::Runtime
{
class ParamsRuntimeRootOwner;
// 只消费已捕获Pattern正文；负索引先于解引用，返回借用chars，不额外418480。
const char* ReadParamsPatternNameNativeAt4AD1B0(const ParamsPatternNative*,std::int32_t);

class ParamsResourceNameRuntime final : public IScrResourceLookupSource, public IScrResourceNameRootSource,
    public IParamsPatternSource
{
public:
    explicit ParamsResourceNameRuntime(ParamsRuntimeRootOwner& params) noexcept;
    [[nodiscard]] const ParamsPatternNative* CapturePattern() const override;
    [[nodiscard]] ScrResourceNameReceiver CapturePatternNames() const override;
    [[nodiscard]] ScrResourceNameRootReceiver CaptureParamsForNames() const override;
    [[nodiscard]] std::int32_t FindResourceIdAt47A020(const char* nameCp932) const override;

private:
    static ScrResourceNameReceiver CaptureRootPattern(const void* receiver);
    static const char* FindName(const void* receiver, std::int32_t id);
    ParamsRuntimeRootOwner& params_;
};
}
