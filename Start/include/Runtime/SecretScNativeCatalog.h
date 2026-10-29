#pragma once
#include <Runtime/EnvironmentStateArchives.h>

namespace KaGuYa::Start::Runtime
{
// 三个原虚槽的同一执行体；只经CaptureBoundSecretCgParamsAt418480借真实30h root。
// 宿主用现有SecretCgParamsBinding绑定来源，不新建SC绑定、缓存或现代catalog。
// 返回Params thumbnail正文+04的借用地址，临时Ref释放后不为调用者保活。
const char* const* ReadSecretScThumbnailNativeAt478230(
    const SecretScArchive&, std::int32_t category, std::int32_t item);
// 原EHFlags=1且无catch，不能照搬CG百分比槽的noexcept。
std::int32_t ReadSecretScPercentAllNativeAt47D830(const SecretScArchive&);
std::int32_t ReadSecretScPercentCategoryNativeAt47C050(const SecretScArchive&, std::int32_t category);
}
