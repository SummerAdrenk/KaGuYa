#include <Runtime/ReadScriptEffectFactoryRuntime.h>

#include <Runtime/DrawManagerRuntime.h>
#include <Runtime/GraphicsDirtyRectApi.h>
#include <Runtime/LayerWindowRuntime.h>
#include <Runtime/ReadScriptEffectSurfaceRuntime.h>
#include <Runtime/ReadScriptFlashShakeEffects.h>
#include <Runtime/ReadScriptMoveEffectRuntime.h>
#include <Runtime/ReadScriptPacketEffects.h>
#include <Runtime/ReadScriptTransitionMaskLoader.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原 PE 5B53DC..5B5618 的 NUL 结尾 CP932 字节；必须抛 char*，不是 const char*。
char ErrorAt5B53DC[] = "\x95\x5C\x8E\xA6\x89\xE6\x91\x9C\x82\xCC\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73" " : [Blend Effect]";
char ErrorAt5B5404[] = "\x8D\x58\x90\x56\x89\xE6\x91\x9C\x82\xCC\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73" " : [Blend Effect]";
char ErrorAt5B542C[] = "\x8F\x89\x8A\xFA\x90\xDD\x92\xE8\x82\xC9\x8E\xB8\x94\x73" " : [Blend Effect]";
char ErrorAt5B544C[] = "\x83\x47\x83\x74\x83\x46\x83\x4E\x83\x67\x89\xE6\x91\x9C\x82\xCC\x93\xC7\x82\xDD\x8D\x9E\x82\xDD\x82\xC9\x8E\xB8\x94\x73" " : [Universal Transit Effect]";
char ErrorAt5B5488[] = "\x95\x5C\x8E\xA6\x89\xE6\x91\x9C\x82\xCC\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73" " : [Universal Transit Effect]";
char ErrorAt5B54BC[] = "\x8D\x58\x90\x56\x89\xE6\x91\x9C\x82\xCC\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73" " : [Universal Transit Effect]";
char ErrorAt5B54F0[] = "\x8F\x89\x8A\xFA\x90\xDD\x92\xE8\x82\xC9\x8E\xB8\x94\x73" " : [Universal Transit Effect]";
char ErrorAt5B551C[] = "\x83\x67\x83\x89\x83\x93\x83\x57\x83\x56\x83\x87\x83\x93\x82\xCC\x8E\xED\x97\xDE\x82\xAA\x82\xA0\x82\xE8\x82\xDC\x82\xB9\x82\xF1\x81\x49" " : [Transit Effect]";
char ErrorAt5B5554[] = "\x8D\x58\x90\x56\x89\xE6\x91\x9C\x82\xCC\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73" " : [Transit Effect]";
char ErrorAt5B557C[] = "\x8F\x89\x8A\xFA\x90\xDD\x92\xE8\x82\xC9\x8E\xB8\x94\x73" " : [Transit Effect]";
char ErrorAt5B55A0[] = "\x89\xC1\x8D\x48\x97\xCC\x88\xE6\x82\xCC\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73" " : [Mosaic Effect]";
char ErrorAt5B55C8[] = "\x95\x5C\x8E\xA6\x89\xE6\x91\x9C\x82\xCC\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73" " : [Mosaic Effect]";
char ErrorAt5B55F0[] = "\x8D\x58\x90\x56\x89\xE6\x91\x9C\x82\xCC\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73" " : [Mosaic Effect]";
char ErrorAt5B5618[] = "\x8F\x89\x8A\xFA\x90\xDD\x92\xE8\x82\xC9\x8E\xB8\x94\x73" " : [Mosaic Effect]";

void ReplaceSurface(std::shared_ptr<RuntimeGraphicsSurface>& slot,
    std::shared_ptr<RuntimeGraphicsSurface> incoming)
{
    // 465870 先释放旧 Ref 再提交新对象；incoming 是原返回临时 Ref，不保活旧图像。
    // shared_ptr 的普通赋值会先让槽可见为新指针，再释放旧对象，故分开执行。
    slot.reset();
    slot = std::move(incoming);
}
}

ReadScriptEffectFactoryRuntime::ReadScriptEffectFactoryRuntime(
    ReadScriptEffectFactoryDependencies dependencies) noexcept : dependencies_(dependencies)
{
}

bool ReadScriptEffectFactoryRuntime::PrepareLayersAt46A980()
{
    return dependencies_.manager.Layers().PrepareLayersAt46A980(1) != 0;
}

LayerWindowRuntime& ReadScriptEffectFactoryRuntime::WindowZero() const
{
    auto* window = dynamic_cast<LayerWindowRuntime*>(dependencies_.manager.Layers().LayerAt418C10(0));
    if (!window) throw std::logic_error("ReadScript effect factory requires the actual LayerWindow at slot 0");
    return *window;
}

void ReadScriptEffectFactoryRuntime::RestorePatternsAt4C4330()
{
    WindowZero().RestorePatternsAt4C4330(dependencies_.drawing);
}

std::shared_ptr<RuntimeGraphicsSurface> ReadScriptEffectFactoryRuntime::CompositeAt4A8340(bool alpha)
{
    // 4A8379..4A83F4：先创建临时 Ref 的对象，再读 DLL 尺寸；clear 固定为 0。
    auto surface = std::make_shared<RuntimeGraphicsSurface>(dependencies_.surfaces);
    const SIZE size = dependencies_.sizes.ReadScreenSizeAt47CC00();
    if (!surface->Create(size.cx, size.cy, alpha, false)) return {};
    // 4A8476：只有缓冲创建成功才走真实注册列表 +1C；异常时不提交 source 槽。
    dependencies_.manager.Layers().RegisteredLayers().CompositeToSurface(
        GraphicsSurfaceDestination::FromNative(surface->NativeBody()), nullptr);
    return surface;
}

void ReadScriptEffectFactoryRuntime::CreateBlendAt4A7BF0(ReadScriptEffectOwnerRuntime& owner)
{
    ReplaceSurface(owner.PreviousAt34(), dependencies_.resources.CaptureScreenAt4ACDD0());
    if (!owner.PreviousAt34()) throw ErrorAt5B53DC;
    WindowZero().PropagateDirtyRectsAt4C4250();
    ReplaceSurface(owner.SourceAt28(), CompositeAt4A8340(false));
    if (!owner.SourceAt28()) throw ErrorAt5B5404;
    dependencies_.dirty.ClearDirtyRectIfLoaded();
    owner.InstallEffect(CreateReadScriptBlendEffect(dependencies_.manager, dependencies_.screen,
        dependencies_.save, owner.SourceAt28().get(), owner.PreviousAt34().get()));
    if (!owner.ReadyEffectAtVslot08()) throw ErrorAt5B542C;
}

void ReadScriptEffectFactoryRuntime::CreateMosaicAt4B45C0(ReadScriptEffectOwnerRuntime& owner)
{
    // 4B461A 在尺寸查询/缓冲创建之前覆盖 +40；失败保留新工作对象（可能为零句柄）。
    ReplaceSurface(owner.AuxiliaryAt40(), std::make_shared<RuntimeGraphicsSurface>(dependencies_.surfaces));
    const SIZE size = dependencies_.sizes.ReadScreenSizeAt47CC00();
    if (!owner.AuxiliaryAt40()->Create(size.cx, size.cy, false, false)) throw ErrorAt5B55A0;
    ReplaceSurface(owner.PreviousAt34(), dependencies_.resources.CaptureScreenAt4ACDD0());
    if (!owner.PreviousAt34()) throw ErrorAt5B55C8;
    WindowZero().PropagateDirtyRectsAt4C4250();
    ReplaceSurface(owner.SourceAt28(), CompositeAt4A8340(false));
    if (!owner.SourceAt28()) throw ErrorAt5B55F0;
    dependencies_.dirty.ClearDirtyRectIfLoaded();
    owner.InstallEffect(CreateReadScriptMosaicEffect(dependencies_.manager, dependencies_.screen,
        dependencies_.save, owner.SourceAt28().get(), owner.PreviousAt34().get(), owner.AuxiliaryAt40().get()));
    if (!owner.ReadyEffectAtVslot08()) throw ErrorAt5B5618;
}

void ReadScriptEffectFactoryRuntime::CreateUniversalAt4C3A20(
    ReadScriptEffectOwnerRuntime& owner, const char* nameCp932)
{
    if (!dependencies_.masks.LoadMaskAt480B30(owner.AuxiliaryAt40(), nameCp932)) throw ErrorAt5B544C;
    ReplaceSurface(owner.PreviousAt34(), dependencies_.resources.CaptureScreenAt4ACDD0());
    if (!owner.PreviousAt34()) throw ErrorAt5B5488;
    WindowZero().PropagateDirtyRectsAt4C4250();
    ReplaceSurface(owner.SourceAt28(), CompositeAt4A8340(true));
    if (!owner.SourceAt28()) throw ErrorAt5B54BC;
    dependencies_.dirty.ClearDirtyRectIfLoaded();
    owner.InstallEffect(CreateReadScriptUniversalEffect(dependencies_.manager, dependencies_.screen,
        dependencies_.save, owner.SourceAt28().get(), owner.PreviousAt34().get(), owner.AuxiliaryAt40().get()));
    if (!owner.ReadyEffectAtVslot08()) throw ErrorAt5B54F0;
}

void ReadScriptEffectFactoryRuntime::CreateTransitAt4C3650(ReadScriptEffectOwnerRuntime& owner,
    std::int32_t opcode)
{
    if (opcode >= 61) throw ErrorAt5B551C; // 4C3679/4C367D：有符号 JGE，没有负数拒绝。
    WindowZero().PropagateDirtyRectsAt4C4250();
    ReplaceSurface(owner.SourceAt28(), CompositeAt4A8340(false));
    if (!owner.SourceAt28()) throw ErrorAt5B5554;
    dependencies_.dirty.ClearDirtyRectIfLoaded();
    owner.InstallEffect(CreateReadScriptTransitEffect(dependencies_.manager, dependencies_.screen,
        dependencies_.save, owner.SourceAt28().get(), opcode));
    if (!owner.ReadyEffectAtVslot08()) throw ErrorAt5B557C;
}

std::unique_ptr<IReadScriptEffectRuntime> ReadScriptEffectFactoryRuntime::CreateMoveAt49C0F0()
{
    return dependencies_.move.CreateMoveAt49C0F0();
}

std::unique_ptr<IReadScriptEffectRuntime> ReadScriptEffectFactoryRuntime::CreateFlashAt49BCA0(
    const std::string& nameCp932, std::int32_t count)
{
    return std::make_unique<ReadScriptFlashEffect>(dependencies_.resources, dependencies_.drawing,
        dependencies_.screen, nameCp932, count);
}

std::unique_ptr<IReadScriptEffectRuntime> ReadScriptEffectFactoryRuntime::CreateShakeAt49D4E0(
    std::int32_t mode, std::int32_t scale, std::int32_t interval)
{
    return std::make_unique<ReadScriptShakeEffect>(dependencies_.resources, dependencies_.drawing,
        dependencies_.screen, mode, scale, interval);
}
}
