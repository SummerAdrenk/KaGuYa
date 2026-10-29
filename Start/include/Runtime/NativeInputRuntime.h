#pragma once
#include <Runtime/InputNativeObjects.h>
namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;
class RootRuntimeOwner;
// 只借唯一24h owner及正文外的服务；状态、Ref和绑定容器均由原正文持有。
class NativeInputRuntime final : public Platform::IInputRuntime,public Platform::IInputStartupRuntime
{
public:
    Platform::MouseView BorrowMouse() const override;
    Platform::KeyboardView BorrowKeyboard() const override;
    Platform::CursorView BorrowCursor() const override;
    void UpdateWithSource(IInputFrameNativeSource& source,const Platform::IMousePositionApi& position) override
    {UpdateInputNativeAt450AA0(*owner_.CaptureAt418210(),source,position,services_);}
    NativeInputRuntime(InputRootOwner& owner,InputNativeServices& services) noexcept:owner_(owner),services_(services){}
    InputRootNativeBody24h* CaptureAt418210(){return owner_.CaptureAt418210();}
    Platform::IInputRuntime& Input() noexcept override{return *this;}
    void InitializeAt4507E0(HWND window) override
    {InitializeAt4507E0(window,MamatomoKeyboardNativeTableAt601358());}
    void InitializeAt4507E0(HWND window,const std::uint8_t* table)
    {InitializeInputNativeAt4507E0(*CaptureAt418210(),window,table,services_);}
    void UpdateAt450AA0(IInputFrameNativeSource& source,const Platform::IMousePositionApi& position)
    {UpdateInputNativeAt450AA0(*CaptureAt418210(),source,position,services_);}
    void ReleaseAt451150() override{ReleaseInputNativeAt451150(*CaptureAt418210(),services_);}
    InputRootOwner& RootSource() noexcept{return owner_;}
    InputNativeServices& NativeServices() noexcept{return services_;}
private:
    InputRootOwner& owner_;
    InputNativeServices& services_;
};
class EnvironmentInputFrameNativeSource final:public IInputFrameNativeSource
{
public:
    EnvironmentInputFrameNativeSource(EnvironmentStartupRuntime& environment,RootRuntimeOwner& root) noexcept
        :environment_(environment),root_(root){}
    HWND ReadWindowAt450ADB() override;
    std::int32_t ReadWidthAt450AE6() override;
    std::int32_t ReadHeightAt450AF1() override;
    std::uint8_t ReadModeAt450B01() override;
    const std::int32_t* ReadViewportAt450B3F() override;
    const std::int32_t* ReadViewportAt450B7B() override;
private:
    EnvironmentStartupRuntime& environment_;
    RootRuntimeOwner& root_;
};
}
