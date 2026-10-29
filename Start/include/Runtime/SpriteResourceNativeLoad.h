#pragma once

#include <Runtime/AnimationFormatNative.h>
#include <Runtime/SpriteNative.h>

namespace KaGuYa::Start::Runtime
{
class ArchiveReaderReference;
enum class ArchiveCodecInputMode;

// 47EEA0：0已逐段共享AN结果；1仅表示原格式码3，可静态回落；其余失败为2。
// 字符串/构造/提交异常不在448010的解析catch范围内。
std::int32_t LoadSpriteAnimationNativeAt47EEA0(SpriteNativeBodyA0h& sprite,
    const YtlReferenceSlotRuntime& bytes, IAnimationFormatNativePictures& pictures,
    IGraphicsSurfaceApi& surfaces, IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    ArchiveCodecInputMode* inputMode = nullptr);

// 480650实际Direct入口：缓存Name/argument，未命中Clear，分别晚取Bytes及最终Name。
// 只处理Direct；4B0840的FileGroup分支有不同的提交规则，不能别名到此函数。
bool LoadSpriteArchiveNativeAt480650(SpriteNativeBodyA0h& sprite,
    const ArchiveReaderReference& reader, std::int32_t playFromStart, std::uint32_t argument,
    IAnimationFormatNativePictures& pictures, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    ArchiveCodecInputMode* inputMode = nullptr);
}
