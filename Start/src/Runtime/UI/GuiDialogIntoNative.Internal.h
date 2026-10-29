#pragma once

#include <Runtime/GuiDialogIntoNative.h>
#include <Runtime/EnvironmentObjectReference.h>

namespace KaGuYa::Start::Runtime::GuiDialogIntoNativeDetail
{
using Common = GuiDialogNativeBody5Ch;
using Body = GuiDialogIntoNativeBody68h;
using Slot = YtlReferenceSlotRuntime;

// 也适用于首5Ch为common、+5C为三指针的更大派生正文。
inline GuiDialogChildrenStorage& Dialogs(Common& body) noexcept
{
    return *reinterpret_cast<GuiDialogChildrenStorage*>(reinterpret_cast<std::byte*>(&body) + 0x5C);
}
inline Common* Resolve(const Slot& slot) noexcept
{ return ResolveEnvironmentObjectReference<Common>(slot); }
inline void Copy(Slot& output, const Slot& source) noexcept
{
    output.baseAt00 = source.baseAt00;
    output.biasAt04 = source.biasAt04;
    output.controlAt08 = source.controlAt08;
    if (output.controlAt08) ++output.controlAt08->referenceCountAt04;
}
// 原正常尾不清参数/赋值目标；payload之后重读同一槽control，不能复制临时槽释放。
inline void ReleaseWithoutClearing(Slot& slot)
{
    auto* control = slot.controlAt08;
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        if (auto* current = slot.controlAt08) current->DeleteControlAtVslot04(1);
    }
}
// 仅原栈参数的EH资格；不是正文state/owner。正常尾抛出与EH清理抛出均terminate。
struct Argument final
{
    Slot& slot;
    bool active = true;
    ~Argument() noexcept { if (active) ReleaseYtlReference(slot); }
    void Finish() noexcept { active = false; ReleaseWithoutClearing(slot); }
};

Common* __fastcall Delete(Common*, void*, std::uint32_t) noexcept;
void __fastcall SetDirty(Common*, void*, Slot);
void __fastcall Begin(Common*, void*);
std::int32_t __fastcall Poll(Common*, void*);
std::int32_t __fastcall End(Common*, void*);
void __fastcall Composite(Common*, void*, Slot, const RECT*);
void __fastcall Draw(Common*, void*, const RECT*);
std::int32_t __fastcall Append(Common*, void*, Slot);
Slot* __fastcall Find(Common*, void*, Slot*, std::int32_t);
}
