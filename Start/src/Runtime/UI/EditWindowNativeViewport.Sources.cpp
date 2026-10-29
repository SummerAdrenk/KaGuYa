#include <Runtime/EditWindowNativeViewport.h>
#include <Runtime/RootRuntimeOwner.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
const YtlReferenceSlotRuntime& RootEditWindowNativeViewportSource::CaptureModeSlotAfter413BE0()
{
    root_.EnsureCreatedAt413BE0();
    // 6198C4在413BE0之后重新读取；使用合法accessor，绝不把现代payload当21Ch。
    return root_.ReadPublishedAt6198C4()->WindowModeSlotAt0C();
}
EnvironmentRootStorage* EnvironmentEditWindowNativeViewportSource::CaptureAt4180C0()
{
    // 已有实现涵盖601160 enter/lazy60h/两次417360/EH leave及正常leave后回读发布槽。
    return owner_.CaptureAt4180C0();
}
namespace
{
class PlatformApi final : public IEditWindowNativeViewportPlatformApi
{
public:
    BOOL IsWindowVisible(HWND window) override { return ::IsWindowVisible(window); }
    BOOL OffsetRect(RECT* rectangle, int dx, int dy) override { return ::OffsetRect(rectangle, dx, dy); }
};
class AllocationApi final : public IEditWindowNativeViewportAllocationApi
{
public:
    void* Allocate(std::size_t bytes) override { return ::operator new(bytes); }
};
}
IEditWindowNativeViewportPlatformApi& DefaultEditWindowNativeViewportPlatformApi() noexcept
{ static PlatformApi api; return api; }
IEditWindowNativeViewportAllocationApi& DefaultEditWindowNativeViewportAllocationApi() noexcept
{ static AllocationApi api; return api; }
void UpdateEditWindowNativeViewportAt4875F0(EditWindowNativeReferences& receiver,
    EditWindowNativeBody98h* target, EditWindowNativeViewportServices& services,
    EditWindowNativeMapServices& maps, WindowThunkNativeServices& thunks)
{
    EditWindowNativeScope scope(maps, thunks);
    EditWindowNativeViewportServices bound{services.root, services.environment, services.platform, maps.api, services.allocations};
    UpdateEditWindowNativeViewportAt4875F0(receiver, target, bound);
}
}
