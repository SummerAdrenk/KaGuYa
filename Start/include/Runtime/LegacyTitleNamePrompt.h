#pragma once

#include <Runtime/LegacyTitleNameDialog.h>
#include <Runtime/LegacyTitleWorkflow.h>

namespace KaGuYa::Start::Runtime
{
// owner 将此对象交给 LegacyTitleWorkflowServices::names；此处不重复工作流的 bit/skip 门控。
class LegacyTitleNamePrompt final : public ILegacyTitleNamePrompt
{
public:
    explicit LegacyTitleNamePrompt(ILegacyTitleNameServices& source,
        ILegacyTitleNameApi& api = DefaultLegacyTitleNameApi(),
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept
        : source_(source), api_(api), strings_(strings) {}
    bool EditableNameBit(std::uint32_t bit) override;
    bool SkipNamePrompt() override;
    std::int32_t RunModal() override;
private:
    ILegacyTitleNameServices& source_;
    ILegacyTitleNameApi& api_;
    IRuntimeStringManagerSource& strings_;
};
}
