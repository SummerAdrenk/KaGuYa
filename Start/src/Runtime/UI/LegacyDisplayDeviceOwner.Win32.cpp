#include <Runtime/LegacyDisplayDeviceApi.h>

namespace KaGuYa::Start::Runtime
{
BOOL Win32LegacyDisplayDeviceApi::Enumerate(const char* name, DWORD index, DEVMODEA& mode)
{ return modes_.Enumerate(name, index, mode); }
LONG Win32LegacyDisplayDeviceApi::Change(const char* name, DEVMODEA& mode, DWORD flags)
{ return modes_.Change(name, mode, flags); }
BOOL Win32LegacyDisplayDeviceApi::EnumerateDevice(DWORD index, DISPLAY_DEVICEA& device)
{ return ::EnumDisplayDevicesA(nullptr, index, &device, 0); }
LONG Win32LegacyDisplayDeviceApi::RestoreDesktop(const char* name)
{ return ::ChangeDisplaySettingsExA(name, nullptr, nullptr, 0, nullptr); }
}
