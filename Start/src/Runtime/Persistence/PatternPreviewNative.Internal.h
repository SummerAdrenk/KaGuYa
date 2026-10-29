#pragma once
#include <Runtime/PatternPreviewNative.h>

namespace KaGuYa::Start::Runtime
{
void ReleasePatternPreviewSavedControl(YtlReferenceControlRuntime*) noexcept;
void ReleasePatternPreviewSavedControlMayThrow(YtlReferenceControlRuntime*);
void RenderPatternPreviewSpritesNative(PatternLayerNativeBody80h&,YtlReferenceSlotRuntime&,
    PatternPreviewNativeServices&);
YtlReferenceControlRuntime* FilterPatternPreviewNative(PatternLayerNativeBody80h&,YtlReferenceSlotRuntime&,
    PatternPreviewNativeServices&);
}
