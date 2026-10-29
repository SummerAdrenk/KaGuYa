#include "EditWindowNativeViewport.Internal.h"
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace EditWindowNativeViewportDetail;
class ViewportRectangleControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ViewportRectangleControl(RECT* rectangle) noexcept
        : EnvironmentObjectReferenceControl(rectangle, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    { if (payloadAt14) ::operator delete(payloadAt14, std::size_t{0x10}); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(ViewportRectangleControl) == 0x18);

WindowModeNative2Ch* CaptureMode(EditWindowNativeViewportServices& services)
{ return Resolve<WindowModeNative2Ch>(services.root.CaptureModeSlotAfter413BE0()); }
std::uint32_t Width(EditWindowNativeViewportServices& services)
{ return Read(services.environment.CaptureAt4180C0()->widthAt18); }
std::uint32_t Height(EditWindowNativeViewportServices& services)
{ return Read(services.environment.CaptureAt4180C0()->heightAt1C); }
}

void UpdateEditWindowNativeViewportAt4875F0(EditWindowNativeReferences& receiver,
    EditWindowNativeBody98h* target, EditWindowNativeViewportServices& services)
{
    // 487627/487663各自固定一个mode receiver；读取顺序dy/dx、height/width。
    auto* origin = CaptureMode(services);
    const LONG dy = Read(origin->viewportAt0C[1]);
    const LONG dx = Read(origin->viewportAt0C[0]);
    auto* dimensions = CaptureMode(services);
    const LONG height = Read(dimensions->viewportAt0C[3]);
    auto* cursor = Read(receiver.begin);
    const LONG width = Read(dimensions->viewportAt0C[2]);
    if (cursor == Read(receiver.end)) return;
    do
    {
        auto* selected = Resolve<EditWindowNativeBody98h>(*cursor);
        if (target)
        {
            if (selected != target) goto next;
        }
        else if (!services.platform.IsWindowVisible(Read(selected->outerAt00.windowAt04))) goto next;
        {
            // 可见性/模式/分配/Environment/布局叶均可重入；每个原解Ref点重新解析。
            selected = Resolve<EditWindowNativeBody98h>(*cursor);
            const auto* logical = static_cast<const volatile RECT*>(&selected->logicalAt4C);
            RECT rectangle{logical->left, logical->top, logical->right, logical->bottom};
            TemporaryMargins temporary;
            EnvironmentObjectReferenceControl* edi = nullptr;
            RECT* margins = nullptr;
            if (Read(CaptureMode(services)->modeAt04))
            {
                const LONG logicalWidth = Sub(rectangle.right, rectangle.left);
                const LONG logicalHeight = Sub(rectangle.bottom, rectangle.top);
                rectangle.left = Scale(width, rectangle.left, Width(services));
                rectangle.top = Scale(height, rectangle.top, Height(services));
                rectangle.right = Add(Scale(width, logicalWidth, Width(services)), rectangle.left);
                rectangle.bottom = Add(Scale(height, logicalHeight, Height(services)), rectangle.top);
                static_cast<void>(services.platform.OffsetRect(&rectangle, dx, dy));

                selected = Resolve<EditWindowNativeBody98h>(*cursor);
                const auto* defaults = static_cast<const volatile RECT*>(&selected->marginsAt5C);
                const RECT captured{defaults->left, defaults->top, defaults->right, defaults->bottom};
                void* raw = services.allocations.Allocate(0x10);
                margins = raw ? ::new(raw) RECT{} : nullptr;
                // 487C65先发布裸base；18h分配失败不取得rectangle的回收资格。
                temporary.slot.biasAt04 = 0;
                temporary.slot.baseAt00 = reinterpret_cast<std::uintptr_t>(margins);
                void* storage = services.allocations.Allocate(0x18);
                edi = storage ? ::new(storage) ViewportRectangleControl(margins) : nullptr;
                edi->referenceCountAt04 = 1;
                edi->owningAt08 = 1;
                edi->strideAt0C = 0x10;
                edi->countAt10 = 1;
                temporary.slot.controlAt08 = edi;
                margins->left = Scale(width, captured.left, Width(services));
                margins->top = Scale(height, captured.top, Height(services));
                margins->right = Scale(width, captured.right, Width(services));
                // 488020最后一次不是内联getter，但仍是独立4180C0捕获。
                margins->bottom = Scale(height, captured.bottom, Height(services));
            }
            // mode0不缩放、不OffsetRect、不分配；488061此时才固定4880F0的receiver。
            selected = Resolve<EditWindowNativeBody98h>(*cursor);
            ApplyEditWindowNativeLayoutAt4880F0(*selected, &rectangle, margins, services.maps);
            temporary.Finish(edi);
        }
    next:
        // 原ESI只加0Ch，end回读固定receiver；不重新定位begin或拒绝头/槽重入。
        cursor = reinterpret_cast<YtlReferenceSlotRuntime*>(reinterpret_cast<std::uintptr_t>(cursor) + 0x0Cu);
    } while (cursor != Read(receiver.end));
}
}
