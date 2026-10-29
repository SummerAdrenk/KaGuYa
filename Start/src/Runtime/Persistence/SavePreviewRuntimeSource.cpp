#include <Runtime/SavePreviewRuntimeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ParamsRuntimeRoot.h>

namespace KaGuYa::Start::Runtime
{
RootSavePreviewRuntimeSource::RootSavePreviewRuntimeSource(
    RootRuntimeOwner& root, ParamsRuntimeRootOwner& params) noexcept : root_(root), params_(params) {}

DrawLayersView RootSavePreviewRuntimeSource::CaptureDrawLayersAt413BE0() const
{
    root_.EnsureCreatedAt413BE0();
    return root_.RequirePublished().PostWindow().DrawLayersAt18();
}

const ParamsGameSystem* RootSavePreviewRuntimeSource::CaptureGameSystemAt418480() const
{
    return params_.GetOrCreateAt418480().GameSystem().get();
}
}
