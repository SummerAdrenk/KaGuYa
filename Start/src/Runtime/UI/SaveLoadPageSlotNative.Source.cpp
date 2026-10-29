#include <Runtime/SaveLoadPageSlotNative.h>
#include <Runtime/RootSaveLoadNativeSource.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>

namespace KaGuYa::Start::Runtime
{
EnvironmentRootStorage* RootSaveLoadPageSlotNativeSource::CaptureEnvironmentAt4180C0()
{ return source_.CaptureEnvironmentAt4180C0(); }
IRuntimeStringManagerSource& RootSaveLoadPageSlotNativeSource::Strings() { return source_.Strings(); }
BOOL RootSaveLoadPageSlotNativeSource::DeleteFileA(const char* path) { return source_.DeleteFileA(path); }
void RootSaveLoadPageSlotNativeSource::ConstructInputAt40B530(RuntimeCString* out, const char* text)
{ source_.ConstructInputAt40B530(out, text); }
int RootSaveLoadPageSlotNativeSource::LocalTime64At5697BB(std::tm* out, const std::int64_t* time)
{ return source_.LocalTime64At5697BB(out, time); }
GraphicsSurfaceNativeBody* RootSaveLoadPageSlotNativeSource::ConstructPreviewSurfaceAt40C2E0(void* raw)
{ return source_.ConstructPreviewSurfaceAt40C2E0(raw); }
std::uint32_t RootSaveLoadPageSlotNativeSource::LoadPreviewAt511D90(GraphicsSurfaceNativeBody* surface,
    const char* path, const void* flags) { return source_.LoadPreviewAt511D90(surface, path, flags); }
bool RootSaveLoadPageSlotNativeSource::CreateTextFontAt4388B0(const GraphicsTextNativeFontParameters* font)
{ return graphics_.TextCalls().CreateTextFontNativeAt4388B0(font); }
bool RootSaveLoadPageSlotNativeSource::InitializeTextAt4389D0(const char* text, std::int32_t x, std::int32_t y)
{ return graphics_.TextCalls().InitializeText(text, x, y); }
bool RootSaveLoadPageSlotNativeSource::BlitAllTextAt53A980(GraphicsSurfaceHandle handle, RECT* rect)
{ return graphics_.TextCalls().BlitAllTextToSurface(handle, rect); }
}
