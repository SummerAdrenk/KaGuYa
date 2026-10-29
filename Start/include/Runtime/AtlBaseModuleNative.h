#pragma once
#include <Runtime/WindowClassNative.h>

namespace KaGuYa::Start::Runtime
{
class IAtlBaseModuleNativeDiagnostics
{
protected:
    ~IAtlBaseModuleNativeDiagnostics()=default;
public:
    virtual BOOL IsDebuggerPresent()=0;
    virtual void OutputDebugStringW(const wchar_t*)=0;
    virtual void MoveBytesAt43D2D0(void* destination,std::uint32_t capacity,
        const void* source,std::uint32_t count)=0;
};
IAtlBaseModuleNativeDiagnostics& DefaultAtlBaseModuleNativeDiagnostics()noexcept;
const GUID& AtlBaseModuleNativeVersionGuid()noexcept;
struct AtlBaseModuleNativeServices final
{
    IWindowClassNativeApi& windows;
    IAtlBaseModuleNativeDiagnostics& diagnostics;
};
// image/guid仅重定位原400000/5C1E90，版本E00h和业务顺序不开放配置。
// sharedStartupFailed对应进程共享6197E9，不在正文内暗建另一份成功状态。
WindowClassModuleNativeBody38h* ConstructAtlBaseModuleNativeAt53E893(WindowClassModuleNativeBody38h&,
    HINSTANCE image,const GUID& versionGuid,BYTE& sharedStartupFailed,AtlBaseModuleNativeServices&);
void ReleaseAtlBaseModuleNativeAt53E949(WindowClassModuleNativeBody38h&,AtlBaseModuleNativeServices&);
HINSTANCE GetAtlBaseModuleInstanceNativeAt53EA87(const WindowClassModuleNativeBody38h&)noexcept;
HINSTANCE GetAtlBaseResourceInstanceNativeAt53EA42(WindowClassModuleNativeBody38h&,std::int32_t index,AtlBaseModuleNativeServices&);
// 低层地址53E98D/53E968/53EB0A的宿主参数仍借整个38h，实际只消费其+2Ch三DWORD数组。
HINSTANCE* AtlBaseResourceSlotNativeAt53E968(WindowClassModuleNativeBody38h&,std::int32_t index,AtlBaseModuleNativeServices&);
bool AppendAtlBaseResourceArrayNativeAt53E98D(WindowClassModuleNativeBody38h&,const HINSTANCE* source,AtlBaseModuleNativeServices&);
bool RemoveAtlBaseResourceArrayNativeAt53EB0A(WindowClassModuleNativeBody38h&,std::int32_t index,AtlBaseModuleNativeServices&);
bool AddAtlBaseResourceInstanceNativeAt53EA0F(WindowClassModuleNativeBody38h&,HINSTANCE,AtlBaseModuleNativeServices&);
bool RemoveAtlBaseResourceInstanceNativeAt53EB58(WindowClassModuleNativeBody38h&,HINSTANCE,AtlBaseModuleNativeServices&);
}
