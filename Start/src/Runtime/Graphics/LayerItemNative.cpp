#include <Runtime/LayerItemNative.h>
#include <Runtime/LayerListNative.h>

#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = LayerItemNativeBody14h;
using Table = LayerItemNativeVtable;

void __fastcall Draw(Body*, void*, const RECT*) {}
void __fastcall Composite(Body*, void*, GraphicsSurfaceNativeBody*, const RECT*) {}
Body* __fastcall Delete(Body* body, void*, std::uint32_t flags)
    { return DeleteLayerItemNativeAt441570(*body, flags); }

const Table methods{
    reinterpret_cast<decltype(Table::drawAt00)>(&Draw),
    reinterpret_cast<decltype(Table::compositeAt04)>(&Composite),
    reinterpret_cast<decltype(Table::deletingDestructorAt08)>(&Delete)};
}

const LayerItemNativeVtable& LayerItemNativeMethods() noexcept { return methods; }

LayerItemNativeBody14h* ConstructLayerItemNativeAt441270(Body& body, LayerListNativeBody18h* parent)
{
    body.vptrAt00 = &methods;
    body.valueAt04 = 0;
    body.valueAt08 = 0;
    body.enabledAt0C = 1;
    body.parentAt10 = parent;
    // 基类构造的注册失败直接外传，尚无441310注销资格。
    if (parent) parent->vptrAt00->registerAt00(parent, &body);
    return &body;
}

void DestroyLayerItemNativeAt441310(Body& body) noexcept
{
    auto* const parent = body.parentAt10;
    body.vptrAt00 = &methods;
    if (parent) parent->vptrAt00->unregisterAt04(parent, &body);
}

LayerItemNativeBody14h* DeleteLayerItemNativeAt441570(Body& body, std::uint32_t flags) noexcept
{
    // 441570/441310的EHFlags均为5；注销或最终释放异常不能返回普通caller。
    DestroyLayerItemNativeAt441310(body);
    auto* const result = &body;
    if (flags & 1u) ::operator delete(result, std::size_t{0x14});
    return result;
}

LayerItemNativeBody14h* DeleteLayerItemNativeDynamic(Body* body, std::uint32_t flags)
{
    return body ? body->vptrAt00->deletingDestructorAt08(body, flags) : nullptr;
}
}
