#pragma once
#include <Runtime/SceneManagerNative.h>
#include <Runtime/RootScenePublicNativeServicesRuntime.h>
#include <optional>

namespace KaGuYa::Start::Runtime
{
struct RootSceneManagerNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentStartupRuntime& environment;
    NativeInputRuntime& input;
    NativeGraphicsRuntimeLibrary& graphics;
    IStartFileApi& files;
    const IEnvironmentKeyTableSource& key;
    ArchiveCodecServices codecs;
    ISceneFactoryNativeSource& factory;
    IGuiControlResourceMessageApi& messages;
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
    const FileTransactionContext* transaction=nullptr;
};
// 长期持有同源服务集合，不拥有root/48h/公共控件，也不在构造或getter时Capture。
// 原48h及逃逸公共/task Ref须先退役；Root/Post的服务借用也须在本集合销毁前解除，
// 或让本集合覆盖Root最终清理。外部factory及全部依赖须更长寿。
class RootSceneManagerNativeServicesRuntime final
{
public:
    explicit RootSceneManagerNativeServicesRuntime(RootSceneManagerNativeDependencies) noexcept;
    // 完整生产图：公共服务先构造，CG/Replay和factory随后完成，最后聚合Manager。
    RootSceneManagerNativeServicesRuntime(RootScenePublicNativeServicesRuntime&,ISceneFactoryNativeSource&)noexcept;
    RootSceneManagerNativeServicesRuntime(const RootSceneManagerNativeServicesRuntime&)=delete;
    RootSceneManagerNativeServicesRuntime& operator=(const RootSceneManagerNativeServicesRuntime&)=delete;
    SceneManagerNativeServices& Services() noexcept{return services_;}
private:
    // 旧便利入口在原位拥有公共服务，不增加堆分配；外借入口不构造第二组。
    std::optional<RootScenePublicNativeServicesRuntime> ownedPublic_;
    SceneManagerNativeServices services_;
};
}
