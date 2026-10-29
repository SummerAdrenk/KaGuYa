#include "CgDisplayWindowNative.Internal.h"
#include <new>

namespace KaGuYa::Start::Runtime
{
CgDisplayWindowNativeBody60h* ConstructCgDisplayWindowNativeAt499A20(
    CgDisplayWindowNativeBody60h& body, SIZE size, std::int32_t count,
    LayerListNativeBody18h* parent, IRuntimeStringManagerSource& strings)
{
    // 注册失败尚未完成基类构造，不具备441310注销资格。
    ConstructLayerItemNativeAt441270(body.window.layer, parent);
    body.window.boundsAt14 = {};
    body.window.layer.vptrAt00 = CgDisplayWindowNativeMethods().AsBase();
    ::new (static_cast<void*>(&body.spritesAt24)) YtlReferenceSlotRuntime{};
    body.maximumXAt38 = 0;
    body.maximumYAt3C = 0;
    body.minimumXAt40 = 0;
    body.minimumYAt44 = 0;
    body.draggingAt54 = 0;
    body.lastPointerAt58 = {};
    // +30/+34/+48/+4C/+50及padding没有构造写入，不能整块值初始化。
    body.window.boundsAt14 = {0, 0, size.cx, size.cy};
    try
    {
        auto* const sprites = NewSpriteArrayAt45B327(std::uint32_t(count), strings);
        InstallSpriteArrayReferenceAt45B361(body.spritesAt24, sprites, count);
    }
    catch (...)
    {
        // 5EEAFC: try=[2,3], catch=499BD9。只清RECT，继续返回已注册窗口。
        // 元素失败的逆序析构/raw回收由原数组工厂完成；control失败不补删数组。
        body.window.boundsAt14 = {};
    }
    return &body;
}
}
