#pragma once

#include <Runtime/LegacyTitleMenu.h>
#include <Runtime/SceneControllerNative.h>

namespace KaGuYa::Start::Runtime
{
struct LegacyTitleSceneData final
{
    const LegacyTitleMenuData& menu;
    const LegacyTitleMenuNames& names;
    const char* blackName;
    const char* bgmName;
};

class ILegacyTitleSceneHost : public ILegacyTitleWorkflowHost
{
public:
    virtual const char* BlackSurfaceName() = 0;
    virtual bool LoadBlackSurface(YtlReferenceSlotRuntime&, const char* nameCp932) = 0;
    // 49C8F0/49C8FD分别取根；高度先于宽度，不缓存一份系统对象。
    virtual std::int32_t ScreenHeight() = 0;
    virtual std::int32_t ScreenWidth() = 0;
    virtual void ClearReplay() = 0;
};

struct LegacyTitleSceneServices final
{
    ILegacyTitleSceneHost& host;
    const LegacyTitleMenuData& menuData;
    LegacyTitleMenuServices& menu;
    ILegacyTitleAudio& audio;
    ILegacyTitleNamePrompt& names;
    ILegacyTitleQuickLoad& quickLoad;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsScreenCalls& screen;
    IRuntimeStringManagerSource& strings;
    IEnvironmentRootMemory& memory;
};

// 内部统一模型：只保留调度器要求的双槽ITask前缀，不复制原CTitle的60h布局。
// 服务必须覆盖此对象寿命；frame/black/menu均是唯一拥有的YTL槽。
struct LegacyTitleScene final
{
    TaskNativeBody4h task{};
    LegacyTitleSceneServices* services = nullptr;
    LegacyTitleWorkflowState workflow;
    YtlReferenceSlotRuntime menu;
    YtlReferenceSlotRuntime frame;
    YtlReferenceSlotRuntime black;
    bool retired = false;
};
static_assert(offsetof(LegacyTitleScene, task) == 0);

// factory必须按sizeof(LegacyTitleScene)分配，不得继续使用现代Title的1Ch。
LegacyTitleScene* ConstructLegacyTitleScene(LegacyTitleScene&, LegacyTitleSceneServices&);
LegacyTitleScene* NewLegacyTitleScene(LegacyTitleSceneServices&);
void ReleaseLegacyTitleScene(LegacyTitleScene&);
const TaskNativeVtable& LegacyTitleSceneMethods() noexcept;
}
