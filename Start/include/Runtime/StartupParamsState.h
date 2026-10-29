#pragma once

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// 4D53D0的root/CParams边界；prologue含先捕获目标root再取Environment的完整顺序。
class IStartupParamsState
{
public:
    virtual ~IStartupParamsState() = default;
    virtual void ReplaceParamsGeneration() = 0;
    virtual void CopyRegistryPrologueFlag() = 0;
    virtual void WriteReplayFile(std::int32_t value) = 0;
    virtual void WriteReplayPosition(std::int32_t value) = 0;
    virtual void ClearHistoryAt4D5730() = 0;
};
}
