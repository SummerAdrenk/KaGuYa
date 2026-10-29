#pragma once

#include <Runtime/ReadScriptSoundRequestsSource.h>

namespace KaGuYa::Start::Runtime
{
// 4A9090逐帧协调器只借来源；每个原捕获点独立执行，不缓存整帧manager/映射/参数。
class ReadScriptSoundRequestsRuntime final
{
public:
    explicit ReadScriptSoundRequestsRuntime(IReadScriptSoundRequestsSource& source) noexcept;
    void ProcessSoundRequestsAt4A9090();

private:
    [[nodiscard]] bool SkipRequestedAt4AA540(ReadScriptUnreadReceiver text);
    IReadScriptSoundRequestsSource& source_;
};
}
