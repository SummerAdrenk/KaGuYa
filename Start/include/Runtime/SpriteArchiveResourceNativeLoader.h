#pragma once

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
struct SpriteNativeBodyA0h;
class ArchiveReaderReference;
struct PatternResourceLoadRequest;
struct AnimationFormatNativePictureServices;

// 4CB030/4A84E0：只分流到480650或4B0840，不另取Bytes、配置或提交逻辑名。
// services只借用；格式调用区间明确选Raw codec，不读取额外输入Ref元数据。
bool LoadPatternSpriteFromReaderNative(SpriteNativeBodyA0h& sprite,
    const ArchiveReaderReference& reader, std::uint8_t initialization,
    const PatternResourceLoadRequest& request, AnimationFormatNativePictureServices& services);
}
