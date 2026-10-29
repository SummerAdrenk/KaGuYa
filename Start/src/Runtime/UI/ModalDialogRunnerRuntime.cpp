#include <Runtime/ModalDialogRunnerRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
ModalDialogRunnerRuntime::ModalDialogRunnerRuntime(
    IModalDialogRunnerServices& services) noexcept
    : services_(&services)
{
}

std::int32_t ModalDialogRunnerRuntime::RunAt5071D0(
    IModalDialogRuntimeTargetReference& targetReference)
{
    const auto resolve = [&targetReference]() -> IModalDialogRuntimeTarget& {
        auto* const target = targetReference.ResolveAt4A5330();
        if (!target)
        {
            // 原程序会在随后访问空虚表时异常；重写版在同一边界明确报告错误。
            throw std::runtime_error("Modal dialog YTL reference resolved to null");
        }
        return *target;
    };

    resolve().BeginAtVslot44();

    while (resolve().PollAtVslot48())
    {
        // 507251..507398：绘制、同步和 root owner 更新均先于消息泵。
        services_->RenderScreenIfGraphicsLoaded();
        services_->SynchronizeRuntimeAt413BE0();
        if (services_->RootOwnerUpdateEnabledAt1B0())
            services_->UpdateRootOwnerAt6CAtVslot18();

        MSG message{};
        if (services_->PeekMessageNoRemove(message))
        {
            for (;;)
            {
                const int getMessageResult =
                    services_->GetMessageAt5073B0(message);
                if (getMessageResult == -1)
                    return 2;

                // 原循环连 GetMessage==0 的 WM_QUIT 也会经过这两步。
                services_->TranslateMessageAt5073C9(message);
                services_->DispatchMessageAt5073D3(message);
                if (!services_->PeekMessageNoRemove(message))
                    break;
            }
        }

        services_->SynchronizeRuntimeAt413BE0();
        if (services_->ExitRequestedAt1BC())
            throw 0;
        services_->UpdateInputAt450AA0();
    }

    if (!resolve().EndAtVslot4C())
        return 2;
    return resolve().ResultAtVslot50();
}
}
