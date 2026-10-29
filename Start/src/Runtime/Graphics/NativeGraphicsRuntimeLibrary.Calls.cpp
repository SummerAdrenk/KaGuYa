#include <Runtime/NativeGraphicsRuntimeLibrary.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
bool CaptureModule(GraphicsRootOwner& owner,GraphicsModuleNativeBody8h*& module)
{
    auto* root=owner.CaptureAt4182F0();
    if(!root->moduleAt00.baseAt00)return false;
    module=ResolveGraphicsModuleReference(root->moduleAt00);
    return true;
}
template<class Function>
Function Resolve(GraphicsModuleNativeBody8h* module,const char* name,int length,
    IRuntimeStringManagerSource& strings,IGraphicsModuleNativeWin32Api& win32)
{ return reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(module,name,length,strings,win32)); }
}
void NativeGraphicsRuntimeLibrary::ApplyInitialFilteringAt48A3A5()
{
    // 48A3A5/48A448/48A4F6各自取根，能力只固定第一次的完整EAX；setter返回值忽略。
    GraphicsModuleNativeBody8h* module=nullptr;
    int capabilities=0;
    if(CaptureModule(root_,module))
        capabilities=Resolve<int(__cdecl*)()>(module,"IsScreenMagMinFiltering",23,strings_,win32_)();
    if((capabilities&1)&&CaptureModule(root_,module))
        Resolve<void(__cdecl*)(int)>(module,"SetScreenMagFiltering",21,strings_,win32_)(0);
    if((capabilities&0x10)&&CaptureModule(root_,module))
        Resolve<void(__cdecl*)(int)>(module,"SetScreenMinFiltering",21,strings_,win32_)(0);
}
void NativeGraphicsRuntimeLibrary::SetTextFirstName(const char* name)
{
    GraphicsModuleNativeBody8h* module=nullptr;
    if(!CaptureModule(root_,module))return;
    // 4D6CD0：显式长度、resolver之前撤名称EH资格、释放缓存header之后才调用。
    Resolve<void(__cdecl*)(const char*)>(module,"SetTextFirstName",16,strings_,win32_)(name);
}
void NativeGraphicsRuntimeLibrary::SetTextSecondName(const char* name)
{
    GraphicsModuleNativeBody8h* module=nullptr;
    if(!CaptureModule(root_,module))return;
    Resolve<void(__cdecl*)(const char*)>(module,"SetTextSecondName",17,strings_,win32_)(name);
}
std::uint8_t NativeGraphicsRuntimeLibrary::SaveSurfaceIfLoaded(int handle,HANDLE file)
{
    GraphicsModuleNativeBody8h* module=nullptr;
    if(!CaptureModule(root_,module))return 0;
    // 48951B..4895C0：第三参为null，保留DLL的AL，不归一化到bool。
    return Resolve<std::uint8_t(__cdecl*)(int,HANDLE,const RECT*)>(module,"SurfaceSave",11,strings_,win32_)(handle,file,nullptr);
}
void NativeGraphicsRuntimeLibrary::BackupScreenToSurfaceIfLoaded(int handle)
{
    GraphicsModuleNativeBody8h* module=nullptr;
    if(!CaptureModule(root_,module))return;
    // 4C3530七个cdecl参数，现有调用者仅传目标，其余全部为零。
    Resolve<void(__cdecl*)(int,int,int,const SIZE*,const RECT*,const RECT*,int)>(
        module,"SurfaceBackup",13,strings_,win32_)(handle,0,0,nullptr,nullptr,nullptr,0);
}
}
