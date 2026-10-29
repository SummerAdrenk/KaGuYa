#pragma once

#include <Runtime/TokimekiGaugeNative.h>
#include <Runtime/LoveHolicGaugesNative.h>
#include <Runtime/ScriptNamedValueNative.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class IDrawManagerNativeRootSource;
class IScrVmCommandErrorSink;
class RootGaugeValueSource final : public ITokimekiGaugeStateSource, public ILoveHolicGaugeStateSource
{
public:
    RootGaugeValueSource(RootRuntimeOwner&, ScriptNamedValueNativeTables,
        IScrVmCommandErrorSink&, IRuntimeStringManagerSource&) noexcept;
    std::int32_t ReadGaugeValue(const char* name) override;
private:
    RootRuntimeOwner& root_;
    ScriptNamedValueNativeTables names_;
    IScrVmCommandErrorSink& errors_;
    IRuntimeStringManagerSource& strings_;
};

class IProgramGaugeRequests
{
public:
    virtual ~IProgramGaugeRequests() = default;
    virtual void RequestGauge(bool secondary, bool requested) = 0;
};
// 两条已确认的调用协议共用现有逐次取Draw接口；不将Root生命周期塞进图层组件。
void RequestGaugeLayerNative(IDrawManagerNativeRootSource&, bool secondary, bool requested);
void RestorePrimaryGaugeLayerNative(IDrawManagerNativeRootSource&, TokimekiGaugeNativeServices*,
    LoveHolicGaugesNativeServices*);
class RootProgramGaugeRequests final : public IProgramGaugeRequests
{
public:
    explicit RootProgramGaugeRequests(RootRuntimeOwner& root) noexcept : root_(root) {}
    void RequestGauge(bool secondary, bool requested) override;
    void RestoreVisiblePrimaryGauge();
private:
    RootRuntimeOwner& root_;
};
}
