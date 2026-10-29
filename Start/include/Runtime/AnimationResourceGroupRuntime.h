#pragma once
#include <Runtime/AnimationResourceRuntime.h>

namespace KaGuYa::Start::Runtime
{
// FileGroup主成员可引用后续PLT图片。只延后图片索引校验，结构检查保持不变；
// 实际绘制仍由各renderer按主+辅助完整帧表取帧，缺帧明确失败，不填充假帧。
AnimationParseState ParseFileGroupAnimationResource(std::span<const std::byte>,
    ParsedAnimationResource&, const ArchiveCodecServices&);
AnimationParseState ParseFileGroupAnimationResourceForTools(std::span<const std::byte>,
    ParsedAnimationResource&);
}
