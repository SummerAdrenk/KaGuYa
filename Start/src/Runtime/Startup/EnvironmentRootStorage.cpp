#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>
#include <Runtime/RuntimeCStringVector.h>

#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
class EnvironmentRootMemory final : public IEnvironmentRootMemory
{
public:
    void* AllocateBody(std::size_t bytes) override { return ::operator new(bytes); }
    // 4976E0/448C80/467DD0等最终交53FBFB的尺寸必须保留，不能丢成无尺寸delete。
    void DeleteBody(void* address, std::size_t bytes) override { ::operator delete(address, bytes); }
    void DeleteVector(void* address, std::size_t bytes) override { ::operator delete(address, bytes); }
};

}

IEnvironmentRootMemory& DefaultEnvironmentRootMemory() noexcept
{
    static EnvironmentRootMemory memory;
    return memory;
}

void DeleteEnvironmentRootAt4976E0(EnvironmentRootStorage& root, std::uint32_t flags,
    IEnvironmentRootMemory& memory) noexcept
{
    // 5ED340 flags=5约束整个函数；失败terminate，不继续销毁余下成员或回滚发布槽。
    ReleaseYtlReference(root.versionAt54);
    ReleaseYtlReference(root.pathsAt48);
    // 497784调用可抛43A700，终止边界属于外层4976E0；共享原始manager/vector释放核。
    DestroyRuntimeCStringVectorAt43A700(root.errorsAt3C, memory);
    DestroyEnvironmentArchiveReferencesAt497080(root.archivesAt30, memory);
    DestroyRuntimeCStringAt40CD40(&root.appendTitleAt14);
    DestroyRuntimeCStringAt40CD40(&root.titleAt10);
    if ((flags & 1u) != 0) memory.DeleteBody(&root, 0x60);
}
}
