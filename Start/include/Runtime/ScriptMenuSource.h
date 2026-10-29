#pragma once

#include <Runtime/ByteArrayReferenceRuntime.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentRegistryRuntime;
class RootRuntimeOwner;
class GlobalParamsArchive;

struct ScriptMenuMessageReceiver final
{
    void* object;
    ByteArrayReferenceRuntime* choicesAt1C;
    void (*persistAt43D3D0)(void*);
};
struct ScriptMenuRootReceiver final
{
    void* object;
    ScriptMenuMessageReceiver (*messageAt00_10)(void*);
};
class IScriptMenuSource
{
public:
    virtual ~IScriptMenuSource() = default;
    virtual GlobalParamsArchive* CaptureSlot7At4180C0() = 0;
    virtual ScriptMenuRootReceiver CaptureMessageRootAfterEnsureAt413BE0() = 0;
};
class RootScriptMenuSource final : public IScriptMenuSource
{
public:
    RootScriptMenuSource(EnvironmentRegistryRuntime& environment, RootRuntimeOwner& root) noexcept;
    GlobalParamsArchive* CaptureSlot7At4180C0() override;
    ScriptMenuRootReceiver CaptureMessageRootAfterEnsureAt413BE0() override;
private:
    EnvironmentRegistryRuntime& environment_;
    RootRuntimeOwner& root_;
};
}
