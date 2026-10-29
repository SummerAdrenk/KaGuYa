#include <Runtime/RootSaveLoadNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/SavePreviewFileNative.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
RootSavePreviewRuntime Preview(RootRuntimePayload& root, RootSaveLoadNativeDependencies& d)
{
    // 464191..464213的根析构仍会退双预览Ref；4326A8在模块卸载后也要查询Graphics根。
    // 将同一长寿命API登记给已选root，不能只依赖调用期间的临时Surface scope。
    return root.CreateSavePreviewService(d.graphics,d.graphics,d.graphics,d.files,d.previewCapture);
}
}
void RootSaveLoadNativeSource::CapturePreviewAt48DBE0(void* root)
{ auto preview = Preview(*static_cast<RootRuntimePayload*>(root),dependencies_); preview.CaptureForSaveAt48DBE0(); }
std::uint32_t RootSaveLoadNativeSource::SavePreviewAt489420(void* root, const char* path)
{
    auto preview = Preview(*static_cast<RootRuntimePayload*>(root),dependencies_);
    return preview.SavePreviewAt489420(path);
}
YtlReferenceSlotRuntime* RootSaveLoadNativeSource::CapturePreviewAt47D740(void* root, YtlReferenceSlotRuntime* out)
{
    auto preview = Preview(*static_cast<RootRuntimePayload*>(root),dependencies_);
    return preview.PreviewAt47D740(*out);
}
GraphicsSurfaceNativeBody* RootSaveLoadNativeSource::ConstructPreviewSurfaceAt40C2E0(void* raw)
{ return ::new(raw) GraphicsSurfaceNativeBody(dependencies_.graphics,dependencies_.strings); }
std::uint32_t RootSaveLoadNativeSource::LoadPreviewAt511D90(GraphicsSurfaceNativeBody* surface,
    const char* path, const void* flags)
{
    return LoadSavePreviewFileNativeAt511D90(*surface,path,std::uint32_t(reinterpret_cast<std::uintptr_t>(flags)),
        dependencies_.files,dependencies_.graphics,dependencies_.strings,dependencies_.memory);
}
}
