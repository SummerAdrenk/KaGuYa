#include <Runtime/OpacityWindowNative.h>
#include <Runtime/OpacityDialogNative.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/GraphicsSurfaceNativeBody.h>
#include <Runtime/GuiDialogNative.h>
#include <Runtime/LayerListNative.h>
#include <Platform/Input.h>

#include <bit>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = OpacityWindowNativeBody30h;
using Base = LayerItemNativeBody14h;
using Table = LayerItemNativeFrameVtable;

Body& Derived(Base* body) noexcept { return *reinterpret_cast<Body*>(body); }
GuiDialogNativeBody5Ch* Dialog(Body& body) noexcept
{
    // 两者首部地址相同；空base保持空，原调用点的空解引用不得变成空成功。
    return reinterpret_cast<GuiDialogNativeBody5Ch*>(ResolveOpacityDialogReference(body.dialogAt24));
}
GuiDirtyRectNativeBody80h* Dirty(Body& body)
{
    auto* const dialog = Dialog(body);
    return dialog->vptrAt00->dirtyAt00(dialog);
}
void __fastcall Draw(Base* self, void*, const RECT* rect)
{
    auto* const dialog = Dialog(Derived(self));
    dialog->vptrAt00->drawAt60(dialog, rect);
}
void __fastcall Composite(Base* self, void*, GraphicsSurfaceNativeBody* target, const RECT* clip)
{
    // 481B80：先捕获receiver，451500可能重入改变外层Ref；随后只晚读已选receiver虚表。
    auto* const dialog = Dialog(Derived(self));
    YtlReferenceSlotRuntime argument;
    ConstructGraphicsSurfaceReferenceAt451500(argument, target, 0);
    dialog->vptrAt00->compositeAt5C(dialog, argument, clip);
}
Base* __fastcall Delete(Base* self, void*, std::uint32_t flags)
    { DeleteOpacityWindowNativeAt468F20(Derived(self), flags); return self; }
RECT* __fastcall Bounds(Base* self, void*, RECT* output)
    { *output = Derived(self).window.boundsAt14; return output; }
std::uint8_t __fastcall Request(Base* self, void*, std::uint8_t requested, std::int32_t)
{
    auto& body = Derived(self);
    const bool enabled = requested != 0;
    if ((body.window.layer.enabledAt0C != 0) == enabled) return 0;
    body.window.layer.enabledAt0C = enabled ? 1u : 0u;
    auto* const dialog = Dialog(body);
    dialog->vptrAt00->writeEnabledAt24(dialog, enabled ? 1 : 0);
    auto* const parent = body.window.layer.parentAt10;
    parent->vptrAt00->addDirtyAt20(parent, &body.window.boundsAt14);
    return 0;
}
void __fastcall Advance(Base*, void*) {} // 46B230本身只有RET，不执行Dialog的Update。
void __fastcall Reset(Base* self, void*)
{
    self->enabledAt0C = 0;
    auto* const dialog = Dialog(Derived(self));
    dialog->vptrAt00->beginAt44(dialog);
}
// 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
// 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const Table methods{
    {reinterpret_cast<decltype(LayerItemNativeVtable::drawAt00)>(&Draw),
     reinterpret_cast<decltype(LayerItemNativeVtable::compositeAt04)>(&Composite),
     reinterpret_cast<decltype(LayerItemNativeVtable::deletingDestructorAt08)>(&Delete)},
    reinterpret_cast<decltype(Table::boundsAt0C)>(&Bounds),
    reinterpret_cast<decltype(Table::requestAt10)>(&Request),
    reinterpret_cast<decltype(Table::advanceAt14)>(&Advance),
    reinterpret_cast<decltype(Table::resetAt18)>(&Reset)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}

const LayerItemNativeFrameVtable& OpacityWindowNativeMethods() noexcept { return methods; }
Body* ConstructOpacityWindowNativeAt45AE70(Body& body, LayerListNativeBody18h* parent,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings)
{
    return ConstructOpacityWindowNativeAt45AE70(body, parent, loader, surfaces, strings,
        MamatomoOpacityDialogNativeProfile());
}
Body* ConstructOpacityWindowNativeAt45AE70(Body& body, LayerListNativeBody18h* parent,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings, const OpacityDialogNativeProfile& profile)
{
    // 441270不在45AFE2 catch内；注册失败尚无注销资格。
    ConstructLayerItemNativeAt441270(body.window.layer, parent);
    body.window.boundsAt14 = {};
    body.window.layer.vptrAt00 = methods.AsBase();
    body.dialogAt24 = {};
    body.window.layer.enabledAt0C = 0;
    try
    {
        auto* const storage = ::operator new(std::size_t{0x6C});
        OpacityDialogNativeBody6Ch* created = nullptr;
        try
        {
            if (storage)
            {
                auto* raw = ::new (storage) OpacityDialogNativeBody6Ch;
                created = ConstructOpacityDialogNativeAt4E4780(*raw, loader, surfaces, strings, profile);
            }
        }
        catch (...)
        {
            // 596743仅释放未完成raw，不调用尚未完成的派生删除虚槽。
            ::operator delete(storage, std::size_t{0x6C});
            throw;
        }
        InstallOpacityDialogReferenceAt45AF17(body.dialogAt24, created);
        // 45AFA9在control回调后重新解析成员，不从先前created拷贝bounds。
        auto* const current = ResolveOpacityDialogReference(body.dialogAt24);
        body.window.boundsAt14 = current->boundsAt5C;
    }
    catch (...)
    {
        // 原catch只清RECT；已发布Ref、孤立base及注册状态按失败前缀保留。
        body.window.boundsAt14 = {};
    }
    return &body;
}

Body* DeleteOpacityWindowNativeAt468F20(Body& body, std::uint32_t flags) noexcept
{
    ReleaseYtlReference(body.dialogAt24);
    DestroyLayerWindowNativeAt464910(body.window);
    auto* const result = &body;
    if (flags & 1u) ::operator delete(result, std::size_t{0x30});
    return result;
}

void PropagateOpacityDirtyAt4ABA60(Body& body)
{
    auto* dirty = Dirty(body);
    dirty->vptrAt00->coalesceAt04(dirty);
    dirty = Dirty(body);
    const auto count = dirty->vptrAt00->countAt18(dirty);
    if (count == 0) return;
    dirty = Dirty(body);
    const auto* const rects = dirty->vptrAt00->rectsAt1C(dirty);
    for (std::int32_t index = 0; index < std::bit_cast<std::int32_t>(count); ++index)
    {
        auto* const parent = body.window.layer.parentAt10;
        parent->vptrAt00->addDirtyAt20(parent, &rects[index]);
    }
    dirty = Dirty(body);
    dirty->vptrAt00->clearAt00(dirty);
}

bool UpdateOpacityWindowAt4BE530(Body& body, IOpacityWindowInputSource& input)
{
    const auto& keyboard = input.CaptureKeyboardAt418210();
    const auto count = keyboard.BindingCount(0);
    for (std::size_t i = 0; i < count; ++i)
        if (keyboard.CurrentState(0, i) != 0 && keyboard.PreviousState(0, i) == 0)
            return false;
    const auto& mouse = input.CaptureMouseAt418210();
    if (mouse.PreviousPressed(1) == 0 && mouse.Pressed(1) != 0 &&
        mouse.ReleaseSuppressionCountdown() == 0) return false;
    auto* const dialog = Dialog(body);
    static_cast<void>(dialog->vptrAt00->pollAt48(dialog));
    PropagateOpacityDirtyAt4ABA60(body);
    return true;
}

Platform::KeyboardView OpacityWindowInputAggregateSource::CaptureKeyboardAt418210()
{
    const auto keyboard = input_.KeyboardOrNull();
    if (!keyboard) throw std::logic_error("Opacity input keyboard slot is empty");
    return *keyboard;
}
Platform::MouseView OpacityWindowInputAggregateSource::CaptureMouseAt418210()
{
    const auto mouse = input_.MouseOrNull();
    if (!mouse) throw std::logic_error("Opacity input mouse slot is empty");
    return *mouse;
}

Body* CreateAndAppendOpacityWindowAt48A9EC(IDrawManagerNativeRootSource& source,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    return CreateAndAppendOpacityWindowAt48A9EC(source, loader, surfaces, strings, memory,
        MamatomoOpacityDialogNativeProfile());
}
Body* CreateAndAppendOpacityWindowAt48A9EC(IDrawManagerNativeRootSource& source,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory,
    const OpacityDialogNativeProfile& profile)
{
    auto* const storage = memory.AllocateBody(0x30);
    Body* created = nullptr;
    try
    {
        if (storage)
        {
            auto* const manager = source.CaptureDrawManagerAfter413BE0();
            if (!manager) throw std::logic_error("Opacity construction dereferenced a null DrawManager");
            auto* const parent = ResolveDrawLayerListAt47AE80(*manager);
            auto* const body = ::new (storage) Body;
            created = ConstructOpacityWindowNativeAt45AE70(*body, parent, loader, surfaces, strings, profile);
        }
    }
    catch (...)
    {
        // 原raw展开调用sized delete；删除服务抛出不能替换正在展开的异常。
        [&]() noexcept { memory.DeleteBody(storage, 0x30); }();
        throw;
    }
    // 48AA30撤销raw展开资格后再Ensure；此后失败不能补删完成正文。
    auto* const manager = source.CaptureDrawManagerAfter413BE0();
    if (!manager) throw std::logic_error("Opacity append dereferenced a null DrawManager");
    AppendOwnedDrawLayerAt46A880(manager->ownedLayersAt14,
        created ? &created->window.layer : nullptr, memory);
    return created;
}
}
