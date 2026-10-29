#pragma once

#include <Runtime/RuntimeCString.h>
#include <Runtime/EnvironmentObjectReference.h>

namespace KaGuYa::Start::Runtime
{
class IGraphicsSurfaceApi;
struct GraphicsSurfaceNativeBody;
struct GraphicsSurfaceNativeVtable final
{
    GraphicsSurfaceNativeBody* (__thiscall* deletingDestructor)(GraphicsSurfaceNativeBody*, std::uint32_t);
};

// 40C2E0/444B40：正文严格24h，外部DLL服务与分配器不得占用原字段。
struct GraphicsSurfaceNativeBody final
{
    explicit GraphicsSurfaceNativeBody(IGraphicsSurfaceApi& api,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    ~GraphicsSurfaceNativeBody() noexcept;
    GraphicsSurfaceNativeBody(const GraphicsSurfaceNativeBody&) = delete;
    GraphicsSurfaceNativeBody& operator=(const GraphicsSurfaceNativeBody&) = delete;
    // 仅供现代vector适配器转移存储；原24h Ref工厂不调用此入口。
    struct ModernMoveTag final {};
    GraphicsSurfaceNativeBody(ModernMoveTag, GraphicsSurfaceNativeBody& source) noexcept;

    void ResetAt432670(IGraphicsSurfaceApi& api);
    [[nodiscard]] bool CreateAt415010(IGraphicsSurfaceApi& api,
        std::int32_t width, std::int32_t height, std::int32_t alpha, std::int32_t clear);
    [[nodiscard]] std::int32_t Handle() const noexcept { return handleAt04; }
    [[nodiscard]] std::int32_t Width() const noexcept { return widthAt08; }
    [[nodiscard]] std::int32_t Height() const noexcept { return heightAt0C; }
    [[nodiscard]] bool HasAlpha() const noexcept { return alphaAt18 != 0; }

    const GraphicsSurfaceNativeVtable* vptrAt00;
    std::int32_t handleAt04;
    std::int32_t widthAt08;
    std::int32_t heightAt0C;
    std::int32_t originXAt10;
    std::int32_t originYAt14;
    std::uint8_t alphaAt18;
    std::uint8_t unwrittenAt19[3];
    union { RuntimeCString nameAt1C; };
    std::uint32_t clearColorAt20;
};
static_assert(sizeof(GraphicsSurfaceNativeBody) == 0x24);
static_assert(alignof(GraphicsSurfaceNativeBody) == 4);
static_assert(offsetof(GraphicsSurfaceNativeBody, handleAt04) == 4);
static_assert(offsetof(GraphicsSurfaceNativeBody, widthAt08) == 8);
static_assert(offsetof(GraphicsSurfaceNativeBody, heightAt0C) == 0x0C);
static_assert(offsetof(GraphicsSurfaceNativeBody, originXAt10) == 0x10);
static_assert(offsetof(GraphicsSurfaceNativeBody, originYAt14) == 0x14);
static_assert(offsetof(GraphicsSurfaceNativeBody, alphaAt18) == 0x18);
static_assert(offsetof(GraphicsSurfaceNativeBody, nameAt1C) == 0x1C);
static_assert(offsetof(GraphicsSurfaceNativeBody, clearColorAt20) == 0x20);

// 原删除虚槽无服务参数；现代宿主在调用边界提供服务，正文不缓存singleton镜像。
// 此scope不持有Surface，也不作为4182F0完整singleton已还原的证明。
class GraphicsSurfaceNativeScope final
{
public:
    explicit GraphicsSurfaceNativeScope(IGraphicsSurfaceApi& api) noexcept;
    // 可显式清空上下文；只有真正销毁非空handle时才要求API，不能提前拒绝无control/非owning槽。
    explicit GraphicsSurfaceNativeScope(IGraphicsSurfaceApi* api) noexcept;
    ~GraphicsSurfaceNativeScope() noexcept;
    GraphicsSurfaceNativeScope(const GraphicsSurfaceNativeScope&) = delete;
    GraphicsSurfaceNativeScope& operator=(const GraphicsSurfaceNativeScope&) = delete;
private:
    IGraphicsSurfaceApi* previous_;
};

// 407A80：旧槽先释放，新base先发布，18h control失败不回滚24h正文。
void AdoptGraphicsSurfaceAt407A80(YtlReferenceSlotRuntime& slot, GraphicsSurfaceNativeBody* body);
// 451500只构造尚未拥有资源的返回槽，不调用407A80的旧槽释放。
void ConstructGraphicsSurfaceReferenceAt451500(YtlReferenceSlotRuntime& out,
    GraphicsSurfaceNativeBody* body, std::uint8_t owning);
[[nodiscard]] GraphicsSurfaceNativeBody* NewGraphicsSurfaceAt40C2E0(IGraphicsSurfaceApi& api,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
