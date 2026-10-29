#include <Runtime/SaveFileStateNativeArchive.h>
#include <Runtime/EnvironmentArchiveRegistration.h>

#include <new>

namespace KaGuYa::Start::Runtime
{
SaveFileStateNativeArchive::SaveFileStateNativeArchive() noexcept : regularPagesAt20{}
{
    // Ref成员自身按+2C/+30/+34及+38/+3C/+40清零；七标量没有默认初始化器。
}
SaveFileStateNativeArchive::~SaveFileStateNativeArchive() noexcept
{
    // 469195..469211 / 5E8340 flags=5：先auto，再quick，最后普通页vector。
    ReleaseYtlReference(autoPageAt38);
    ReleaseYtlReference(quickPageAt2C);
    DestroySaveFilePagesAt4623A0(regularPagesAt20);
}
void SaveFileStateNativeArchive::Serialize(SerializeBuffer&) noexcept
{
    // 48B6A0只有retn4，不读取参数、模式或this字段。
}
void* SaveFileStateNativeArchive::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    this->~SaveFileStateNativeArchive();
    if (flags & 1u) ::operator delete(this, OriginalObjectSize);
    return this;
}
void RegisterSaveFileStateArchiveAt47E441(EnvironmentRootOwner& owner, IEnvironmentRootMemory& memory)
{
    void* raw = ::operator new(SaveFileStateNativeArchive::OriginalObjectSize);
    auto* body = raw ? ::new (raw) SaveFileStateNativeArchive : nullptr;
    // 47E49C在正文构造后才捕获根；没有新建正文的catch回滚资格。
    auto* root = owner.CaptureAt4180C0();
    RegisterEnvironmentArchiveAt46A4A0(*root, body, memory);
}
}
