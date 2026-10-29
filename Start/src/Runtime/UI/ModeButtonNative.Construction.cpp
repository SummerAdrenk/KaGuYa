#include "ModeButtonNative.Internal.h"
#include <exception>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace ModeButtonNativeDetail;
struct ConstructionUnwind final
{
    Body& body;
    std::int32_t state = 2;
    bool active = true;
    ~ConstructionUnwind() noexcept
    {
        if (!active) return;
        // 5EE698 state3/4动作541E55，不能把清空阶段异常变成可恢复bad_alloc。
        if (state == 3 || state == 4) std::terminate();
        // state2 -> 1 -> 0，原EH各叶均为noexcept析构，次序与正常析构不同。
        DestroyModeButtonNavigationNativeAt4A3AB0(body.navigationAt30);
        ReleaseGuiDirtyRectReferenceAt460500(body.dirtyAt24);
        DestroyModeButtonChildrenNativeAt4A3A00(body.childrenAt14);
    }
};
}
ModeButtonNativeBody50h* ConstructModeButtonNativeAt4A2E60(
    ModeButtonNativeBody50h& body, ModeButtonNativeServices& services)
{
    ModeButtonNativeScope scope(services);
    body.vptrAt00 = &IBtnProcNativeVtable();
    body.childrenAt14 = {};
    ::new (static_cast<void*>(&body.dirtyAt24)) YtlReferenceSlotRuntime{};
    body.navigationAt30 = {};
    body.releaseStateAt10 = 0;
    ConstructionUnwind unwind{body};
    unwind.state = 3;
    ClearModeButtonChildrenNativeAt4A8490(body);
    body.selectedAt04 = -1;
    body.directionAt08 = -1;
    body.confirmLatchAt0C = 0;
    body.neighborsAt20 = nullptr;
    unwind.state = 4;
    auto* last = Read(body.navigationAt30.end);
    auto* first = Read(body.navigationAt30.begin);
    ReleaseModeButtonNavigationRangeNativeAt498CA0(first, last);
    body.navigationAt30.end = Read(body.navigationAt30.begin);
    unwind.state = 2;
    void* storage = services.memory.AllocateBody(0x80);
    auto* dirty = storage ? ::new (storage) GuiDirtyRectNativeBody80h : nullptr;
    // 原498860先发布正文，再分配control；control失败不另建owner回滚正文。
    AdoptGuiDirtyRectReferenceAt498860(body.dirtyAt24, dirty, services.memory);
    body.vptrAt00 = &IModeBtnProcNativeVtable();
    ::new (static_cast<void*>(&body.publicGuiAt44)) YtlReferenceSlotRuntime{};
    unwind.active = false;
    return &body;
}
}
