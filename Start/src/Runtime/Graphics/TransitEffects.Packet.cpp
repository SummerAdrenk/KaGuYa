#include "TransitEffects.h"

#include <array>

namespace KaGuYa::Start::Runtime::TransitEffects
{
std::uint8_t BlendAt448CB0(const TransitOperation& operation, IGraphicsTransitApi& graphics)
{
    const auto current = operation.current;
    if (current <= 0) return 1;
    switch (operation.targetKind)
    {
    case 0:
        if (current >= 255)
            return graphics.TransitCopyDraw(Source(operation).handleAt04,
                operation.x, operation.y, nullptr, nullptr, operation.clip, 0);
        static_cast<void>(graphics.TransitCopyDraw(operation.previous,
            operation.x, operation.y, nullptr, nullptr, operation.clip, 0));
        // 448D82..448DA1：previous 的 AL 不门控后续；此时才读取 source 和新进度。
        return graphics.TransitBlendDraw(Source(operation).handleAt04,
            operation.x, operation.y, operation.current, nullptr, nullptr, operation.clip, 0);
    case 1:
        if (current >= 255)
            return graphics.TransitCopyBlit(operation.destination, Source(operation).handleAt04,
                operation.x, operation.y, nullptr, nullptr, operation.clip, 0);
        static_cast<void>(graphics.TransitCopyBlit(operation.destination, operation.previous,
            operation.x, operation.y, nullptr, nullptr, operation.clip, 0));
        return graphics.TransitBlendBlit(operation.destination, Source(operation).handleAt04,
            operation.x, operation.y, operation.current, nullptr, nullptr, operation.clip, 0);
    default:
        return 0;
    }
}

std::uint8_t UniversalAt44ED80(const TransitOperation& operation, IGraphicsTransitApi& graphics)
{
    const std::array<std::int32_t, 4> thresholds{
        operation.current, Add(operation.current, operation.range), 0, 255};
    // 44EDB2..44EE45：修改的是 source，不是 destination；在模块/targetKind 判断前读句柄。
    graphics.TransitUniversalBlit(Source(operation).handleAt04, operation.mask, thresholds.data(),
        operation.x, operation.y, nullptr, nullptr, operation.clip, 0);
    switch (operation.targetKind)
    {
    case 0:
        if (operation.current >= operation.target)
            static_cast<void>(graphics.TransitCopyDraw(operation.previous,
                operation.x, operation.y, nullptr, nullptr, operation.clip, 0));
        // 44EF62 再次取 source；不得跨 Universal/Copy 保存句柄或整个 packet 快照。
        return graphics.TransitDraw(Source(operation).handleAt04,
            operation.x, operation.y, nullptr, nullptr, operation.clip, 0);
    case 1:
        if (operation.current >= operation.target)
            static_cast<void>(graphics.TransitCopyBlit(operation.destination, operation.previous,
                operation.x, operation.y, nullptr, nullptr, operation.clip, 0));
        return graphics.TransitBlit(operation.destination, Source(operation).handleAt04,
            operation.x, operation.y, nullptr, nullptr, operation.clip, 0);
    default:
        return 0;
    }
}

std::uint8_t MosaicAt44DBF0(const TransitOperation& operation, IGraphicsTransitApi& graphics)
{
    const auto current = operation.current;
    if (current >= 512) return 0;
    // 44DC09..44DC18：DWORD 回绕后 SHR，负数不钳位，也不是算术右移。
    const auto size = static_cast<std::uint32_t>(current < 256 ? current : Sub(512, current)) >> 3;
    const auto source = current < 256 ? operation.previous : Source(operation).handleAt04;
    static_cast<void>(graphics.TransitCopyBlit(operation.destination, source,
        operation.x, operation.y, nullptr, nullptr, operation.clip, 0));
    if (size >= 2)
        graphics.TransitMosaic(operation.destination, size, operation.clip);
    // 44DCC0：到这里才选择目标分支；Copy/Mosaic 已发生，size 不随回调改写进度重算。
    switch (operation.targetKind)
    {
    case 0:
        return graphics.TransitCopyDraw(operation.destination,
            operation.x, operation.y, nullptr, nullptr, operation.clip, 0);
    case 1:
        return 1;
    default:
        return 0;
    }
}
}
