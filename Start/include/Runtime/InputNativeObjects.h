#pragma once
#include <Runtime/InputRootNative.h>
#include <Platform/Input.h>

namespace KaGuYa::Start::Runtime
{
struct InputNativeServices;
struct WinHookNativeBody30h;
struct WinHookNativeServices;
class WinHookRootNativeOwner;
struct InputRawMessageVtable
{
    std::int32_t (__thiscall* dispatchAt00)(void*,HWND,UINT,WPARAM,LPARAM,LRESULT*,std::uint32_t);
};
struct MouseRawNativeBody10h final
{
    const InputRawMessageVtable* vptrAt00;
    std::uint8_t pressedAt04[3];
    std::uint8_t latchedAt07[3];
    std::int16_t wheelAt0A;
    std::int16_t remainderAt0C;
    std::uint8_t unwrittenAt0E[2];
};
struct KeyboardRawNativeBody104h final
{
    const InputRawMessageVtable* vptrAt00;
    std::uint8_t keysAt04[256];
};
struct CursorNativeBody10h final
{
    const InputRawMessageVtable* vptrAt00;
    HCURSOR currentAt04;
    HCURSOR previousAt08;
    HWND windowAt0C;
};
static_assert(sizeof(MouseRawNativeBody10h)==0x10&&offsetof(MouseRawNativeBody10h,wheelAt0A)==0x0A);
static_assert(sizeof(KeyboardRawNativeBody104h)==0x104&&offsetof(KeyboardRawNativeBody104h,keysAt04)==4);
static_assert(sizeof(CursorNativeBody10h)==0x10&&offsetof(CursorNativeBody10h,windowAt0C)==0x0C);

class IInputNativeHooks
{
public:
    virtual ~IInputNativeHooks()=default;
    virtual void Add(void*,Platform::WindowHookDispatcher::NativeCallback,void*)=0;
    virtual void Remove(void*)=0;
};
// 固定本次450E20选中的正文，方法调用不再捕获根或修改共享receiver缓存。
class InputNativeHookReceiver final
{
public:
    InputNativeHookReceiver(IInputNativeHooks& controlled) noexcept:controlled_(&controlled){}
    InputNativeHookReceiver(WinHookNativeBody30h* body,WinHookNativeServices& services) noexcept
        :body_(body),services_(&services){}
    void AddRaw(void*,InputNativeServices&) const;
    void RemoveRaw(void*) const;
private:
    IInputNativeHooks* controlled_=nullptr;
    WinHookNativeBody30h* body_=nullptr;
    WinHookNativeServices* services_=nullptr;
};
class IInputNativeHookSource
{
public:
    virtual ~IInputNativeHookSource()=default;
    virtual InputNativeHookReceiver CaptureAt450E20()=0;
};
// dispatcher中只登记原正文/回调/服务借用，没有另一个Mouse或Keyboard状态。
class InputNativeDispatcher final : public IInputNativeHooks,public IInputNativeHookSource
{
public:
    explicit InputNativeDispatcher(Platform::WindowHookDispatcher& value) noexcept:dispatcher_(value){}
    InputNativeHookReceiver CaptureAt450E20() override{return InputNativeHookReceiver(*this);}
    void Add(void* p,Platform::WindowHookDispatcher::NativeCallback f,void* c) override{dispatcher_.AddNative(p,f,c);}
    void Remove(void* p) override{dispatcher_.RemoveNative(p);}
private:
    Platform::WindowHookDispatcher& dispatcher_;
};
class InputNativeWinHookSource final:public IInputNativeHookSource
{
public:
    InputNativeWinHookSource(WinHookRootNativeOwner& owner,WinHookNativeServices& services) noexcept
        :owner_(owner),services_(services){}
    InputNativeHookReceiver CaptureAt450E20() override;
private:
    WinHookRootNativeOwner& owner_;
    WinHookNativeServices& services_;
};
class IInputNativePlatform
{
public:
    virtual ~IInputNativePlatform()=default;
    // 451210读取原全局hInstance，随后才根据system字节选择NULL。
    virtual HINSTANCE ReadModuleAt451210()=0;
    virtual SHORT GetKeyStateAt450F7C(int)=0;
};
struct InputNativeServices final
{
    IInputNativeHookSource& hooks;
    IInputNativePlatform& platform;
    const Platform::ICursorApi& cursor;
    IEnvironmentRootMemory& memory;
};
class InputNativeScope final
{
public:
    explicit InputNativeScope(InputNativeServices&) noexcept;
    ~InputNativeScope() noexcept;
    InputNativeScope(const InputNativeScope&)=delete;
    InputNativeScope& operator=(const InputNativeScope&)=delete;
private:
    InputNativeServices* previous_;
};
MouseManagerNativeBody3Ch* ConstructMouseManagerNativeAt44FE50(MouseManagerNativeBody3Ch&,InputNativeServices&);
KeyboardManagerNativeBody438h* ConstructKeyboardManagerNativeAt44FC70(KeyboardManagerNativeBody438h&,const std::uint8_t* table,InputNativeServices&);
CursorNativeBody10h* ConstructCursorNativeAt4508EB(CursorNativeBody10h&,HWND,InputNativeServices&);
MouseManagerNativeBody3Ch* DeleteMouseManagerNativeAt450730(MouseManagerNativeBody3Ch&,std::uint32_t) noexcept;
KeyboardManagerNativeBody438h* DeleteKeyboardManagerNativeAt450610(KeyboardManagerNativeBody438h&,std::uint32_t) noexcept;
MouseRawNativeBody10h* DeleteMouseRawNativeAt4506C0(MouseRawNativeBody10h&,std::uint32_t) noexcept;
KeyboardRawNativeBody104h* DeleteKeyboardRawNativeAt4505A0(KeyboardRawNativeBody104h&,std::uint32_t) noexcept;
CursorNativeBody10h* DeleteCursorNativeAt450530(CursorNativeBody10h&,std::uint32_t) noexcept;
std::int32_t DispatchMouseRawNativeAt451000(MouseRawNativeBody10h&,HWND,UINT,WPARAM,LPARAM,LRESULT*,std::uint32_t);
std::int32_t DispatchKeyboardRawNativeAt450F60(KeyboardRawNativeBody104h&,HWND,UINT,WPARAM,LPARAM,LRESULT*,std::uint32_t,InputNativeServices&);
std::int32_t DispatchCursorNativeAt450F30(CursorNativeBody10h&,HWND,UINT,WPARAM,LPARAM,LRESULT*,std::uint32_t,InputNativeServices&);
std::uint8_t SetCursorNativeAt451200(CursorNativeBody10h&,LPCSTR,std::uint8_t system,std::uint8_t force,InputNativeServices&);
void InitializeInputNativeAt4507E0(InputRootNativeBody24h&,HWND,const std::uint8_t* bindings,InputNativeServices&);
void ReleaseInputNativeAt451150(InputRootNativeBody24h&,InputNativeServices&);
// 44FA70/4500D0：三字节容器，包含大块32字节对齐和原容量增长规则。
void AppendKeyboardBindingNativeAt44FA70(EnvironmentVectorStorage<KeyboardBindingNative3>&,
    const KeyboardBindingNative3&,IEnvironmentRootMemory&);
void DestroyKeyboardBindingsNativeAt4500D0(EnvironmentVectorStorage<KeyboardBindingNative3>&,IEnvironmentRootMemory&);
const std::uint8_t* MamatomoKeyboardNativeTableAt601358() noexcept;

class IInputFrameNativeSource
{
public:
    virtual ~IInputFrameNativeSource()=default;
    virtual HWND ReadWindowAt450ADB()=0;
    virtual std::int32_t ReadWidthAt450AE6()=0;
    virtual std::int32_t ReadHeightAt450AF1()=0;
    virtual std::uint8_t ReadModeAt450B01()=0;
    virtual const std::int32_t* ReadViewportAt450B3F()=0;
    virtual const std::int32_t* ReadViewportAt450B7B()=0;
};
void UpdateInputNativeAt450AA0(InputRootNativeBody24h&,IInputFrameNativeSource&,
    const Platform::IMousePositionApi&,InputNativeServices&);
}
