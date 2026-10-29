#include <Runtime/EnvironmentMusicNameRuntime.h>

#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
EnvironmentMusicNameRuntime::EnvironmentMusicNameRuntime(EnvironmentStartupRuntime& environment) noexcept
    : environment_(environment)
{
}

void EnvironmentMusicNameRuntime::WriteCurrentMusicNameAt2C(const char* const name)
{
    // 4D5A1E/25、4D5AEC/F3 晚取当前 slot2，再于 4D5A31/4D5B12 写 +2C；异常不吞掉。
    auto& destination = environment_.Archives().UnSaveData().StringAt2C();
    // 4D5AEC/F3取得目标之后，4D5AF8..5B10才读取源指针并扫描首NUL。
    const auto length = name ? static_cast<std::uint32_t>(std::strlen(name)) : 0u;
    SetRuntimeCStringAt437680(destination, name, std::bit_cast<std::int32_t>(length));
}
}
