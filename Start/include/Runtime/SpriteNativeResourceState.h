#pragma once

#include <Runtime/SpriteNative.h>
#include <Runtime/PatternResourceExecutor.h>

namespace KaGuYa::Start::Runtime
{
// 4CB9E9：检查CString显式长度，不检查active/Surface或首字符。
bool IsSpriteLogicalNameEmptyNativeAt4CB9E9(const SpriteNativeBodyA0h& sprite) noexcept;

// 4AD080的1/3/0分别映射Empty/InvalidLoadMode/Valid，不把enum序号当原返回码。
PatternSpriteRectResult ReadSpriteDrawableRectNativeAt4AD080(const SpriteNativeBodyA0h& sprite) noexcept;

// 原入口在4CBD69的mask非零分支之后；调用方负责mask==0时跳过整组。
// 不等同完整Clear；此叶可抛，内部43F240/43EA10仍保留终止边界。
// 调用方须让借用GraphicsSurfaceNativeScope覆盖本次清理，不把服务写进A0h。
void PrepareSpriteFileGroupNativeAt4CBD71(SpriteNativeBodyA0h& sprite, FileGroupChange changes);
// 逻辑名提交直接复用AssignRuntimeCStringAt410220(sprite.logicalNameAt0C, source)。
}
