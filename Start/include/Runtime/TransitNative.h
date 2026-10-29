#pragma once

#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/TransitRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 分配服务只在原生调用期间借用；正文和18h control均不保存第二owner或服务字段。
// 直接调用正文/control虚槽且使用非默认分配器时，也必须由调用者建立此scope。
class TransitNativeMemoryScope final
{
public:
    explicit TransitNativeMemoryScope(IEnvironmentRootMemory& memory) noexcept;
    ~TransitNativeMemoryScope() noexcept;
    TransitNativeMemoryScope(const TransitNativeMemoryScope&) = delete;
    TransitNativeMemoryScope& operator=(const TransitNativeMemoryScope&) = delete;
private:
    IEnvironmentRootMemory* previous_;
};

[[nodiscard]] TransitRuntime* NewTransitNativeAt458819(ICrtRandomSource& random,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
// 45883E以后不再拥有新正文的构造EH资格；旧释放/新control分配失败均不得回收新正文。
void InstallTransitReferenceAt45883E(YtlReferenceSlotRuntime& slot, TransitRuntime* body,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
[[nodiscard]] TransitRuntime* ResolveTransitReference(const YtlReferenceSlotRuntime& slot) noexcept;
void ReleaseTransitReferenceAt458841(YtlReferenceSlotRuntime& slot,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void ReleaseTransitReferenceAt4603B0(YtlReferenceSlotRuntime& slot,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;
void DestroyTransitNativeAt448C70(TransitRuntime& body) noexcept;
[[nodiscard]] TransitRuntime* DeleteTransitNativeAt448C80(TransitRuntime* body,
    std::uint32_t flags, IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
}
