#include <Runtime/ScriptProgramMessageRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/FileManagerNative.h>
#include <Runtime/MessageOwnerNative.h>
#include <Runtime/ScriptCallStacksNative.h>
#include "ScriptFileCommandSupport.h"

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
MessageOwnerNative34h& CaptureMessage(RootRuntimeOwner& root)
{
    root.EnsureCreatedAt413BE0();
    auto* manager = root.RequirePublished().PostWindow().FileManagerAt00();
    auto* const message = manager ? ResolveMessageReference(manager->messageAt10) : nullptr;
    if (!message)
        throw std::logic_error("PROGRAM null message owner (modern safety boundary)");
    return *message;
}
struct MessageLocals
{
    MessageTextCommandReference name;
    RuntimeCStringArrayReference voices;
    MessageTextCommandReference body;
    MessageFormatSelectionTarget Target() noexcept { return {name, voices, body}; }
    void Finish() noexcept
    {
        // 4CFE8A..4CFEFC的正常路径固定每个control；4100C0展开才重读各引用槽。
        for (auto* slot : {&body.RawSlot(), &voices.RawSlot(), &name.RawSlot()})
        {
            ReleaseCapturedScriptStackControl(slot->controlAt08);
            *slot = {};
        }
    }
};
static_assert(sizeof(MessageLocals) == 0x24);
}
float RootScriptProgramMessageSource::ReadVersionAfterRootAt4CFD70()
{
    auto& message = CaptureMessage(root_);
    if (!message.Reader()) throw std::logic_error("PROGRAM null reader (modern safety boundary)");
    return message.Reader()->ReadVersionAt47DDE0();
}
std::int32_t RootScriptProgramMessageSource::ReadTextAfterRootAt4CFDC5(std::int32_t index,
    MessageFormatSelectionTarget output)
{
    auto& message = CaptureMessage(root_);
    return message.ReadTextAt4AD590(index, output);
}
std::int32_t RootScriptProgramMessageSource::ReadFormatAfterRootAt4CFE13(std::int32_t index,
    MessageFormatSelectionTarget output)
{
    auto& message = CaptureMessage(root_);
    return message.ReadFormatAt417C40(index, output, 0, false);
}
void ApplyScriptProgramMessageAt4CFD3B(std::int32_t index, IScriptProgramMessageSource& source,
    IProgramCurrentEnvironment& destination)
{
    MessageLocals locals;
    MessageTextCommandReference* selected;
    // 原COMISS/JB对NaN也进入legacy；阈值5AEBA8原PE为4.0f，不读取params版本。
    if (source.ReadVersionAfterRootAt4CFD70() >= 4.0f)
    {
        if (source.ReadTextAfterRootAt4CFDC5(index, locals.Target()) < 0 || !locals.body.RawSlot().baseAt00) throw 2;
        selected = &locals.body;
    }
    else
    {
        if (source.ReadFormatAfterRootAt4CFE13(index, locals.Target()) < 0 || !locals.name.RawSlot().baseAt00) throw 2;
        selected = &locals.name; // 4CFE42/4CFE4C是+00的姓名，不是+18正文。
    }
    auto* command = selected->Get();
    if (!command) throw std::logic_error("PROGRAM null text command (modern safety boundary)");
    FileCommandDetail::TextSlot text;
    ::new (&text.value) RuntimeCString(command->GetTextAt47D550());
    FileCommandDetail::TextUnwind cleanup{&text.value};
    const char* const captured = text.value.GetString();
    destination.WriteStringAt3C(captured); // 原先取chars，再Ensure root/取SceneManager；CString仍存活。
    DestroyRuntimeCStringAt40CD40(&text.value);
    cleanup.value = nullptr;
    locals.Finish();
}
}
