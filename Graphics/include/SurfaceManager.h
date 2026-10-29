#pragma once

#include "RendererReference.h"
#include "TextRenderer.h"
#include "SurfaceReference.h"
#include "DibBitmapReference.h"
#include "NativeReferenceArray.h"

namespace KaGuYa::Graphics
{
// 原CSurfaceManager持有Surface数组、CTextBlt Ref和DIB数组；Close不销毁manager。
struct SurfaceManager final
{
    ~SurfaceManager() noexcept(false);
    NativeReferenceArray<SurfaceReference> surfaces;
    // 原0236A构造只写两张表及text；+0C/+28未写，不把它们当指针或额外owner。
    std::uint32_t unwrittenAt0C;
    RendererReference<TextState, TextStateDeleter> text;
    NativeReferenceArray<DibBitmapReference> bitmaps;
    std::uint32_t unwrittenAt28;
};
static_assert(sizeof(SurfaceManager) == 0x2C);
static_assert(offsetof(SurfaceManager, text) == 0x10 && offsetof(SurfaceManager, bitmaps) == 0x1C);

SurfaceManager* GetSurfaceManager() noexcept;
SurfaceManager& RequireSurfaceManager();
}
