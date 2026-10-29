#include <Runtime/RootNameRegistrationNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/GuiPanelNative.h>

namespace KaGuYa::Start::Runtime
{
void RootNameRegistrationNativeSource::EnsureRootAt413BE0(){dependencies_.root.EnsureCreatedAt413BE0();}
void* RootNameRegistrationNativeSource::RootAt6198C4(){return dependencies_.root.ReadPublishedAt6198C4();}
YtlReferenceSlotRuntime& RootNameRegistrationNativeSource::RootDrawSlotAt18(void* root)
{return static_cast<RootRuntimePayload*>(root)->DrawManagerSlotAt18();}
std::int32_t& RootNameRegistrationNativeSource::RootModalFlagAt1B8(void* root)
{
    // 原getter只借当前DWORD，不追加Root::RequirePreWindowInitialization业务门控。
    return reinterpret_cast<std::int32_t&>(static_cast<RootRuntimePayload*>(root)->Frame().Lifecycle().valueAt1B8);
}
YtlReferenceSlotRuntime* RootNameRegistrationNativeSource::CaptureBackgroundAt4ACDD0(void*,YtlReferenceSlotRuntime* out)
{
    // 原callee不消费Draw ECX；caller已独立解析该Ref，不能在这里多Ensure。
    CaptureGraphicsScreenAt4ACDD0(*out,dependencies_.graphics,dependencies_.graphics,dependencies_.graphics);return out;
}
YtlReferenceSlotRuntime* RootNameRegistrationNativeSource::CreatePanelAt504E90(YtlReferenceSlotRuntime* out,
    GuiDialogNativeBody5Ch* parent,const GuiStaticDescriptorNative24h* descriptor)
{return CreateGuiPanelAt504E90(out,parent,descriptor,dependencies_.pictures,dependencies_.graphics,dependencies_.strings);}
void* RootNameRegistrationNativeSource::NewEditAt46FA30(void* root,std::int32_t disable,std::int32_t alignment,HWND borderArgument)
{return dependencies_.edits.CreateAt46FA30(*static_cast<RootRuntimePayload*>(root),std::uint32_t(disable),std::uint32_t(alignment),reinterpret_cast<std::uintptr_t>(borderArgument));}
YtlReferenceSlotRuntime* RootNameRegistrationNativeSource::FindEditAt479DF0(void* root,YtlReferenceSlotRuntime* out,void* edit)
{return dependencies_.edits.FindAt479DF0(*static_cast<RootRuntimePayload*>(root),out,edit);}
void RootNameRegistrationNativeSource::ApplyEditAt4875F0(void* root,void* edit)
{UpdateEditWindowNativeViewportAt4875F0(static_cast<RootRuntimePayload*>(root)->EditWindowsAt54(),static_cast<EditWindowNativeBody98h*>(edit),dependencies_.viewport,
    dependencies_.edits.MapServices(),dependencies_.edits.ThunkServices());}
void RootNameRegistrationNativeSource::RemoveEditAt486B60(void* root,void* edit)
{dependencies_.edits.RemoveAt486B60(*static_cast<RootRuntimePayload*>(root),edit);}
RuntimeCString* RootNameRegistrationNativeSource::ReadEditTextAt50B000(void* edit,RuntimeCString* out)
{return ReadEditWindowNativeTextAt50B000(*static_cast<EditWindowNativeBody98h*>(edit),out,dependencies_.strings);}
void* RootNameRegistrationNativeSource::ParamsAt418480(){return dependencies_.params.CaptureAt418480();}
YtlReferenceSlotRuntime& RootNameRegistrationNativeSource::ParamsGameSystemSlotAt04(void* params)
{return static_cast<ParamsRuntimeRoot*>(params)->gameSystemAt04.Native();}
std::uint8_t RootNameRegistrationNativeSource::GameSystemFlagsAt2C(void* game)
{return static_cast<ParamsGameSystem*>(game)->defaultNameMode;}
const char* RootNameRegistrationNativeSource::GameSystemDefaultNameAt30(void* game)
{return static_cast<ParamsGameSystem*>(game)->defaultGivenName.Native().GetString();}
const char* RootNameRegistrationNativeSource::GameSystemDefaultNameAt34(void* game)
{return static_cast<ParamsGameSystem*>(game)->defaultFamilyName.Native().GetString();}
void* RootNameRegistrationNativeSource::SystemAt4180C0(){return dependencies_.environment.CaptureAt4180C0();}
void* RootNameRegistrationNativeSource::RegistryAt417990(void* system,std::int32_t index)
{return ResolveEnvironmentArchiveAt417990(*static_cast<EnvironmentRootStorage*>(system),std::uint32_t(index));}
RuntimeCString& RootNameRegistrationNativeSource::RegistryNameAt5C(void* save)
{return static_cast<SaveDataEnvironmentArchive*>(save)->stringAt5C;}
RuntimeCString& RootNameRegistrationNativeSource::RegistryNameAt58(void* save)
{return static_cast<SaveDataEnvironmentArchive*>(save)->stringAt58;}
std::uint8_t& RootNameRegistrationNativeSource::RegistryNameChangedAt54(void* save)
{return static_cast<SaveDataEnvironmentArchive*>(save)->valueAt54;}
HWND RootNameRegistrationNativeSource::SystemWindowAt2C(void* system)
{return reinterpret_cast<HWND>(static_cast<EnvironmentRootStorage*>(system)->windowAt2C);}
HWND RootNameRegistrationNativeSource::MainWindowAt6198C8()
{
    // 原51F3B6..51F4B4是60h Environment的内联捕获，最后取+2C HWND。
    return SystemWindowAt2C(dependencies_.environment.CaptureAt4180C0());
}
InputRootNativeBody24h* RootNameRegistrationNativeSource::InputAt418210(){return dependencies_.input.CaptureAt418210();}
}
