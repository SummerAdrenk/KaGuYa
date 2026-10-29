#pragma once
#include <Runtime/SpriteResourceNativeLoad.h>

namespace KaGuYa::Start::Runtime
{
// 4B0840：member=0装主AN/静态，非零只在mode2装PL/静态辅助成员。
// 不做480650的cache/Clear，不替外层Pattern写logicalName/active/argument。
bool LoadSpriteFileGroupNativeAt4B0840(SpriteNativeBodyA0h& sprite,
    std::int32_t member, const ArchiveReaderReference& reader, std::int32_t playFromStart,
    IAnimationFormatNativePictures& pictures, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    ArchiveCodecInputMode* inputMode = nullptr);
}
