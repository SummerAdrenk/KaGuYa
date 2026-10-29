#include <Runtime/RootSaveLoadConfirmDialogNativeSource.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/GuiControlResourceLoadRuntime.h>

namespace KaGuYa::Start::Runtime
{
void* RootSaveLoadConfirmDialogNativeSource::CaptureLoadReceiverAt413BE0()
{ return pictures_.CaptureLoadReceiverAt413BE0(); }
bool RootSaveLoadConfirmDialogNativeSource::LoadIntoAt4800A0(void* receiver,
    GraphicsSurfaceNativeBody& surface, const char* name)
{ return pictures_.LoadIntoAt4800A0(receiver, surface, name); }
std::uint8_t RootSaveLoadConfirmDialogNativeSource::LoadSurfaceAt480000(void* receiver,
    YtlReferenceSlotRuntime& out, const char* name)
{ return LoadGuiControlSurfaceReferenceAt480000(out, receiver, name, pictures_, graphics_, strings_); }
const RuntimeCString& RootSaveLoadConfirmDialogNativeSource::MaskNameAt61A608() { return mask_; }
const char* RootSaveLoadConfirmDialogNativeSource::FontFaceAt61A60C() { return font_.GetString(); }
bool RootSaveLoadConfirmDialogNativeSource::CreateTextFontAt4388B0(const GraphicsTextNativeFontParameters* parameters)
{ return graphics_.TextCalls().CreateTextFontNativeAt4388B0(parameters); }
IGraphicsTextApi& RootSaveLoadConfirmDialogNativeSource::TextAt4182F0() { return graphics_.TextCalls(); }
std::int32_t RootSaveLoadConfirmDialogNativeSource::ReadImmutableDwordAtOriginalVa(std::uint32_t address)
{ return ReadSaveLoadConfirmDialogNativeImmutableDword(address); }
}
