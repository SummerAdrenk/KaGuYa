#include "ReadScriptSceneNative.Internal.h"
#include <bit>
#include <exception>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace ReadScriptSceneNativeDetail;
thread_local ReadScriptSceneNativeServices* active = nullptr;
struct ConstructionUnwind final
{
    Body& body;
    bool scroll = false;
    bool text = false;
    bool armed = true;
    ~ConstructionUnwind() noexcept
    {
        if (!armed) return;
        // state7 -> 6 -> 4 -> 3 -> 2 -> 1 -> 0；不调完整析构/4C0990/End。
        if (text) DestroyRuntimeCStringAt40CD40(&body.errorMessageAtB0);
        if (scroll) DestroyFreeScrollNativeAt4A3B60(body.scrollAt60);
        UnwindReadScriptEffectSlotsAt4A14D0(body.effectsAt1C);
        RetireTask(body);
    }
};
}
namespace ReadScriptSceneNativeDetail
{
ReadScriptSceneNativeServices& ActiveServices()
{
    if (!active) throw std::logic_error("ReadScript native raw call requires an external service scope");
    return *active;
}
}
ReadScriptSceneNativeScope::ReadScriptSceneNativeScope(ReadScriptSceneNativeServices& services) noexcept
    : previous_(active) { active = &services; }
ReadScriptSceneNativeScope::~ReadScriptSceneNativeScope() noexcept { active = previous_; }

Body* ConstructReadScriptSceneNativeAt4A14D0(Body& body, ReadScriptSceneNativeServices& services)
{
    ReadScriptSceneNativeScope scope(services);
    EffectCallScope effects(services);
    body.vptrAt00 = &ReadScriptSceneNativeMethods();
    ::new (&body.effectsAt1C) ReadScriptEffectSlotsNative30h;
    InitializeReadScriptEffectSlotsAt4A14D0(body.effectsAt1C);
    ConstructionUnwind unwind{body};
    ::new (&body.scrollAt60) FreeScrollNativeBody50h;
    // helper内部负责state5；返回后才取得state6整成员清理资格。
    ConstructFreeScrollNativeAt4A1559(body.scrollAt60, services.sizes);
    unwind.scroll = true;
    InitializeEmptyRuntimeCString(body.errorMessageAtB0, services.strings);
    unwind.text = true;
    ReleaseReadScriptEffectSlotsAt4C0990(body.effectsAt1C);
    RestoreReadScriptSceneNativeBodyAt4A1646(body, services.restore);
    unwind.armed = false;
    return &body;
}
void RestoreReadScriptSceneNativeBodyAt4A1646(Body& body, IReadScriptSceneRestoreSource& source)
{
    body.drawEnabledAt08 = 1;
    body.mainStateAt04 = 0;
    body.substateAt0C = 0;
    if ((source.ReadScriptFlagsAfter413BE0() & 2u) != 0)
    {
        switch (source.ReadSceneMarkerAfter413BE0())
        {
        case 0:
            body.mainStateAt04 = 3;
            body.substateAt0C = 1;
            source.CaptureToolAfter413BE0().SetModeAt434FB0(0);
            source.CaptureToolAfter413BE0().WriteLatchByteAt38(0);
            body.interactionValueAt10 = std::bit_cast<std::int32_t>(source.ReadUnSaveModeAt4180C0());
            break;
        case 1: body.mainStateAt04 = 4; break;
        case 2: body.mainStateAt04 = 7; break;
        default: break;
        }
    }
    source.ClearScriptFlagsAndReloadAfter413BE0();
}
void DestroyReadScriptSceneNativeAt4A4920(Body& body, ReadScriptSceneNativeServices& services) noexcept
{
    ReadScriptSceneNativeScope scope(services);
    EffectCallScope effects(services);
    body.vptrAt00 = &ReadScriptSceneNativeMethods();
    // 源PE FuncInfo5F099C EHFlags=5：任何叶异常立即terminate，不续清后缀。
    ReleaseReadScriptEffectSlotsAt4C0990(body.effectsAt1C);
    DestroyRuntimeCStringAt40CD40(&body.errorMessageAtB0);
    DestroyFreeScrollNativeAt4A3B60(body.scrollAt60);
    DestroyReadScriptEffectSlotMembersAt4A4920(body.effectsAt1C);
    RetireTask(body);
}
Body* DeleteReadScriptSceneNativeAt4A6F60(Body* body, std::uint32_t flags,
    ReadScriptSceneNativeServices& services) noexcept
{
    DestroyReadScriptSceneNativeAt4A4920(*body, services);
    if ((flags & 1u) != 0) ::operator delete(body, std::size_t{0xBC});
    return body;
}
}
