#pragma once

#include <Runtime/GraphicsSurfaceNativeBody.h>

#include <type_traits>

namespace KaGuYa::Start::Runtime
{
class IGraphicsWindowSurfaceApi;
class IGuiControlResourceLoadServices;
class GraphicsSurfaceNativeApi;
struct ColorTransWindowNativeBody38h;

struct ColorTransWindowNativeVtable final
{
    ColorTransWindowNativeBody38h* (__thiscall* deletingDestructorAt00)(
        ColorTransWindowNativeBody38h*, std::uint32_t);
};

// 45E970：仅原38h正文；三份Ref没有自动拥有析构，+28/+30/+34不由构造写入。
struct ColorTransWindowNativeBody38h final
{
    const ColorTransWindowNativeVtable* vptrAt00;
    union { YtlReferenceSlotRuntime source1At04; };
    union { YtlReferenceSlotRuntime source2At10; };
    union { YtlReferenceSlotRuntime processedAt1C; };
    std::uint32_t unwrittenAt28;
    std::int32_t modeAt2C;
    std::uint32_t lastColorAt30;
    std::int32_t lastTransparencyAt34;
    ColorTransWindowNativeBody38h() noexcept {}
    ~ColorTransWindowNativeBody38h() noexcept {}
    ColorTransWindowNativeBody38h(const ColorTransWindowNativeBody38h&) = delete;
    ColorTransWindowNativeBody38h& operator=(const ColorTransWindowNativeBody38h&) = delete;
};
static_assert(sizeof(ColorTransWindowNativeVtable) == 4);
static_assert(sizeof(ColorTransWindowNativeBody38h) == 0x38);
static_assert(alignof(ColorTransWindowNativeBody38h) == 4);
static_assert(std::is_standard_layout_v<ColorTransWindowNativeBody38h>);
static_assert(offsetof(ColorTransWindowNativeBody38h, source1At04) == 4);
static_assert(offsetof(ColorTransWindowNativeBody38h, source2At10) == 0x10);
static_assert(offsetof(ColorTransWindowNativeBody38h, processedAt1C) == 0x1C);
static_assert(offsetof(ColorTransWindowNativeBody38h, unwrittenAt28) == 0x28);
static_assert(offsetof(ColorTransWindowNativeBody38h, modeAt2C) == 0x2C);
static_assert(offsetof(ColorTransWindowNativeBody38h, lastColorAt30) == 0x30);
static_assert(offsetof(ColorTransWindowNativeBody38h, lastTransparencyAt34) == 0x34);

class IColorTransWindowNativeClear
{
public:
    virtual ~IColorTransWindowNativeClear() = default;
    // 调用方已检查旧handle。实现必须先且仅一次4182F0，再从固定正文读取
    // clearColor/handle，最后向该次捕获的root提交443910；不重取processed Ref。
    virtual void ClearAfterCaptureAt46F745(GraphicsSurfaceNativeBody& fixedBody) = 0;
};

// 显式借用生产代理的body-aware叶，无默认singleton/owner，不额外Capture。
class ColorTransWindowNativeClear final : public IColorTransWindowNativeClear
{
public:
    explicit ColorTransWindowNativeClear(GraphicsSurfaceNativeApi& graphics) noexcept;
    void ClearAfterCaptureAt46F745(GraphicsSurfaceNativeBody& fixedBody) override;
private:
    GraphicsSurfaceNativeApi& graphics_;
};

struct ColorTransWindowNativeServices final
{
    IGraphicsSurfaceApi& surfaces;
    IGraphicsWindowSurfaceApi& drawing;
    IGuiControlResourceLoadServices& resources;
    IColorTransWindowNativeClear& clear;
    IRuntimeStringManagerSource& strings;
};

[[nodiscard]] const ColorTransWindowNativeVtable& ColorTransWindowNativeMethods() noexcept;
ColorTransWindowNativeBody38h* ConstructColorTransWindowNativeAt45E970(
    ColorTransWindowNativeBody38h& body, const char* firstName, const char* secondName,
    std::int32_t mode, ColorTransWindowNativeServices& services);
[[nodiscard]] ColorTransWindowNativeBody38h* NewColorTransWindowNativeAt45F384(
    const char* firstName, const char* secondName, std::int32_t mode,
    ColorTransWindowNativeServices& services);
// normal+14的38h Ref：旧槽先释放，新base先发布，control失败不补删已完成正文。
void AdoptColorTransWindowNativeAt45F3B7(YtlReferenceSlotRuntime& slot,
    ColorTransWindowNativeBody38h* created);
// 486A30普通调用可抛；删除入口为终止边界。两者销毁非空Surface时，调用方须覆盖
// GraphicsSurfaceNativeScope；构造/Apply自行建立其调用期Surface scope，不额外持有正文。
void ClearColorTransWindowNativeAt486A30(ColorTransWindowNativeBody38h& body);
void DestroyColorTransWindowNativeAt469730(ColorTransWindowNativeBody38h& body) noexcept;
ColorTransWindowNativeBody38h* DeleteColorTransWindowNativeAt469730(
    ColorTransWindowNativeBody38h& body, std::uint32_t flags) noexcept;
[[nodiscard]] std::uint8_t ApplyColorTransWindowNativeAt46F6D0(
    ColorTransWindowNativeBody38h& body, std::uint32_t color, std::int32_t transparency,
    ColorTransWindowNativeServices& services);

[[nodiscard]] GraphicsSurfaceNativeBody* ResolveColorTransSource1NativeAt47ADC0(
    const ColorTransWindowNativeBody38h& body) noexcept;
[[nodiscard]] GraphicsSurfaceNativeBody* ResolveColorTransSource2NativeAt47C770(
    const ColorTransWindowNativeBody38h& body) noexcept;
// processed为内联Ref解析；47DFE0属于normal pane，不冒用其地址命名本入口。
[[nodiscard]] GraphicsSurfaceNativeBody* ResolveColorTransProcessedNative(
    const ColorTransWindowNativeBody38h& body) noexcept;
}
