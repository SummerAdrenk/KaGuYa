#include <Runtime/RootProcessRetirementNative.h>
#include <Runtime/RootCriticalSectionNative.h>
#include <Runtime/RootRuntimeOwner.h>

namespace KaGuYa::Start::Runtime
{
void RetireRootProcessNativeAt5AB3C0(RootCriticalSectionNativeReference& lock,
    RootRuntimeOwner* const volatile& ownerMetadata) noexcept
{
    lock.Enter();
    auto* selectedOwner = ownerMetadata; // 5AB3F6对应正文选择，不能在Enter之前快照。
    if (selectedOwner && selectedOwner->ReadPublishedAt6198C4())
        selectedOwner->ReleaseCurrentUnderCriticalLockAt5AB3F6();
    // 不建RAII Leave guard；原5ECC20/5ECC28两个action均terminate，异常不执行后缀。
    lock.Leave();
}
}
