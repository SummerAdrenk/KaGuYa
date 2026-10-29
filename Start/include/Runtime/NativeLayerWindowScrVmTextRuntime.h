#pragma once
#include <Runtime/ScrVmTextHandler.h>
#include <Runtime/NativeLayerWindowScrVmUpdateRuntime.h>
#include <Runtime/ScriptSoundGroupSource.h>
#include <Runtime/ScriptSoundLeafSource.h>

namespace KaGuYa::Start::Runtime
{
// TEXT与UPDATE共用同一CScript/原4Ch图层边界，不借旧modern对象或转换原异常类别。
class NativeLayerWindowScrVmTextRuntime final:public IScrVmTextRuntimeBoundary
{
public:
    NativeLayerWindowScrVmTextRuntime(NativeLayerWindowScrVmUpdateRuntime& updates,
        const IScrResourceNameSource& names,IScriptSoundGroupSource& groups,
        IScriptSoundLeafSource& sounds,IScrVmSoundOpenGate& gate) noexcept
        :updates_(updates),names_(names),groups_(groups),sounds_(sounds),gate_(gate){}
    std::size_t FirstStringGroupCountAt50() const override;
    std::size_t SecondStringGroupCountAt48() const override;
    void ApplyResultLayerList(std::int32_t) override;
    void ApplyFirstStringSlot(std::int32_t,std::size_t) override;
    void ApplySecondStringSlot(std::int32_t,std::size_t) override;
    void SelectEffect(std::int32_t) override;
private:
    NativeLayerWindowScrVmUpdateRuntime& updates_;
    const IScrResourceNameSource& names_;
    IScriptSoundGroupSource& groups_;
    IScriptSoundLeafSource& sounds_;
    IScrVmSoundOpenGate& gate_;
};
}
