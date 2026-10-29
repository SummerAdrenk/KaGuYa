#pragma once
#include <Runtime/PropertyPageNative.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/SoundManagerNativeRuntime.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
struct PropertyPageSoundVolumePair8h final { int textAt00, sliderAt04; };
struct PropertyPageSoundVoiceParam10h final
{
    int characterAt00;
    union { RuntimeCString masterAt04; };
    union { RuntimeCString keyAt08; };
    DWORD volumeAt0C;
    PropertyPageSoundVoiceParam10h() noexcept {}
    // 两个CString仅由43CEA0手工退役，union宿主析构不取得额外释放资格。
    ~PropertyPageSoundVoiceParam10h() noexcept {}
};
// 40BADD..40BBDC。未写区不能通过值初始化变成设置默认值；服务不进入原正文。
struct PropertyPageSoundNativeBodyA8h final
{
    union { PropertyPageNativeBody54h pageAt00; };
    BYTE unwrittenAt54[4];
    EnvironmentVectorStorage<YtlReferenceSlotRuntime> voicesAt58;
    EnvironmentVectorStorage<PropertyPageSoundVolumePair8h> volumesAt64;
    BYTE musicAt70, effectsAt71, voiceAt72, lowerAt73, waitAt74, lowerEnabledAt75;
    BYTE unwrittenAt76[2];
    union { EditWindowEndpointNativeBody24h listAt78; };
    HWND treeAt9C;
    RuntimeCString* nameAtA0;
    RuntimeCString* surnameAtA4;
    PropertyPageSoundNativeBodyA8h() noexcept {}
};
static_assert(sizeof(PropertyPageSoundVolumePair8h)==8);
static_assert(sizeof(PropertyPageSoundVoiceParam10h)==0x10);
static_assert(sizeof(PropertyPageSoundNativeBodyA8h)==0xA8);
static_assert(offsetof(PropertyPageSoundNativeBodyA8h,voicesAt58)==0x58);
static_assert(offsetof(PropertyPageSoundNativeBodyA8h,volumesAt64)==0x64);
static_assert(offsetof(PropertyPageSoundNativeBodyA8h,musicAt70)==0x70);
static_assert(offsetof(PropertyPageSoundNativeBodyA8h,listAt78)==0x78);
static_assert(offsetof(PropertyPageSoundNativeBodyA8h,treeAt9C)==0x9C);
static_assert(offsetof(PropertyPageSoundNativeBodyA8h,nameAtA0)==0xA0);
static_assert(offsetof(PropertyPageSoundNativeBodyA8h,surnameAtA4)==0xA4);

// Win32薄叶；测试可只实现本次路径消费的方法，生产每项均直接调用对应API。
class IPropertyPageSoundNativeApi : public IEditWindowNativeMessageApi
{
public:
    virtual HWND GetDlgItem(HWND,int)=0;
    virtual HWND GetParent(HWND)=0;
    virtual LRESULT SendMessageA(HWND,UINT,WPARAM,LPARAM)=0;
    virtual BOOL EnableWindow(HWND,BOOL)=0;
    virtual int GetWindowTextLengthA(HWND)=0;
    virtual int GetWindowTextA(HWND,char*,int)=0;
    virtual BOOL SetWindowTextA(HWND,const char*)=0;
    virtual DWORD GetMessagePos()=0;
    virtual int MapWindowPoints(HWND,HWND,POINT*,UINT)=0;
    virtual SHORT GetKeyState(int)=0;
    virtual HWND GetFocus()=0;
    virtual BOOL IsWindowEnabled(HWND)=0;
    virtual DWORD GetSysColor(int)=0;
    virtual BOOL GetWindowRect(HWND,RECT*)=0;
    virtual int GetSystemMetrics(int)=0;
    virtual HIMAGELIST ImageList_LoadImageA(HINSTANCE,LPCSTR,int,int,COLORREF,UINT,UINT)=0;
    virtual void InstallSoundTreeStateImages(HWND tree);
};
IPropertyPageSoundNativeApi& DefaultPropertyPageSoundNativeApi() noexcept;

class IPropertyPageSoundNativeSource
{
public:
    virtual ~IPropertyPageSoundNativeSource()=default;
    virtual EnvironmentRootStorage* EnvironmentAt4180C0()=0;
    // 4216F0内联锁/懒建站点，不能与相邻4180C0查询合并。
    virtual EnvironmentRootStorage* EnvironmentInlineAt6198C8()=0;
    virtual void EnsureRootAt413BE0()=0;
    virtual void* RootAt6198C4()=0;
    virtual YtlReferenceSlotRuntime& SoundAt24(void*)=0;
    virtual void StopAt4383B0(SoundManagerNativeBody50h*,DWORD)=0;
    virtual void ClearAt4384D0(SoundManagerNativeBody50h*)=0;
    virtual bool PlayingAt419370(SoundManagerNativeBody50h*,DWORD)=0;
    virtual void SetVolumeAt438170(int handle,int maximum,int value)=0;
    virtual void OpenAt42BC90(SoundManagerNativeBody50h*,DWORD,const char*,int,int,BYTE)=0;
};
// 真正Root/Env/Sound适配器。42BC90依赖必须显式提供实际open核，不伪造试听成功。
class RootPropertyPageSoundNativeSource final : public IPropertyPageSoundNativeSource
{
public:
    RootPropertyPageSoundNativeSource(RootRuntimeOwner&,EnvironmentRootOwner&,
        SoundManagerNativeRuntime&,ISoundManagerNativeOpen&) noexcept;
    EnvironmentRootStorage* EnvironmentAt4180C0() override;
    EnvironmentRootStorage* EnvironmentInlineAt6198C8() override;
    void EnsureRootAt413BE0() override;
    void* RootAt6198C4() override;
    YtlReferenceSlotRuntime& SoundAt24(void*) override;
    void StopAt4383B0(SoundManagerNativeBody50h*,DWORD) override;
    void ClearAt4384D0(SoundManagerNativeBody50h*) override;
    bool PlayingAt419370(SoundManagerNativeBody50h*,DWORD) override;
    void SetVolumeAt438170(int,int,int) override;
    void OpenAt42BC90(SoundManagerNativeBody50h*,DWORD,const char*,int,int,BYTE) override;
private:
    RootRuntimeOwner& root_;
    EnvironmentRootOwner& environment_;
    SoundManagerNativeRuntime& sounds_;
    ISoundManagerNativeOpen& open_;
};
struct PropertyPageSoundNativeServices final
{
    IPropertyPageSoundNativeSource& source;
    IPropertyPageSoundNativeApi& api;
    DialogEndpointNativeServices& endpoints;
    WindowClassModuleNativeBody38h& module;
    DWORD& sheetButtonsAt6197EC;
    IRuntimeStringManagerSource& strings;
    IEnvironmentRootMemory& memory;
};
class PropertyPageSoundNativeScope final
{
public:
    explicit PropertyPageSoundNativeScope(PropertyPageSoundNativeServices&) noexcept;
    ~PropertyPageSoundNativeScope() noexcept;
    PropertyPageSoundNativeScope(const PropertyPageSoundNativeScope&)=delete;
    PropertyPageSoundNativeScope& operator=(const PropertyPageSoundNativeScope&)=delete;
private:
    PropertyPageSoundNativeServices* previous_;
};
PropertyPageSoundNativeServices& RequirePropertyPageSoundNativeServices();
const DialogEndpointNativeVtable4& PropertyPageSoundNativeMethods() noexcept; // 5AE6A8
const EditWindowEndpointNativeVtable4& PropertyPageSoundCheckListNativeMethods() noexcept; // 5ADD04
void InitializePropertyPageSoundNativeAt40BADD(PropertyPageSoundNativeBodyA8h&,
    WindowClassModuleNativeBody38h&) noexcept;
void DestroyPropertyPageSoundNativeAt40EB10(PropertyPageSoundNativeBodyA8h&,PropertyPageSoundNativeServices&) noexcept;
UINT CALLBACK PropertyPageSoundCreationNativeAt430E60(HWND,UINT,PropertyPageNativeHeader34h*);
BOOL ProcessPropertyPageSoundNativeAt42FF30(PropertyPageSoundNativeBodyA8h&,HWND,UINT,WPARAM,LPARAM,LRESULT*,DWORD,
    PropertyPageSoundNativeServices&);
BOOL ProcessPropertyPageSoundCheckListNativeAt42DAB0(EditWindowEndpointNativeBody24h&,HWND,UINT,WPARAM,LPARAM,LRESULT*,DWORD,
    IPropertyPageSoundNativeApi&);
LRESULT InitializePropertyPageSoundControlsNativeAt425690(PropertyPageSoundNativeBodyA8h&,PropertyPageSoundNativeServices&);
BOOL ApplyPropertyPageSoundNativeAt4216F0(PropertyPageSoundNativeBodyA8h&,PropertyPageSoundNativeServices&);
// CStringToNum原算法；invalid输入返回0，32位乘加回绕，不使用strtol的语义。
int ParsePropertyPageSoundNumberNativeAt40BEB0(const char*,int length) noexcept;
}
