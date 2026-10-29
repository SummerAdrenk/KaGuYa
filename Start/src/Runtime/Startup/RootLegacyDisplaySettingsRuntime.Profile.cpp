#include <Runtime/RootLegacyDisplaySettingsRuntime.h>
#include <Runtime/GameBuildProfile.h>

namespace KaGuYa::Start::Runtime
{
// 字段由主线追加，01/02/03显式启用；不按编号/资源存在性猜兼容模式。
LegacyDisplaySettingsFamily CompiledLegacyDisplaySettingsFamily() noexcept
{ return CompiledGameProfile().legacyDisplaySettings; }
}
