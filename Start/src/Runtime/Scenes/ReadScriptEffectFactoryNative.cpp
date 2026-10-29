#include <Runtime/ReadScriptEffectFactoryNative.h>
#include <Runtime/GraphicsDirtyRectApi.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/LayerWindowNative.h>
#include <Runtime/LayerListNative.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
ReadScriptEffectNativeCallScope::ReadScriptEffectNativeCallScope(ReadScriptEffectFactoryNativeServices& s)
    : moveServices_{s.draw, s.pattern, s.packet.screen}, surface_(s.pattern.surfaces),
      packet_(s.packet), move_(moveServices_), flashShake_(s.flashShake), window_(s.layerWindow),
      layers_(DefaultEnvironmentRootMemory(), &s.packet.screen)
{
    const auto& calls = s.drawCalls;
    if (!calls.layers && !calls.layerRuntime) return;
    if (!calls.layers || !calls.layerRuntime)
        throw std::logic_error("ReadScript effect draw scope requires paired layer services");
    // 原46A980/46AD20直接经七层虚槽访问全局服务。仅借绑定，不额外取根/冻结Draw正文。
    postLayers_.emplace(*calls.layers,*calls.layerRuntime,calls.graphics,calls.gui);
}

namespace
{
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

using ServicesScope = ReadScriptEffectNativeCallScope;
struct Temporary final
{
    YtlReferenceSlotRuntime& value;
    bool active = true;
    ~Temporary() noexcept { if (active) ReleaseYtlReference(value); }
    void ReleaseNormal() noexcept
    {
        // 4A7BF0 state1/3及同类工厂的正常尾为terminate；失败不再次清理。
        active = false;
        ReleaseYtlReference(value);
    }
};
void CapturePrevious(ReadScriptEffectOwnerNative& owner, ReadScriptEffectFactoryNativeServices& s)
{
    // 4ACDD0不读ECX，但caller的413BE0/Draw Ref解析不可省略。
    (void)s.draw.CaptureDrawManagerAfter413BE0();
    YtlReferenceSlotRuntime captured;
    CaptureGraphicsScreenAt4ACDD0(captured, s.pattern.surfaces, s.sizes, s.transition);
    Temporary cleanup{captured};
    CopyAssignGraphicsSurfaceReferenceAt465870(owner.PreviousAt34(), captured);
    cleanup.ReleaseNormal();
}
void Propagate(ReadScriptEffectFactoryNativeServices& s)
{
    auto* manager = s.draw.CaptureDrawManagerAfter413BE0();
    auto* window = reinterpret_cast<LayerWindowNativeBody4Ch*>(DrawLayerAt418C10(*manager, 0));
    PropagateLayerWindowDirtyNativeAt4C4250(*window);
}
void Composite(ReadScriptEffectOwnerNative& owner, ReadScriptEffectFactoryNativeServices& s, int alpha)
{
    auto* manager = s.draw.CaptureDrawManagerAfter413BE0();
    YtlReferenceSlotRuntime captured;
    CompositeGraphicsFrameAt4A8340(captured, s.pattern.surfaces, s.sizes, DrawLayersView(manager), alpha);
    Temporary cleanup{captured};
    CopyAssignGraphicsSurfaceReferenceAt465870(owner.SourceAt28(), captured);
    cleanup.ReleaseNormal();
}
}
void CreateReadScriptBlendNativeAt4A7BF0(ReadScriptEffectOwnerNative& owner,
    ReadScriptEffectFactoryNativeServices& s)
{
    ServicesScope scope(s);
    CapturePrevious(owner, s);
    if (!owner.PreviousAt34().baseAt00) throw ErrorAt5B53DC;
    Propagate(s);
    Composite(owner, s, 0);
    if (!owner.SourceAt28().baseAt00) throw ErrorAt5B5404;
    s.dirty.ClearDirtyRectIfLoaded();
    auto* created = NewReadScriptBlendNative(owner.SourceAt28(), owner.PreviousAt34(), s.environment);
    owner.InstallEffect(created, 0x3C);
    if (!owner.ReadyEffectAtVslot08()) throw ErrorAt5B542C;
}
void CreateReadScriptMosaicNativeAt4B45C0(ReadScriptEffectOwnerNative& owner,
    ReadScriptEffectFactoryNativeServices& s)
{
    ServicesScope scope(s);
    auto* work = NewGraphicsSurfaceAt40C2E0(s.pattern.surfaces, s.pattern.strings);
    AdoptGraphicsSurfaceAt407A80(owner.AuxiliaryAt40(), work);
    const auto size = s.sizes.ReadScreenSizeAt47CC00();
    auto* target = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(owner.AuxiliaryAt40());
    if (!target->CreateAt415010(s.pattern.surfaces, size.cx, size.cy, 0, 0)) throw ErrorAt5B55A0;
    CapturePrevious(owner, s);
    if (!owner.PreviousAt34().baseAt00) throw ErrorAt5B55C8;
    Propagate(s);
    Composite(owner, s, 0);
    if (!owner.SourceAt28().baseAt00) throw ErrorAt5B55F0;
    s.dirty.ClearDirtyRectIfLoaded();
    auto* created = NewReadScriptMosaicNative(owner.SourceAt28(), owner.PreviousAt34(),
        owner.AuxiliaryAt40(), s.environment);
    owner.InstallEffect(created, 0x3C);
    if (!owner.ReadyEffectAtVslot08()) throw ErrorAt5B5618;
}
void CreateReadScriptTransitNativeAt4C3650(ReadScriptEffectOwnerNative& owner, std::int32_t opcode,
    ReadScriptEffectFactoryNativeServices& s)
{
    ServicesScope scope(s);
    if (opcode >= 61) throw ErrorAt5B551C;
    Propagate(s);
    Composite(owner, s, 0);
    if (!owner.SourceAt28().baseAt00) throw ErrorAt5B5554;
    s.dirty.ClearDirtyRectIfLoaded();
    auto* created = NewReadScriptTransitNative(owner.SourceAt28(), opcode, s.environment);
    owner.InstallEffect(created, 0x3C);
    if (!owner.ReadyEffectAtVslot08()) throw ErrorAt5B557C;
}
void CreateReadScriptUniversalNativeAt4C3A20(ReadScriptEffectOwnerNative& owner, const char* name,
    ReadScriptEffectFactoryNativeServices& s, IReadScriptEffectMaskNativeServices& masks)
{
    ServicesScope scope(s);
    auto* manager = s.draw.CaptureDrawManagerAfter413BE0();
    if (!masks.LoadMaskAt480B30(manager, owner.AuxiliaryAt40(), name)) throw ErrorAt5B544C;
    CapturePrevious(owner, s);
    if (!owner.PreviousAt34().baseAt00) throw ErrorAt5B5488;
    Propagate(s);
    Composite(owner, s, 1);
    if (!owner.SourceAt28().baseAt00) throw ErrorAt5B54BC;
    s.dirty.ClearDirtyRectIfLoaded();
    auto* created = NewReadScriptUniversalNative(owner.SourceAt28(), owner.PreviousAt34(),
        owner.AuxiliaryAt40(), s.environment);
    owner.InstallEffect(created, 0x3C);
    if (!owner.ReadyEffectAtVslot08()) throw ErrorAt5B54F0;
}
}
