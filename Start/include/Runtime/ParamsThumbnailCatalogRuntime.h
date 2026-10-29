#pragma once

#include <Runtime/ParamsRuntimeProfile.h>

namespace KaGuYa::Start::Runtime
{
// 477800 的原输出槽合同：命中复制三 DWORD 并增持；未命中清零。
// outRef 是新返回槽，不减持其中的旧值；调用方负责这份新引用。
[[nodiscard]] YtlReferenceSlotRuntime& FindParamsThumbnailReferenceAt477800(
    const ParamsThumbnailTable& thumbnails, YtlReferenceSlotRuntime& outRef,
    const char* nameCp932);

// 47818B..4781D6 / 478632..478678：先取正文+4字段地址，再释放缓存control。
// 返回字段仍借用params，不把临时Ref留在现代identifier中延长正文寿命。
[[nodiscard]] const char* const* BorrowParamsThumbnailAliasAt477800(
    const ParamsGameSystem& gameSystem, const char* nameCp932);

// 现代正文查询便利口；同样经过原Ref返回/释放，不冒充完整477FD0/478230。
[[nodiscard]] const ParamsThumbnailRuntime* FindParamsThumbnailAt477800(
    const ParamsGameSystem& gameSystem, const char* nameCp932);
}
