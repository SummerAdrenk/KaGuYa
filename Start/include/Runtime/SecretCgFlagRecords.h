#pragma once

#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentRootRuntime.h>

#include <cstddef>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
struct SecretCgFlagRecord final
{
    std::uint32_t idAt00;
    std::uint8_t byteFlagAt04;
    std::uint8_t unwrittenAt05[3];
};
static_assert(sizeof(SecretCgFlagRecord) == 8 && alignof(SecretCgFlagRecord) == 4);
static_assert(offsetof(SecretCgFlagRecord, idAt00) == 0);
static_assert(offsetof(SecretCgFlagRecord, byteFlagAt04) == 4);
static_assert(offsetof(SecretCgFlagRecord, unwrittenAt05) == 5);

// 45C120..45C14A：原24-byte control布局；实际cg_flag虚槽在cpp内实现。
using SecretCgFlagControl = EnvironmentObjectReferenceControl;
using SecretCgReferenceVector = EnvironmentVectorStorage<YtlReferenceSlotRuntime>;
static_assert(sizeof(SecretCgFlagControl) == 24 && sizeof(SecretCgReferenceVector) == 12);

// control虚槽独立可用，正文/control必须可由operator delete释放；memory不能另设释放域。
// 45C0AF先Allocate8，再读sourceRef的base/bias及缓存EDI的stride，不能预取ID/stride。
// out是新临时槽且不得与sourceRef别名；不增持sourceRef/cachedControl，不释放out旧引用。
void CreateRecordReference(YtlReferenceSlotRuntime& out,
    const YtlReferenceSlotRuntime& sourceRef, std::uint32_t index,
    const SecretCgFlagControl* cachedControl, IEnvironmentRootMemory& memory);

// 45C0F0：只接收新临时槽，不释放旧引用。24-byte分配失败保留base，control仍为空。
void AdoptSecretCgFlagAt45C0F0(YtlReferenceSlotRuntime& fresh,
    SecretCgFlagRecord* body, IEnvironmentRootMemory& memory);
[[nodiscard]] SecretCgFlagRecord* ResolveSecretCgFlag(
    const YtlReferenceSlotRuntime& reference) noexcept;

// 45C825：signed count<=0为空；payload走::operator new[]，control走memory.AllocateBody。
// 不初始化int内容。control失败保留base，不回收数组；虚槽使用普通delete[]/delete。
void AllocateSecretCgIndicesAt45C825(YtlReferenceSlotRuntime& out,
    std::uint32_t count, IEnvironmentRootMemory& memory);

void DestroySecretCgFlagPayloadAt492FD0(SecretCgFlagControl& control,
    IEnvironmentRootMemory& memory);
void DeleteSecretCgFlagControlAt468190(SecretCgFlagControl& control,
    std::uint32_t flags, IEnvironmentRootMemory& memory);

void ReleaseSecretCgReferenceAt460C00(YtlReferenceSlotRuntime& reference,
    IEnvironmentRootMemory& memory) noexcept;
void ReleaseSecretCgReferenceRangeAt452710(YtlReferenceSlotRuntime* begin,
    YtlReferenceSlotRuntime* end, IEnvironmentRootMemory& memory) noexcept;

// 45C155快路径 + 4551B0增长；408710经helper/EH核对后可共用此尾增API。
// 仅处理12-byte Ref，不构造mapping正文、不增加外层16-byte记录。
void AppendSecretCgReferenceAt4551B0(SecretCgReferenceVector& storage,
    const YtlReferenceSlotRuntime& source, IEnvironmentRootMemory& memory);
void ClearSecretCgReferencesAt45BD08(SecretCgReferenceVector& storage,
    IEnvironmentRootMemory& memory) noexcept;
void DestroySecretCgReferencesAt461440(SecretCgReferenceVector& storage,
    IEnvironmentRootMemory& memory) noexcept;

// 45C0AD..45C1B8叶操作：append失败释放临时Ref后外传；不包含45CAC4外层吞异常。
void AppendNewSecretCgFlagAt45C0AD(SecretCgReferenceVector& storage,
    const YtlReferenceSlotRuntime& sourceRef, std::uint32_t index,
    const SecretCgFlagControl* cachedControl, IEnvironmentRootMemory& memory);
}
