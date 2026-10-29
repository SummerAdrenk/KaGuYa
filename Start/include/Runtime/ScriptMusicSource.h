#pragma once

#include <Runtime/ScrVmMusicHandler.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentRegistryRuntime;
class RootRuntimeOwner;
struct SoundManagerNativeBody50h;
class SoundManagerNativeMusic;

// 原manager借用receiver按值固定；服务指针在正文外，不增持或延长旧manager寿命。
class ScriptMusicManagerReceiver final
{
public:
    ScriptMusicManagerReceiver(IScrVmMusicRuntimeBoundary*) noexcept;
    static ScriptMusicManagerReceiver Native(SoundManagerNativeBody50h*, SoundManagerNativeMusic&) noexcept;
    explicit operator bool() const noexcept { return object_ != nullptr; }
    void* Object() const noexcept { return object_; }
    bool HasPlaybackSlots() const;
    void ApplyActiveName(const char* name, std::int32_t value) const;
    void StopMusicSlots() const;
private:
    struct Operations
    {
        bool (*hasSlots)(void*, void*);
        void (*apply)(void*, void*, const char*, std::int32_t);
        void (*stop)(void*, void*);
    };
    ScriptMusicManagerReceiver(void* object, void* service, const Operations& operations) noexcept
        : object_(object), service_(service), operations_(&operations) {}
    void* object_;
    void* service_;
    const Operations* operations_;
};

struct ScriptMusicParameterReceiver final
{
    void* object;
    const std::uint32_t* (*volumeAt14)(void*);
};

class IScriptMusicSource
{
public:
    virtual ~IScriptMusicSource() = default;
    virtual ScriptMusicParameterReceiver CaptureParametersAt1C() = 0;
    virtual ScriptMusicManagerReceiver CaptureSoundManagerAfterEnsureAt413BE0() = 0;
};

class RootScriptMusicSource final : public IScriptMusicSource
{
public:
    RootScriptMusicSource(EnvironmentRegistryRuntime& environment, RootRuntimeOwner& root) noexcept;
    ScriptMusicParameterReceiver CaptureParametersAt1C() override;
    ScriptMusicManagerReceiver CaptureSoundManagerAfterEnsureAt413BE0() override;
private:
    EnvironmentRegistryRuntime& environment_;
    RootRuntimeOwner& root_;
};

void ApplyScriptMusicAt4D59E0(std::int32_t resourceId, IScriptMusicSource& source,
    const IScrResourceNameSource& names, IScrVmMusicNameWriter& writer);
}
