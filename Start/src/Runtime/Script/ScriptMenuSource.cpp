#include <Runtime/ScriptMenuSource.h>
#include <Runtime/EnvironmentRegistryRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/FileManagerNative.h>
#include <Runtime/MessageOwnerNative.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
RootScriptMenuSource::RootScriptMenuSource(EnvironmentRegistryRuntime& environment, RootRuntimeOwner& root) noexcept
    : environment_(environment), root_(root) {}
GlobalParamsArchive* RootScriptMenuSource::CaptureSlot7At4180C0()
{ return static_cast<GlobalParamsArchive*>(environment_.CaptureArchiveAt4180C0(7)); }
ScriptMenuRootReceiver RootScriptMenuSource::CaptureMessageRootAfterEnsureAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return {root_.Published(), [](void* p) -> ScriptMenuMessageReceiver {
        if (!p) throw std::logic_error("MENU null root (modern safety boundary)");
        auto* manager = static_cast<RootRuntimePayload*>(p)->PostWindow().FileManagerAt00();
        if (!manager) throw std::logic_error("MENU null file owner (modern safety boundary)");
        auto* message = ResolveMessageReference(manager->messageAt10);
        if (!message) return {nullptr, nullptr, nullptr}; // 4CF785/4CF79F，负choice仍可正常退出。
        // 同一次root解出的message既提供标志Ref，也接收持久化；不重新取全局message。
        return {message, &message->flagsAt10[1], [](void* receiver) {
            static_cast<MessageOwnerNative34h*>(receiver)->PersistUnreadFlagsAt43D3D0();
        }};
    }};
}
}
