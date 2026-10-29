#include <Runtime/NativeInputRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <bit>
namespace KaGuYa::Start::Runtime
{
HWND EnvironmentInputFrameNativeSource::ReadWindowAt450ADB()
{return reinterpret_cast<HWND>(environment_.CaptureAt4180C0().windowAt2C);}
std::int32_t EnvironmentInputFrameNativeSource::ReadWidthAt450AE6()
{return std::bit_cast<std::int32_t>(environment_.CaptureAt4180C0().widthAt18);}
std::int32_t EnvironmentInputFrameNativeSource::ReadHeightAt450AF1()
{return std::bit_cast<std::int32_t>(environment_.CaptureAt4180C0().heightAt1C);}
std::uint8_t EnvironmentInputFrameNativeSource::ReadModeAt450B01()
{root_.EnsureCreatedAt413BE0();return root_.RequirePublished().WindowModeAt0C().ModeAt04();}
const std::int32_t* EnvironmentInputFrameNativeSource::ReadViewportAt450B3F()
{root_.EnsureCreatedAt413BE0();return root_.RequirePublished().WindowModeAt0C().RequireViewportAt0C().data();}
const std::int32_t* EnvironmentInputFrameNativeSource::ReadViewportAt450B7B()
{root_.EnsureCreatedAt413BE0();return root_.RequirePublished().WindowModeAt0C().RequireViewportAt0C().data();}
}
