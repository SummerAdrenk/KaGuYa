#include "GuiPanelNative.Internal.h"
#include <new>

namespace KaGuYa::Start::Runtime
{
YtlReferenceSlotRuntime* CreateGuiPanelAt504E90(YtlReferenceSlotRuntime& output,
    void* parent, const GuiStaticDescriptorNative24h& descriptor,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings)
{
    using namespace GuiPanelNativeDetail;
    GraphicsSurfaceNativeScope surfaceScope(surfaces);
    UnwrittenReference group;
    LoadGuiControlResourceAt5047A0(group.slot, descriptor.namesAt04.data(), loader, surfaces, strings);
    LocalReference groupLifetime{group.slot};
    const auto base = group.slot.baseAt00;
    if (!base) throw 1;
    auto* captured = group.slot.controlAt08;
    if (const auto* point = descriptor.positionAt20)
    {
        const auto y = point->y;
        const auto x = point->x;
        Slot argument{base, group.slot.biasAt04, captured};
        if (captured) ++captured->referenceCountAt04;
        MoveGuiControlResourceAt5086C0(argument, x, y);
    }
    void* raw = ::operator new(0x6C);
    struct RawUnwind final
    {
        void* p;
        bool active = true;
        ~RawUnwind() noexcept { if (active) ::operator delete(p, std::size_t{0x6C}); }
    } rawLifetime{raw};
    Body* body = nullptr;
    if (raw)
    {
        // 504F20在定位和new6C后重读base/bias，但保留本次ESI control。
        body = ConstructGuiPanelInlineAt504F20(raw, parent, descriptor.eventAt00,
            CopyWithControl(group.slot, captured));
    }
    rawLifetime.active = false; // fresh包装前回state0；new18失败不回收完成正文。
    ConstructGuiPanelReferenceAt504FC4(output, body);
    groupLifetime.Captured(captured); // state4是终止边界，不是output回滚。
    return &output;
}
YtlReferenceSlotRuntime* CreateGuiPanelAt504E90(YtlReferenceSlotRuntime* freshOutput,
    GuiDialogNativeBody5Ch* parent, const GuiStaticDescriptorNative24h* descriptor,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings)
{ return CreateGuiPanelAt504E90(*freshOutput, parent, *descriptor, loader, surfaces, strings); }
}
