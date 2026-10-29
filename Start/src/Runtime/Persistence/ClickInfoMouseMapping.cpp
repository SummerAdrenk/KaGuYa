#include <Runtime/ClickInfoMouseMapping.h>
#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/SaveDataEnvironmentArchive.h>

#include <cstring>

namespace KaGuYa::Start::Runtime
{
void ApplyClickInfoMouseMapping(
    const SaveDataEnvironmentArchive& save, ClickInfoArchive& click) noexcept
{
    std::uint32_t mode;
    std::memcpy(&mode, save.bytesAt8C.data(), sizeof(mode));
    // x86 原码每次只覆盖组首 DWORD，后 DWORD 和 +04 整组必须保留。
    const auto write = [](std::array<std::uint8_t, 8>& group,
                          const std::uint32_t value) noexcept {
        std::memcpy(group.data(), &value, sizeof(value));
    };
    // 477AB9..477BBD：无符号完整 DWORD；未知模式不写，不先清空。
    switch (mode)
    {
    case 0:
        write(click.bytesAt0C, 1);
        write(click.bytesAt34, 0);
        write(click.bytesAt14, 3);
        write(click.bytesAt24, 0);
        write(click.bytesAt1C, 0);
        write(click.bytesAt2C, 0);
        break;
    case 1:
        write(click.bytesAt0C, 0);
        write(click.bytesAt14, 1);
        write(click.bytesAt34, 0);
        write(click.bytesAt24, 0);
        write(click.bytesAt1C, 0);
        write(click.bytesAt2C, 0);
        break;
    case 2:
        write(click.bytesAt0C, 0);
        write(click.bytesAt14, 3);
        write(click.bytesAt24, 1);
        write(click.bytesAt34, 0);
        write(click.bytesAt1C, 0);
        write(click.bytesAt2C, 0);
        break;
    case 3:
        write(click.bytesAt0C, 0);
        write(click.bytesAt14, 3);
        write(click.bytesAt1C, 1);
        write(click.bytesAt24, 0);
        write(click.bytesAt34, 0);
        write(click.bytesAt2C, 0);
        break;
    case 4:
        write(click.bytesAt0C, 0);
        write(click.bytesAt14, 3);
        write(click.bytesAt1C, 0);
        write(click.bytesAt24, 0);
        write(click.bytesAt2C, 1);
        write(click.bytesAt34, 0);
        break;
    case 5:
        write(click.bytesAt0C, 0);
        write(click.bytesAt34, 1);
        write(click.bytesAt14, 3);
        write(click.bytesAt24, 0);
        write(click.bytesAt1C, 0);
        write(click.bytesAt2C, 0);
        break;
    default:
        break;
    }
}
}
