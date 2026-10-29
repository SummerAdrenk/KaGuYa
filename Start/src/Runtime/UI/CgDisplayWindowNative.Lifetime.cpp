#include "CgDisplayWindowNative.Internal.h"
#include <new>

namespace KaGuYa::Start::Runtime
{
void DestroyCgDisplayWindowNative(CgDisplayWindowNativeBody60h& body) noexcept
{
    // 4A6030保留派生表到数组释放结束；不结束drag、不恢复registry、不补dirty。
    ReleaseSpriteReferenceAt460260(body.spritesAt24);
    DestroyLayerWindowNativeAt464910(body.window);
}
CgDisplayWindowNativeBody60h* DeleteCgDisplayWindowNativeAt4A6030(
    CgDisplayWindowNativeBody60h& body, std::uint32_t flags) noexcept
{
    DestroyCgDisplayWindowNative(body);
    auto* const result = &body;
    if (flags & 1u) ::operator delete(result, std::size_t{0x60});
    return result;
}
}
