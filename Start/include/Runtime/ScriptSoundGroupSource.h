#pragma once

#include <Runtime/ParamsRuntimeRoot.h>

namespace KaGuYa::Start::Runtime
{
// 4D1520每个418480调用独立捕获根；不能以SoundManager的物理槽布局代替params。
class IScriptSoundGroupSource
{
public:
    virtual ~IScriptSoundGroupSource() = default;
    [[nodiscard]] virtual ParamsRuntimeRoot* CaptureAt418480() = 0;
};

class ParamsScriptSoundGroupSource final : public IScriptSoundGroupSource
{
public:
    explicit ParamsScriptSoundGroupSource(ParamsRuntimeRootOwner& owner) noexcept : owner_(owner) {}
    [[nodiscard]] ParamsRuntimeRoot* CaptureAt418480() override { return owner_.CaptureAt418480(); }
private:
    ParamsRuntimeRootOwner& owner_;
};
}
