#include "GuiDialogIntoNative.Internal.h"

#include <bit>

namespace KaGuYa::Start::Runtime
{
std::int32_t NestedGuiDialogReferenceCount(const GuiDialogChildrenStorage& storage) noexcept
{ return GuiControlChildReferenceCount(storage); }
std::int32_t NestedGuiDialogReferenceCapacity(const GuiDialogChildrenStorage& storage) noexcept
{ return GuiControlChildReferenceCapacity(storage); }

YtlReferenceSlotRuntime* GrowNestedGuiDialogReferencesAt4D97A0(GuiDialogChildrenStorage& storage,
    YtlReferenceSlotRuntime* position, const YtlReferenceSlotRuntime& source)
{
    // 主体220指令与4D93B0等价；4D8020/53BC00/53B6E0/53C9F0也逐分支等价。
    // 共用真实三指针算法，含source晚读、中间插入、EH范围与32-byte对齐delete。
    return GrowGuiControlChildrenAt4D93B0(storage, position, source);
}
void AppendNestedGuiDialogReference(GuiDialogChildrenStorage& storage,
    const YtlReferenceSlotRuntime& source)
{
    auto* position = storage.end;
    if (position != storage.capacity)
    {
        GuiDialogIntoNativeDetail::Copy(*position, source);
        ++storage.end;
    }
    else static_cast<void>(GrowNestedGuiDialogReferencesAt4D97A0(storage, position, source));
}
void ReleaseNestedGuiDialogReferencesRangeAt4D8020(YtlReferenceSlotRuntime* begin,
    YtlReferenceSlotRuntime* end) noexcept
{
    // 两范围函数各49指令等价；固定end、回调后重读control、只清base/control。
    ReleaseGuiControlChildrenRangeAt4D7F00(begin, end);
}
void DestroyNestedGuiDialogReferencesAt4F6DC0(GuiControlChildrenStorage& storage) noexcept
{
    // 独立核4F6DC0：空begin不改头；非空释放后重读begin/capacity，delete后才清头。
    // 该算法不解析元素正文；复用4F6C60不会把dialog当作控件。
    DestroyGuiControlChildrenAt4F6C60(storage);
}

namespace GuiDialogIntoNativeDetail
{
std::int32_t __fastcall Append(Common* body, void*, Slot argument)
{
    Argument cleanup{argument};
    auto& storage = Dialogs(*body);
    AppendNestedGuiDialogReference(storage, argument);
    const auto index = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(NestedGuiDialogReferenceCount(storage)) - 1u);
    cleanup.Finish();
    return index;
}
Slot* __fastcall Find(Common* body, void*, Slot* output, std::int32_t id)
{
    auto& storage = Dialogs(*body);
    for (auto* position = storage.begin; position != storage.end; ++position)
    {
        auto* child = Resolve(*position);
        if (child->vptrAt00->idAt0C(child) == id)
        {
            // 509E42在id回调之后才复制三个字段。fresh输出不释放旧位模式，允许自别名。
            Copy(*output, *position);
            return output;
        }
    }
    *output = {};
    return output;
}
}
}
