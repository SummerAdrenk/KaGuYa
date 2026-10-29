#include <Runtime/ReadScriptUpdateEffectNative.h>
#include <Runtime/ScriptNativeBody.h>
#include <Runtime/LayerWindowNative.h>

#include <new>

namespace KaGuYa::Start::Runtime
{
ReadScriptUpdateEffectNative::ReadScriptUpdateEffectNative(ReadScriptEffectOwnerNative& owner,
    ReadScriptEffectFactoryNativeServices& services, IReadScriptEffectMaskNativeServices& masks,
    IMoveEffectNativeConstructionSource& source, IReadScriptFlashShakeNativeSource& flashShake) noexcept
    : owner_(owner), services_(services), masks_(masks), source_(source), flashShake_(flashShake) {}

bool ReadScriptUpdateEffectNative::PrepareLayersAt46A980()
{
    ReadScriptEffectNativeCallScope scope(services_);
    // 4C4C82外层捕获不能被46A980内部七次捕获代替。
    (void)services_.draw.CaptureDrawManagerAfter413BE0();
    return PrepareDrawLayersAt46A980(services_.draw, 1) != 0;
}
void ReadScriptUpdateEffectNative::RestorePatternsAt4C4330()
{
    ReadScriptEffectNativeCallScope scope(services_);
    auto* manager = services_.draw.CaptureDrawManagerAfter413BE0();
    auto* window = reinterpret_cast<LayerWindowNativeBody4Ch*>(DrawLayerAt418C10(*manager, 0));
    RestoreLayerWindowPatternsNativeAt4C4330(*window, services_.pattern);
}
std::uint16_t ReadScriptUpdateEffectNative::ReadSelector()
{
    // 4C47AC/4C4D38：两个调用点都新取Root/Script，再解当前effect Ref。
    auto* script = source_.CaptureScriptAfter413BE0();
    const auto* effect = ResolveEnvironmentObjectReference<ScriptEffectNative>(script->effectAt0C);
    return static_cast<const volatile std::uint16_t&>(effect->status);
}
const char* ReadScriptUpdateEffectNative::ReadResourceName()
{
    // 4C4DC5是第三次取根；借实际CString字符槽，不从先前selector读取缓存对象。
    auto* script = source_.CaptureScriptAfter413BE0();
    auto* effect = ResolveEnvironmentObjectReference<ScriptEffectNative>(script->effectAt0C);
    return effect->resourceNameAt04.GetString();
}
void ReadScriptUpdateEffectNative::CreateBlend()
    { CreateReadScriptBlendNativeAt4A7BF0(owner_, services_); }
void ReadScriptUpdateEffectNative::CreateMosaic()
    { CreateReadScriptMosaicNativeAt4B45C0(owner_, services_); }
void ReadScriptUpdateEffectNative::CreateUniversal(const char* name)
    { CreateReadScriptUniversalNativeAt4C3A20(owner_, name, services_, masks_); }
void ReadScriptUpdateEffectNative::CreateTransit(std::int32_t opcode)
    { CreateReadScriptTransitNativeAt4C3650(owner_, opcode, services_); }
void ReadScriptUpdateEffectNative::CreateMove()
{
    ReadScriptEffectNativeCallScope scope(services_);
    void* storage = ::operator new(0x24);
    MoveEffectNativeBody24h* created = nullptr;
    try
    {
        if (storage)
        {
            created = ::new(storage) MoveEffectNativeBody24h;
            ConstructMoveEffectNativeAt49C0F0(*created, source_, services_.pattern);
        }
    }
    catch (...) { ::operator delete(storage, 0x24); throw; }
    // 完成正文构造后才退役旧Effect；安装失败不额外回收已完成候选。
    owner_.InstallEffect(created, 0x24);
}
void ReadScriptUpdateEffectNative::CreateFlash(const char* name, std::int32_t count)
{
    ReadScriptEffectNativeCallScope scope(services_);
    auto* created = NewReadScriptFlashNative(name, count, flashShake_, services_.flashShake);
    owner_.InstallEffect(created, 0x34);
}
void ReadScriptUpdateEffectNative::CreateShake(std::int32_t mode, std::int32_t scale, std::int32_t interval)
{
    ReadScriptEffectNativeCallScope scope(services_);
    auto* created = NewReadScriptShakeNative(mode, scale, interval, flashShake_, services_.flashShake);
    owner_.InstallEffect(created, 0x28);
}
bool ReadScriptUpdateEffectNative::HasEffect() const { return owner_.HasEffectAt1C(); }
std::uint8_t ReadScriptUpdateEffectNative::ReadyEffect() const { return owner_.ReadyEffectAtVslot08(); }
void ReadScriptUpdateEffectNative::ReleaseResources()
{
    ReadScriptEffectNativeCallScope scope(services_);
    owner_.ReleaseResourcesAt4C0990();
}
}
