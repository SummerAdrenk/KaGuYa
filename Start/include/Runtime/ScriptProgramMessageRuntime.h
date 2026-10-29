#pragma once

#include <Runtime/MessageFormatSelectionRuntime.h>
#include <Runtime/ProgramRuntimeServices.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class IScriptProgramMessageSource
{
public:
    virtual ~IScriptProgramMessageSource() = default;
    virtual float ReadVersionAfterRootAt4CFD70() = 0;
    virtual std::int32_t ReadTextAfterRootAt4CFDC5(std::int32_t index, MessageFormatSelectionTarget output) = 0;
    virtual std::int32_t ReadFormatAfterRootAt4CFE13(std::int32_t index, MessageFormatSelectionTarget output) = 0;
};
class RootScriptProgramMessageSource final : public IScriptProgramMessageSource
{
public:
    explicit RootScriptProgramMessageSource(RootRuntimeOwner& root) noexcept : root_(root) {}
    float ReadVersionAfterRootAt4CFD70() override;
    std::int32_t ReadTextAfterRootAt4CFDC5(std::int32_t index, MessageFormatSelectionTarget output) override;
    std::int32_t ReadFormatAfterRootAt4CFE13(std::int32_t index, MessageFormatSelectionTarget output) override;
private:
    RootRuntimeOwner& root_;
};
// 4CFD3B..4CFEFC：仅command0的三Ref/版本查询/CString写入，不吞外围PROGRAM异常。
void ApplyScriptProgramMessageAt4CFD3B(std::int32_t index, IScriptProgramMessageSource& source,
    IProgramCurrentEnvironment& destination);
}
