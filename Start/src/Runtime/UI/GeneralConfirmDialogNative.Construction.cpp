#include "GeneralConfirmDialogNative.Internal.h"
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace GeneralConfirmNativeDetail;
void AppendStatic(Body& body, const GuiStaticDescriptorNative24h& descriptor)
{
    auto& deps = RequireGeneralConfirmDialogNativeServices();
    Slot child;
    CreateGuiStaticAt505800(child, &body.common, descriptor, deps.pictures, deps.surfaces, deps.strings);
    // 4E33DF/4E340B后直接转交4FF020的按值参数；无多余fresh输出守卫。
    GuiDialogBaseVtable().appendChildAt10(&body.common, child);
}
void AppendButton(Body& body, const GuiButtonDescriptorNative2Ch& descriptor)
{
    auto& deps = RequireGeneralConfirmDialogNativeServices();
    Slot child;
    CreateGuiButtonAt5041D0(child, &body.common, descriptor, deps.pictures, deps.surfaces, deps.strings);
    RefLifetime childLifetime{child}; // state5，失败只展开已成功返回的本地child。
    Slot event;
    CreateToolClickedEventAt4E2F5A(event, child);
    // 4E360A..364B与Tool共享内联发布序；new18失败时正文没有展开资格。
    RefLifetime eventLifetime{event};
    auto* eventControl = event.controlAt08;
    Slot argument;
    Copy(argument, event);
    RefLifetime argumentLifetime{argument};
    auto* selected = ResolveGuiControl(child);
    auto& target = selected->eventAt3C;
    if (&target != &argument)
    {
        ReleaseUncleared(target);
        target = {event.baseAt00, 0, eventControl};
        if (eventControl) ++eventControl->referenceCountAt04;
    }
    selected->eventEnabledAt48 = 1;
    argumentLifetime.FinishCaptured(eventControl); // state8，普通尾缓存ESI，EH重读argument。
    Slot copiedChild;
    Copy(copiedChild, child);
    RefLifetime copiedLifetime{copiedChild};
    AppendGuiControlChildReference(body.common.childrenAt0C, copiedChild);
    copiedLifetime.Finish(true); // state10，先child入列，再event入列。
    AppendPopupClickedEventReferenceAt4EA97F(body.eventsAt74, event, eventControl);
    eventLifetime.FinishCaptured(eventControl);
    childLifetime.Finish(true);
}
}
GuiDialogNativeBody5Ch* ConstructGeneralConfirmDialogNativeAt4E3330(void* storage, std::int32_t mode)
{
    auto* body = ::new(storage) GeneralConfirmDialogNativeBody84h;
    ConstructGuiDialogAt4E0BE0(&body->common, nullptr, -1);
    body->capturedAt5C = {};
    body->eventsAt74 = {};
    body->common.vptrAt00 = &GeneralConfirmDialogNativeVtable().common;
    struct Unwind final
    {
        Body& body;
        bool armed = true;
        ~Unwind() noexcept { if (armed) GeneralConfirmNativeDetail::DestroyMembers(body); }
    } unwind{*body};
    AdoptGuiInputAt536EA0(body->common.inputAt18, NewGameGuiInputAt4EAAED());
    // 在原第一次表读取处选择profile；5-8删除由现代SaveLoad承接，不在此重复确认。
    const auto& profile = GeneralConfirmNativeDetail::Resources();
    const auto& row = FindGeneralConfirmMode(profile, mode);
    AppendStatic(*body, profile.statics[static_cast<std::size_t>(row.background)]);
    if (row.message >= 0) AppendStatic(*body, profile.statics[static_cast<std::size_t>(row.message)]);
    if (row.buttons == GeneralConfirmButtons::Acknowledge)
    {
        // 08的4E9AD0/5764A4：mode11只构造event16的Ack按钮，不构造双向导航。
        for (const auto& descriptor : profile.acknowledgeButtons) AppendButton(*body, descriptor);
    }
    else
    {
        // 09-11原构造先追加slash再追加Yes/No；保留重复event和插入次序。
        for (const auto& descriptor : profile.additionalStatics) AppendStatic(*body, descriptor);
        for (const auto& descriptor : profile.yesNoButtons) AppendButton(*body, descriptor);
        for (const auto& navigation : profile.navigation)
            CreateAndAppendGuiNavigation(body->common.navigationAt24, navigation.action, navigation.mappings.data());
    }
    body->modeAt80 = mode; // 4E38E4，所有子对象/导航完成后才写。
    unwind.armed = false;
    return &body->common;
}
}
