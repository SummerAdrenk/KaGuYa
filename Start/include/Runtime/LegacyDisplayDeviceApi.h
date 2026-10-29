#pragma once
#include <Runtime/LegacyDisplaySettingsPlatform.h>

namespace KaGuYa::Start::Runtime
{
// 增量叶接口：既有 receiver 和模式探针仍只依赖 ILegacyDisplayModeApi。
// EnumerateDevice 固定枚举顶层适配器(NULL,index,flags=0)，FALSE 为枚举结束。
class ILegacyDisplayDeviceApi : public ILegacyDisplayModeApi
{
public:
    virtual BOOL EnumerateDevice(DWORD index, DISPLAY_DEVICEA&) = 0;
    // 01:4172C7 / 03:419817：lpDevMode=NULL、hwnd=NULL、flags=0、lParam=NULL。
    virtual LONG RestoreDesktop(const char* deviceName) = 0;
};

class Win32LegacyDisplayDeviceApi final : public ILegacyDisplayDeviceApi
{
public:
    BOOL Enumerate(const char*, DWORD, DEVMODEA&) override;
    LONG Change(const char*, DEVMODEA&, DWORD) override;
    BOOL EnumerateDevice(DWORD, DISPLAY_DEVICEA&) override;
    LONG RestoreDesktop(const char*) override;
private:
    Win32LegacyDisplayModeApi modes_;
};
}
