#pragma once
#include <Runtime/ScriptVoiceSource.h>
#include <Runtime/ScriptSoundLeafSource.h>
#include <Runtime/ReadScriptSoundWaitSource.h>

namespace KaGuYa::Start::Runtime
{
class SoundSlotManagerRuntime;
struct SoundManagerNativeBody50h;
class SoundManagerNativeVolume;
class EnvironmentSoundParamsArchive;
class EnvironmentRootOwner;
class RootRuntimeOwner;

// 只借选定manager/服务；不保存或复制nameSelectionMode策略。
struct TextSoundVolumeReceiver final
{
    void* object;
    void* service;
    void (*adjustAt4C2410)(void*, void*);
    void (*refreshAt433F60)(void*, void*);
    void Adjust() const { adjustAt4C2410(object, service); }
    void Refresh() const { refreshAt433F60(object, service); }
    static TextSoundVolumeReceiver Native(SoundManagerNativeBody50h*, SoundManagerNativeVolume&) noexcept;
    static TextSoundVolumeReceiver Modern(SoundSlotManagerRuntime*) noexcept;
};
struct TextSoundManagerReceiver final
{
    ScriptVoiceManagerReceiver voice;
    ScriptSoundManagerReceiver effects;
    TextSoundVolumeReceiver volume;
    ReadScriptSoundWaitReceiver slots;
};
struct TextSoundRootReceiver final
{
    void* object;
    TextSoundManagerReceiver (*resolveAt4188F0)(void*);
    TextSoundManagerReceiver Resolve() const { return resolveAt4188F0(object); }
};
struct TextSoundVoiceGateReceiver final
{
    using Function = std::uint8_t (__thiscall*)(void*, const YtlReferenceSlotRuntime*);
    void* object;
    Function allowedAt60;
    bool Allowed(const YtlReferenceSlotRuntime* names) const { return allowedAt60(object, names) != 0; }
    // 缓存原参数vslot+60在voices捕获之前；空对象在此读取vptr时暴露非法访问。
    static TextSoundVoiceGateReceiver Native(EnvironmentSoundParamsArchive*);
};
// 4B2C2D/4BC646先固定vtable指针，取得voices后才读+60；不同于提前缓存函数。
struct TextSoundVoiceGateTableReceiver final
{
    void* object;
    const std::uintptr_t* capturedVtable;
    static TextSoundVoiceGateTableReceiver Native(EnvironmentSoundParamsArchive*);
    bool Allowed(const YtlReferenceSlotRuntime* names) const;
};
class ITextWindowSoundSource
{
public:
    virtual ~ITextWindowSoundSource() = default;
    virtual TextSoundRootReceiver CaptureRootAfter413BE0() = 0;
    virtual TextSoundVoiceGateReceiver CaptureParametersAt4188C0() = 0;
    virtual const YtlReferenceSlotRuntime* CaptureVoicesSlotAt3C() = 0;
    virtual EnvironmentVectorStorage<RuntimeCString>* CaptureMappingsAt30() = 0;
};
class EnvironmentTextWindowSoundSource final : public ITextWindowSoundSource
{
public:
    EnvironmentTextWindowSoundSource(EnvironmentRootOwner&, RootRuntimeOwner&) noexcept;
    TextSoundRootReceiver CaptureRootAfter413BE0() override;
    TextSoundVoiceGateReceiver CaptureParametersAt4188C0() override;
    const YtlReferenceSlotRuntime* CaptureVoicesSlotAt3C() override;
    EnvironmentVectorStorage<RuntimeCString>* CaptureMappingsAt30() override;
private:
    EnvironmentRootOwner& environment_;
    RootRuntimeOwner& root_;
};
}
