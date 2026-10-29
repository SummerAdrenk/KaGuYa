#pragma once
#include <Runtime/AppModuleMethodsNative.h>

namespace KaGuYa::Start::Runtime
{
using AppModuleNativeExit=void(__cdecl*)();
using AppModuleNativeRegistrar=int(__cdecl*)(AppModuleNativeExit);
AppModuleNativeBody3Ch& ProcessAppModuleNative()noexcept;
AppModuleNativeBody3Ch*& ProcessAppModulePublicationNative()noexcept;
AppModuleNativeBody3Ch*& ProcessAtlModulePublicationNative()noexcept;
GUID& ProcessAppModuleLibraryIdNative()noexcept;
AppModuleAutoMapNativeSource ProcessAppModuleAutoMapNative()noexcept;
void ConstructProcessAppModuleNativeAt401340(AppModuleNativeBody3Ch&,AppModuleNativeBody3Ch*& firstPublication,
    AppModuleNativeBody3Ch*& secondPublication,BYTE& sharedFailed,IWindowClassNativeApi&,
    AppModuleNativeExit,AppModuleNativeRegistrar);
void ReleaseProcessAppModuleNativeAt5AA420(AppModuleNativeBody3Ch&,AppModuleNativeServices&);
HRESULT InitializeProcessAppModuleFrameworkNativeAt43D724(HINSTANCE);
}
