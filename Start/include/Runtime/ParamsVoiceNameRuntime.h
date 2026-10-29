#pragma once

#include <Runtime/ScrVmVoiceHandler.h>

namespace KaGuYa::Start::Runtime
{
class ParamsRuntimeRootOwner;

// 4D325B..4D32E8：每次从静态Params根取得Pattern，不缓存child或名称。
// params须长于本对象；返回raw不保活Pattern，调用方不得跨其替换/修改保存。
class ParamsVoiceNameRuntime final : public IScrVmVoiceNameSource
{
public:
    explicit ParamsVoiceNameRuntime(ParamsRuntimeRootOwner& params) noexcept;
    [[nodiscard]] const char* FindVoiceNameAt4D3286(std::int32_t index) const override;

private:
    ParamsRuntimeRootOwner& params_;
};
}
