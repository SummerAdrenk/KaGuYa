#pragma once

#include <Runtime/EnvironmentRootRuntime.h>

#include <cstddef>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// 45C23F/45C252：真实 CSecretCG+18 的元素，不是同形 Archive/CString vector。
// 仅供原始存储中的显式生命周期使用，不对元素另跑隐式 ATL 析构。
struct SecretCgMappingRuntime final
{
    RuntimeCString nameAt00;
    EnvironmentVectorStorage<YtlReferenceSlotRuntime> referencesAt04;
};
using SecretCgMappingVectorStorage = EnvironmentVectorStorage<SecretCgMappingRuntime>;

static_assert(sizeof(SecretCgMappingRuntime) == 16);
static_assert(offsetof(SecretCgMappingRuntime, nameAt00) == 0);
static_assert(offsetof(SecretCgMappingRuntime, referencesAt04) == 4);
static_assert(sizeof(SecretCgMappingVectorStorage) == 12);
static_assert(offsetof(SecretCgMappingVectorStorage, begin) == 0);
static_assert(offsetof(SecretCgMappingVectorStorage, end) == 4);
static_assert(offsetof(SecretCgMappingVectorStorage, capacity) == 8);

// 4577F0：增/减/等长；先默认尾段再复制旧前缀。初始三指针置空。
// strings/memory 显式传入，不进入正文；失败残留按原 EH，不能假设强异常保证。
void ResizeSecretCgMappingVectorAt4577F0(
    SecretCgMappingVectorStorage& storage, std::uint32_t count,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory);

// 461500 / 5E8450 EHFlags=5：终止边界；正常返回后清三指针。
// begin==null 时直接返回；memory 必须与分配时同域。
void DestroySecretCgMappingVectorAt461500(
    SecretCgMappingVectorStorage& storage, IEnvironmentRootMemory& memory) noexcept;
}
