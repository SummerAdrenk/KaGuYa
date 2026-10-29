#pragma once
#include <Platform/InputViews.h>

#include <Runtime/LayerWindowNativeBase.h>
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Platform
{
class KeyboardView;
class MouseView;
class IInputRuntime;
}

namespace KaGuYa::Start::Runtime
{
class IDrawManagerNativeRootSource;
class IGuiControlResourceLoadServices;
class IGraphicsSurfaceApi;
class IRuntimeStringManagerSource;
struct OpacityDialogNativeProfile;

struct OpacityWindowNativeBody30h final
{
    LayerWindowNativeBody24h window;
    union { YtlReferenceSlotRuntime dialogAt24; };
    OpacityWindowNativeBody30h() noexcept {}
};
static_assert(sizeof(OpacityWindowNativeBody30h) == 0x30);
static_assert(alignof(OpacityWindowNativeBody30h) == 4);
static_assert(offsetof(OpacityWindowNativeBody30h, window) == 0);
static_assert(offsetof(OpacityWindowNativeBody30h, dialogAt24) == 0x24);
static_assert(std::is_standard_layout_v<OpacityWindowNativeBody30h>);
static_assert(std::is_trivially_destructible_v<OpacityWindowNativeBody30h>);

const LayerItemNativeFrameVtable& OpacityWindowNativeMethods() noexcept;
OpacityWindowNativeBody30h* ConstructOpacityWindowNativeAt45AE70(OpacityWindowNativeBody30h& body,
    LayerListNativeBody18h* parent, IGuiControlResourceLoadServices& loader,
    IGraphicsSurfaceApi& surfaces, IRuntimeStringManagerSource& strings);
OpacityWindowNativeBody30h* ConstructOpacityWindowNativeAt45AE70(OpacityWindowNativeBody30h& body,
    LayerListNativeBody18h* parent, IGuiControlResourceLoadServices& loader,
    IGraphicsSurfaceApi& surfaces, IRuntimeStringManagerSource& strings,
    const OpacityDialogNativeProfile& profile);
OpacityWindowNativeBody30h* DeleteOpacityWindowNativeAt468F20(OpacityWindowNativeBody30h& body,
    std::uint32_t flags) noexcept;
void PropagateOpacityDirtyAt4ABA60(OpacityWindowNativeBody30h& body);

// 输入仍是正文外的明确平台边界。原4BE534与4BE586分别取当前输入，不能预存一帧快照。
class IOpacityWindowInputSource
{
public:
    virtual ~IOpacityWindowInputSource() = default;
    virtual Platform::KeyboardView CaptureKeyboardAt418210() = 0;
    virtual Platform::MouseView CaptureMouseAt418210() = 0;
};
// 与GuiModalInputSource相同的显式平台桥，不宣称24h输入singleton已原生化。
class OpacityWindowInputAggregateSource final : public IOpacityWindowInputSource
{
public:
    explicit OpacityWindowInputAggregateSource(const Platform::IInputRuntime& input) noexcept
        : input_(input) {}
    Platform::KeyboardView CaptureKeyboardAt418210() override;
    Platform::MouseView CaptureMouseAt418210() override;
private:
    const Platform::IInputRuntime& input_;
};
bool UpdateOpacityWindowAt4BE530(OpacityWindowNativeBody30h& body, IOpacityWindowInputSource& input);

// 原始30h分配在首个Ensure前；完成正文后第二次取根，append失败不得补删完成正文。
OpacityWindowNativeBody30h* CreateAndAppendOpacityWindowAt48A9EC(IDrawManagerNativeRootSource& source,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
OpacityWindowNativeBody30h* CreateAndAppendOpacityWindowAt48A9EC(IDrawManagerNativeRootSource& source,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory,
    const OpacityDialogNativeProfile& profile);
}
