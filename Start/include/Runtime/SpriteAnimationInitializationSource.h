#pragma once

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// 4CC61E / 4A8969：成功读取资源后，重新取得 Environment registry[1]+6D。
// 不是 Sprite 字段，也不能在整个 Pattern/CG 装载开始时缓存。
class ISpriteAnimationInitializationSource
{
public:
    virtual ~ISpriteAnimationInitializationSource() = default;
    [[nodiscard]] virtual std::uint8_t ReadRegistrySlot1ByteAt6D() const = 0;
};
}
