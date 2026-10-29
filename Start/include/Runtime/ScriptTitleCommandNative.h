#pragma once

#include <Runtime/RuntimeCString.h>
#include <Runtime/ScriptDispatchNative.h>

namespace KaGuYa::Start::Runtime
{
struct ScriptTitleRootReceiver final
{
    void* object;
    std::uint32_t (*readPcAt47BDB0)(void*);
    RuntimeCString& (*titleAt47BDB0)(void*);
};
class IScriptTitleCommandSource
{
public:
    virtual ~IScriptTitleCommandSource() = default;
    virtual ScriptTitleRootReceiver CaptureAfterEnsureAt413BE0() = 0;
};
class RootScriptTitleCommandSource final : public IScriptTitleCommandSource
{
public:
    explicit RootScriptTitleCommandSource(RootRuntimeOwner& root) noexcept : root_(root) {}
    ScriptTitleRootReceiver CaptureAfterEnsureAt413BE0() override;
private:
    RootRuntimeOwner& root_;
};

[[nodiscard]] std::int32_t ExecuteScriptTitleAt4D25A0(ScrContainer& parser,
    IScriptTitleCommandSource& source, IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
