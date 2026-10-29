#include "SettingDialogNative.Internal.h"
#include <new>

namespace KaGuYa::Start::Runtime
{
using namespace SettingDialogNativeDetail;
SettingDialogNativeBodyACh* ConstructSettingDialogNativeAt4F1BF0(void* storage)
{
    auto& s = RequireSettingDialogNativeServices();
    auto* b = ::new(storage) SettingDialogNativeBodyACh;
    ConstructGuiDialogAt4E0BE0(&b->common, nullptr, -1);
    b->dialogsAt5C = {};
    b->common.vptrAt00 = &SettingDialogNativeVtable().common;
    struct Unwind final
    {
        Body* p; IEnvironmentRootMemory& memory; int stage = 0; bool active = true;
        ~Unwind() noexcept
        {
            if (!active) return;
            if (stage >= 4) DestroyRuntimeCStringVectorAt40DD20(p->snapshotNamesAt84, memory);
            if (stage >= 3) DestroyRuntimeCStringAt40CD40(&p->secondNameAt80);
            if (stage >= 2) DestroyRuntimeCStringAt40CD40(&p->firstNameAt7C);
            if (stage >= 1) ReleaseYtlReference(p->backgroundAt68);
            DestroyGuiDialogIntoNativeAt4F7B80(&p->common);
        }
    } unwind{b, s.memory}; // state0只取得Into资格，不跑派生完整析构。
    b->backgroundAt68 = {}; unwind.stage = 1;
    InitializeEmptyRuntimeCString(b->firstNameAt7C, s.strings); unwind.stage = 2;
    InitializeEmptyRuntimeCString(b->secondNameAt80, s.strings); unwind.stage = 3;
    b->snapshotNamesAt84 = {}; unwind.stage = 4;
    auto* raw = s.memory.AllocateBody(0x7C);
    Common* child = nullptr;
    {
        struct Raw final
        { void* p; IEnvironmentRootMemory& memory; bool active = true; ~Raw() noexcept { if (active) memory.DeleteBody(p, 0x7C); } } cleanup{raw, s.memory};
        if (raw) child = &ConstructSettingSystemPageNativeAt4F3740(raw, &b->common, 0)->common;
        cleanup.active = false;
    }
    FreshReference parameter;
    // 原直接构造按值参数；new18失败没有此Ref的展开资格，故留下孤立7Ch正文。
    ConstructOwnedDialogReference(parameter.slot, child, 0x7C, s.memory);
    GuiDialogIntoBaseVtable().appendDialogAt8C(&b->common, parameter.slot); // 直接4FF130消费参数。

    FreshReference dirty;
    ConstructGuiDirtyRectReferenceAt45B227(dirty.slot, s.memory);
    LocalReference dirtyLifetime{dirty.slot}; // 成功返回后state6；失败前不补回收dirty80。
    auto* captured = Read(dirty.slot.controlAt08);
    if (&b->common.dirtyAt50 != &dirty.slot)
    {
        auto& old = b->common.dirtyAt50;
        if (Read(old.controlAt08)) { ReleaseWithoutClearing(old); old.baseAt00 = 0; }
        old.baseAt00 = Read(dirty.slot.baseAt00); old.biasAt04 = 0; old.controlAt08 = captured;
        if (captured) ++captured->referenceCountAt04;
    }
    for (auto* position = Read(b->dialogsAt5C.begin); position != Read(b->dialogsAt5C.end); ++position)
    {
        auto* nested = Resolve<Common>(*position);
        Slot argument{Read(dirty.slot.baseAt00), 0, captured};
        if (captured) ++captured->referenceCountAt04;
        Read(nested->vptrAt00)->setDirtyAt18(nested, argument);
    }
    dirtyLifetime.Captured(captured, false); // state7 normal尾，之后退回state4。
    AdoptGuiInputAt536EA0(b->common.inputAt18, NewGameGuiInputAt4EAAED());
    unwind.active = false;
    return b;
}
GuiDialogNativeBody5Ch* ConstructSettingDialogAt4F1BF0(void* raw)
{ return &ConstructSettingDialogNativeAt4F1BF0(raw)->common; }
}
