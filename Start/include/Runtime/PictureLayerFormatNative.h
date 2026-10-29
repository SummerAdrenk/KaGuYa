#pragma once

#include <Runtime/AnimationFormatNative.h>

namespace KaGuYa::Start::Runtime
{
// 4482A0：PL00/01/10/11/20/30共用真44h与447F60，不构造、Clear或拥有body。
// PIC返回AL=0转int2；与448010忽略AL的AN包装不同。输入为借用Ref，不预检count。
std::int32_t LoadPictureLayerFormatNativeAt4482A0(AnimationFormatNativeBody44h& body,
    const YtlReferenceSlotRuntime& input, IAnimationFormatNativePictures& pictures);
}
