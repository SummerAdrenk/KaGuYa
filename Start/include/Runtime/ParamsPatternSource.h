#pragma once

namespace KaGuYa::Start::Runtime
{
struct ParamsPatternNative;

// 46ED5B/46EE84各自捕获当前Pattern；随后条目读取只借这次取得的正文，不AddRef。
class IParamsPatternSource
{
public:
    virtual ~IParamsPatternSource() = default;
    [[nodiscard]] virtual const ParamsPatternNative* CapturePattern() const = 0;
};

// 显式固定模型适配口；生产懒根使用ParamsResourceNameRuntime，不能在构造时生成快照。
class FixedParamsPatternSource final : public IParamsPatternSource
{
public:
    explicit FixedParamsPatternSource(const ParamsPatternNative& pattern) noexcept : pattern_(pattern) {}
    [[nodiscard]] const ParamsPatternNative* CapturePattern() const override { return &pattern_; }
private:
    const ParamsPatternNative& pattern_;
};
}
