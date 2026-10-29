#include "GuiDialogIntoNative.Internal.h"

namespace KaGuYa::Start::Runtime::GuiDialogIntoNativeDetail
{
void __fastcall SetDirty(Common* body, void*, Slot argument)
{
    Argument cleanup{argument};
    if (&body->dirtyAt50 != &argument)
    {
        ReleaseWithoutClearing(body->dirtyAt50);
        Copy(body->dirtyAt50, argument);
    }
    auto& storage = Dialogs(*body);
    for (auto* position = storage.begin; position != storage.end; ++position)
    {
        auto* child = Resolve(*position);
        Slot copied;
        Copy(copied, argument);
        child->vptrAt00->setDirtyAt18(child, copied);
    }
    cleanup.Finish();
}
void __fastcall Begin(Common* body, void*)
{
    body->vptrAt00->writeEnabledAt24(body, 1);
    auto& storage = Dialogs(*body);
    for (auto* position = storage.begin; position != storage.end; ++position)
    {
        auto* child = Resolve(*position);
        child->vptrAt00->beginAt44(child);
    }
}
std::int32_t __fastcall Poll(Common* body, void*)
{
    auto& storage = Dialogs(*body);
    for (auto* position = storage.begin; position != storage.end; ++position)
    {
        auto* child = Resolve(*position);
        child->vptrAt00->pollAt48(child);
    }
    return 1;
}
std::int32_t __fastcall End(Common* body, void*)
{
    auto& storage = Dialogs(*body);
    for (auto* position = storage.begin; position != storage.end; ++position)
    {
        auto* child = Resolve(*position);
        child->vptrAt00->endAt4C(child);
    }
    return 1;
}
void __fastcall Composite(Common* body, void*, Slot argument, const RECT* clip)
{
    Argument cleanup{argument};
    if (body->enabledAt38)
    {
        auto& storage = Dialogs(*body);
        for (auto* position = storage.begin; position != storage.end; ++position)
        {
            const bool normal = body->stateAt34 != 0;
            auto* child = Resolve(*position);
            std::int32_t previous = 0;
            if (!normal)
            {
                previous = child->vptrAt00->readStateAt20(child);
                child = Resolve(*position);
                child->vptrAt00->writeStateAt1C(child, 0);
                child = Resolve(*position);
            }
            Slot copied;
            Copy(copied, argument);
            child->vptrAt00->compositeAt5C(child, copied, clip);
            if (!normal)
            {
                child = Resolve(*position);
                child->vptrAt00->writeStateAt1C(child, previous);
            }
        }
    }
    // EH不恢复临时置零的子state；恢复只属于原正常分支。
    cleanup.Finish();
}
void __fastcall Draw(Common* body, void*, const RECT* clip)
{
    if (!body->enabledAt38) return;
    auto& storage = Dialogs(*body);
    for (auto* position = storage.begin; position != storage.end; ++position)
    {
        const bool normal = body->stateAt34 != 0;
        auto* child = Resolve(*position);
        std::int32_t previous = 0;
        if (!normal)
        {
            previous = child->vptrAt00->readStateAt20(child);
            child = Resolve(*position);
            child->vptrAt00->writeStateAt1C(child, 0);
            child = Resolve(*position);
        }
        child->vptrAt00->drawAt60(child, clip);
        if (!normal)
        {
            child = Resolve(*position);
            child->vptrAt00->writeStateAt1C(child, previous);
        }
    }
}
}
