#pragma once
#include <Runtime/ToolDialogNative.h>
#include <Runtime/LayerWindowNativeBase.h>
#include <Runtime/LayerListNative.h>
#include <Runtime/DrawLayerReferenceArray.h>
#include <Runtime/ToolWindowLayerRuntime.h>
#include <Runtime/ScriptNamedValueNative.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class IScrVmCommandErrorSink;
struct ToolWindowNativeBody40h final
{
    LayerWindowNativeBody24h window;
    union { DrawLayerReferenceArray dialogsAt24; };
    std::uint8_t valueAt30;
    std::uint8_t unwrittenAt31[3];
    std::int32_t valueAt34;
    std::uint8_t valueAt38;
    std::uint8_t unwrittenAt39[3];
    std::int32_t selectedAt3C;
    ToolWindowNativeBody40h() noexcept {}
};
static_assert(sizeof(ToolWindowNativeBody40h) == 0x40);
static_assert(offsetof(ToolWindowNativeBody40h, dialogsAt24) == 0x24);
static_assert(offsetof(ToolWindowNativeBody40h, valueAt30) == 0x30);
static_assert(offsetof(ToolWindowNativeBody40h, valueAt34) == 0x34);
static_assert(offsetof(ToolWindowNativeBody40h, valueAt38) == 0x38);
static_assert(offsetof(ToolWindowNativeBody40h, selectedAt3C) == 0x3C);
static_assert(std::is_standard_layout_v<ToolWindowNativeBody40h>);
static_assert(std::is_trivially_destructible_v<ToolWindowNativeBody40h>);

class EnvironmentToolWindowReplayNativeSource final : public IToolWindowReplaySource
{
public:
    EnvironmentToolWindowReplayNativeSource(EnvironmentRootOwner&, RootRuntimeOwner&,
        ScriptNamedValueNativeTables, IScrVmCommandErrorSink&,
        IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource()) noexcept;
    std::int32_t ReadReplayAt417B50() override;
private:
    EnvironmentRootOwner& environment_;
    RootRuntimeOwner& root_;
    ScriptNamedValueNativeTables names_;
    IScrVmCommandErrorSink& errors_;
    IRuntimeStringManagerSource& strings_;
};
struct ToolWindowNativeServices final
{
    const ToolDialogNativeProfile& profile;
    const ToolDialogNativeRelation& relation;
    IGuiControlResourceLoadServices& resources;
    IToolWindowReplaySource& replay;
    IGraphicsSurfaceApi& surfaces;
    IRuntimeStringManagerSource& strings;
    IEnvironmentRootMemory& memory;
};
class ToolWindowNativeScope final
{
public:
    explicit ToolWindowNativeScope(ToolWindowNativeServices&) noexcept;
    ~ToolWindowNativeScope() noexcept;
    ToolWindowNativeScope(const ToolWindowNativeScope&) = delete;
    ToolWindowNativeScope& operator=(const ToolWindowNativeScope&) = delete;
private:
    ToolWindowNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    LayerListNativeScope layers_;
    ToolDialogNativeScope dialog_;
};
const LayerItemNativeFrameVtable& ToolWindowNativeMethods() noexcept;
ToolWindowNativeBody40h* ConstructToolWindowNativeAt45A4B0(ToolWindowNativeBody40h&,
    LayerListNativeBody18h*, ToolWindowNativeServices&);
ToolWindowNativeBody40h* DeleteToolWindowNativeAt468DE0(ToolWindowNativeBody40h&, std::uint32_t) noexcept;
YtlReferenceSlotRuntime* SelectToolDialogNativeAt417B50(ToolWindowNativeBody40h&,
    YtlReferenceSlotRuntime& freshOutput, IToolWindowReplaySource&);
void EnableToolWindowNativeAt418F70(ToolWindowNativeBody40h&, std::uint8_t, IToolWindowReplaySource&);
}
