#include <Runtime/SettingDialogNativeGlobals.h>
#include <Runtime/RootSettingPage04NativeSource.h>
#include <cstdlib>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
alignas(RuntimeCString) constinit std::array<std::byte,16> slots{};
constexpr SettingDialogGlobalEvidence evidence[]{
    {0x5AD718,0x407310,0x61A644,0x5AC5F0},
    {0x5AD71C,0x4073A0,0x61A648,0x5AC610},
    {0x5AD720,0x406BC0,0x61A64C,0x5AC450},
    {0x5AD724,0x407430,0x61A650,0x5AC630}};
std::size_t Index(SettingDialogGlobalId id) noexcept
{const auto i=static_cast<std::size_t>(id);if(i>=4)_invalid_parameter_noinfo_noreturn();return i;}
class PreviewSource final:public ISettingPage01PreviewBlockNativeGlobals
{
public:
    const char* FontFaceAt61A644() override{return ProcessSettingDialogNativeGlobal(SettingDialogGlobalId::PreviewFace).GetString();}
    const char* SampleCharactersAt61A648() override{return ProcessSettingDialogNativeGlobal(SettingDialogGlobalId::PreviewSample).GetString();}
};
// 仅外部接口元数据；显式placement不登记额外C++静态析构，不另建字体指针副本。
alignas(PreviewSource) constinit std::array<std::byte,sizeof(PreviewSource)> previewSource{};
class Registrar final:public ISettingDialogGlobalExitRegistrar
{public:int RegisterAtExit(SettingDialogGlobalExit callback) override{return std::atexit(callback);}};
template<SettingDialogGlobalId id>void __cdecl Release()
{DestroyRuntimeCStringAt40CD40(&ProcessSettingDialogNativeGlobal(id));}
template<SettingDialogGlobalId id>void __cdecl Initialize()
{
    if constexpr(id==SettingDialogGlobalId::PreviewFace)::new(previewSource.data()) PreviewSource;
    Registrar registrar;InitializeSettingDialogNativeGlobal(id,ProcessSettingDialogNativeGlobal(id),Release<id>,registrar);
}
}
RuntimeCString& ProcessSettingDialogNativeGlobal(SettingDialogGlobalId id) noexcept
{return *reinterpret_cast<RuntimeCString*>(slots.data()+4*Index(id));}
ISettingPage01PreviewBlockNativeGlobals& ProcessSettingDialogPreviewGlobals() noexcept
{return *std::launder(reinterpret_cast<PreviewSource*>(previewSource.data()));}
std::span<const SettingDialogGlobalEvidence,4> SettingDialogNativeGlobalsSample18() noexcept{return evidence;}
void InitializeSettingDialogNativeGlobal(SettingDialogGlobalId id,RuntimeCString& slot,SettingDialogGlobalExit callback,
    ISettingDialogGlobalExitRegistrar& registrar,IRuntimeStringManagerSource& strings)
{
    switch(id)
    {
    case SettingDialogGlobalId::PreviewFace:ConstructSettingPage01PreviewFontFaceAt407310(slot,strings);break;
    case SettingDialogGlobalId::PreviewSample:ConstructSettingPage01PreviewSampleAt4073A0(slot,strings);break;
    case SettingDialogGlobalId::NameFace:ConstructSettingNameFontFaceAt406BC0(slot,strings);break;
    case SettingDialogGlobalId::SystemFace:ConstructSettingSystemFontFaceAt407430(slot,strings);break;
    default:_invalid_parameter_noinfo_noreturn();
    }
    struct Unwind final{RuntimeCString* slot;~Unwind() noexcept{if(slot)DestroyRuntimeCStringAt40CD40(slot);}}unwind{&slot};
    static_cast<void>(registrar.RegisterAtExit(callback));unwind.slot=nullptr;
}
using Initializer=void(__cdecl*)();
#pragma section(".CRT$XCUKaGuYa5AD718",read)
#pragma section(".CRT$XCUKaGuYa5AD71C",read)
#pragma section(".CRT$XCUKaGuYa5AD720",read)
#pragma section(".CRT$XCUKaGuYa5AD724",read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD718")) Initializer const KaGuYaSettingPreviewFaceInitializer=Initialize<SettingDialogGlobalId::PreviewFace>;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD71C")) Initializer const KaGuYaSettingPreviewSampleInitializer=Initialize<SettingDialogGlobalId::PreviewSample>;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD720")) Initializer const KaGuYaSettingNameFaceInitializer=Initialize<SettingDialogGlobalId::NameFace>;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD724")) Initializer const KaGuYaSettingSystemFaceInitializer=Initialize<SettingDialogGlobalId::SystemFace>;
#pragma comment(linker,"/include:_KaGuYaSettingPreviewFaceInitializer")
#pragma comment(linker,"/include:_KaGuYaSettingPreviewSampleInitializer")
#pragma comment(linker,"/include:_KaGuYaSettingNameFaceInitializer")
#pragma comment(linker,"/include:_KaGuYaSettingSystemFaceInitializer")
}
