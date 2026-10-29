#include <Runtime/LoadAfterSelectionServicesRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/DrawManagerRuntime.h>

namespace KaGuYa::Start::Runtime
{
DrawLayersView RootLoadAfterDrawSource::CaptureDrawLayersAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return root_.RequirePublished().PostWindow().DrawLayersAt18();
}
DrawTransitionView RootLoadAfterDrawSource::CaptureTransitionAfter413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return root_.RequirePublished().PostWindow().DrawTransitionAt18(graphics_);
}
DrawLayersView FixedLoadAfterDrawSource::CaptureDrawLayersAt413BE0(){return DrawLayersView(manager_->Layers());}
DrawTransitionView FixedLoadAfterDrawSource::CaptureTransitionAfter413BE0(){return DrawTransitionView(*manager_);}
}
