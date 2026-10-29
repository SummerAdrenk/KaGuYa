#include <Runtime/SaveLoadDialogNativeWorkflow.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>

namespace KaGuYa::Start::Runtime
{
void SaveLoadDialogNativeGraphicsSource::DrawCapturedAt441980(GraphicsSurfaceNativeBody* source,const RECT* clip)
{
    const auto handle=source->handleAt04;
    if(!handle)return;
    (void)graphics_.SurfaceDrawRaw(handle,0,0,nullptr,nullptr,clip,0);
}
void SaveLoadDialogNativeGraphicsSource::BlitCapturedAt4425C0(GraphicsSurfaceNativeBody* target,
    GraphicsSurfaceNativeBody* source,const RECT* clip)
{
    const auto destination=target->handleAt04;
    if(!destination)return;
    const auto image=source->handleAt04;
    if(!image)return;
    (void)graphics_.SurfaceBlitRaw(destination,image,0,0,nullptr,nullptr,clip,0);
}
}
