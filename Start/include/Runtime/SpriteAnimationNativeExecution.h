#pragma once

#include <Runtime/SpriteNative.h>

namespace KaGuYa::Start::Runtime
{
// 443740：0执行了一个等待段/走到末尾；1未激活；2暂停或只减等待；-1原命令错误。
// 消费54h正文当前Ref/stride/bias，不加负索引、参数长度或循环次数的现代限制。
std::int32_t AdvanceSpriteAnimationNativeAt443740(SpriteAnimationNativeBody54h& body);

// 47E980：先逐项替换/初始化映射及counter，再执行首段；失败不回滚此前的发布。
// 参数按原DWORD判零，不提前bool归一化。repeat/result/commands/branches不是本入口的重置对象。
bool InitializeSpriteAnimationNativeAt47E980(SpriteAnimationNativeBody54h& body,
    std::int32_t playFromStart);

// 46B1D0：仅按logicalName长度及mode==2门控，不检查Sprite.active。
std::int32_t AdvanceSpriteNativeAt46B1D0(SpriteNativeBodyA0h& body);

// 4C23A0：按mappingCount*4的32位宽度memmove，保留Ref偏移/重叠，不重新分配。
void CopySpriteFinalMappingsNativeAt4C23A0(SpriteAnimationNativeBody54h& body);
// 这两个原入口内联同一命令核心，但外围停止条件不同，不能彼此代替。
void AdvanceSpriteAnimationToFinalNativeAt4C1D20(SpriteAnimationNativeBody54h& body);
void AdvanceSpriteAnimationSegmentsNativeAt4D5790(SpriteAnimationNativeBody54h& body,
    std::int32_t segmentCount);
}
