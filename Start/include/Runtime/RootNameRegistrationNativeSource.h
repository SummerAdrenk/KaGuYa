#pragma once
#include <Runtime/NameRegistrationDialogNative.h>
#include <Runtime/RootEditWindowNativeSource.h>
#include <Runtime/EditWindowNativeViewport.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class ParamsRuntimeRootOwner;
class NativeGraphicsRuntimeLibrary;
struct RootNameRegistrationNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    ParamsRuntimeRootOwner& params;
    InputRootOwner& input;
    NativeGraphicsRuntimeLibrary& graphics;
    IGuiControlResourceLoadServices& pictures;
    RootEditWindowNativeSource& edits;
    EditWindowNativeViewportServices& viewport;
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
};
class RootNameRegistrationNativeSource final:public INameRegistrationDialogNativeSource
{
public:
    explicit RootNameRegistrationNativeSource(RootNameRegistrationNativeDependencies d) noexcept:dependencies_(d){}
    void EnsureRootAt413BE0() override;
    void* RootAt6198C4() override;
    YtlReferenceSlotRuntime& RootDrawSlotAt18(void*) override;
    std::int32_t& RootModalFlagAt1B8(void*) override;
    YtlReferenceSlotRuntime* CaptureBackgroundAt4ACDD0(void*,YtlReferenceSlotRuntime*) override;
    YtlReferenceSlotRuntime* CreatePanelAt504E90(YtlReferenceSlotRuntime*,GuiDialogNativeBody5Ch*,const GuiStaticDescriptorNative24h*) override;
    void* NewEditAt46FA30(void*,std::int32_t,std::int32_t,HWND) override;
    YtlReferenceSlotRuntime* FindEditAt479DF0(void*,YtlReferenceSlotRuntime*,void*) override;
    void ApplyEditAt4875F0(void*,void*) override;
    void RemoveEditAt486B60(void*,void*) override;
    RuntimeCString* ReadEditTextAt50B000(void*,RuntimeCString*) override;
    void* ParamsAt418480() override;
    YtlReferenceSlotRuntime& ParamsGameSystemSlotAt04(void*) override;
    std::uint8_t GameSystemFlagsAt2C(void*) override;
    const char* GameSystemDefaultNameAt30(void*) override;
    const char* GameSystemDefaultNameAt34(void*) override;
    void* SystemAt4180C0() override;
    void* RegistryAt417990(void*,std::int32_t) override;
    RuntimeCString& RegistryNameAt5C(void*) override;
    RuntimeCString& RegistryNameAt58(void*) override;
    std::uint8_t& RegistryNameChangedAt54(void*) override;
    HWND SystemWindowAt2C(void*) override;
    HWND MainWindowAt6198C8() override;
    InputRootNativeBody24h* InputAt418210() override;
    BOOL IsWindowEnabled(HWND w) override{return ::IsWindowEnabled(w);}
    HWND SetFocus(HWND w) override{return ::SetFocus(w);}
    BOOL ShowWindow(HWND w,int command) override{return ::ShowWindow(w,command);}
    BOOL SetWindowTextA(HWND w,const char* text) override{return ::SetWindowTextA(w,text);}
    LRESULT SendMessageA(HWND w,UINT m,WPARAM p,LPARAM l) override{return ::SendMessageA(w,m,p,l);}
    BOOL DeleteObject(HGDIOBJ object) override{return ::DeleteObject(object);}
    HBRUSH CreateSolidBrush(COLORREF color) override{return ::CreateSolidBrush(color);}
    EditWindowNativeMapServices* NativeEditMaps() const noexcept override{return &dependencies_.edits.MapServices();}
    WindowThunkNativeServices* NativeEditThunks() const noexcept override{return &dependencies_.edits.ThunkServices();}
private:
    RootNameRegistrationNativeDependencies dependencies_;
};
}
