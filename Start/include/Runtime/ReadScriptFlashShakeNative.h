#pragma once

#include <Runtime/ReadScriptEffectNative.h>
#include <Runtime/ByteArrayReferenceRuntime.h>
#include <Runtime/GraphicsSurfaceNativeBody.h>

#include <type_traits>

namespace KaGuYa::Start::Runtime
{
class IGraphicsWindowSurfaceApi;
class IGraphicsScreenCalls;
class IGraphicsScreenSizeApi;
class IGraphicsSurfaceTransitionApi;
class IGuiControlResourceLoadServices;
class IDrawManagerNativeRootSource;

// 49BCA0/5B4D78：三个原Ref12；名称只借char*，不存CString或额外拥有者。
// union阻止Ref默认初始化提前写入；+05..07及失败前尚未写入的+2C/+30保持原字节。
struct ReadScriptFlashNativeBody34h final
{
    ReadScriptEffectNativeBase8h baseAt00;
    union { YtlReferenceSlotRuntime flashAt08; };
    union { YtlReferenceSlotRuntime previousAt14; };
    union { ByteArrayReferenceRuntime intervalsAt20; };
    std::int32_t remainingAt2C;
    std::int32_t indexAt30;
    ReadScriptFlashNativeBody34h() noexcept {}
    ~ReadScriptFlashNativeBody34h() noexcept {}
    ReadScriptFlashNativeBody34h(const ReadScriptFlashNativeBody34h&) = delete;
    ReadScriptFlashNativeBody34h& operator=(const ReadScriptFlashNativeBody34h&) = delete;
};
struct ReadScriptShakeNativeBody28h final
{
    ReadScriptEffectNativeBase8h baseAt00;
    union { YtlReferenceSlotRuntime previousAt08; };
    std::int32_t intervalAt14;
    std::int32_t modeAt18;
    std::int32_t counterAt1C;
    std::int32_t indexAt20;
    std::int32_t scaleAt24;
    ReadScriptShakeNativeBody28h() noexcept {}
    ~ReadScriptShakeNativeBody28h() noexcept {}
    ReadScriptShakeNativeBody28h(const ReadScriptShakeNativeBody28h&) = delete;
    ReadScriptShakeNativeBody28h& operator=(const ReadScriptShakeNativeBody28h&) = delete;
};
static_assert(sizeof(ReadScriptFlashNativeBody34h) == 0x34);
static_assert(alignof(ReadScriptFlashNativeBody34h) == 4);
static_assert(std::is_standard_layout_v<ReadScriptFlashNativeBody34h>);
static_assert(offsetof(ReadScriptFlashNativeBody34h, flashAt08) == 8);
static_assert(offsetof(ReadScriptFlashNativeBody34h, previousAt14) == 0x14);
static_assert(offsetof(ReadScriptFlashNativeBody34h, intervalsAt20) == 0x20);
static_assert(offsetof(ReadScriptFlashNativeBody34h, remainingAt2C) == 0x2C);
static_assert(offsetof(ReadScriptFlashNativeBody34h, indexAt30) == 0x30);
static_assert(sizeof(ReadScriptShakeNativeBody28h) == 0x28);
static_assert(alignof(ReadScriptShakeNativeBody28h) == 4);
static_assert(std::is_standard_layout_v<ReadScriptShakeNativeBody28h>);
static_assert(offsetof(ReadScriptShakeNativeBody28h, previousAt08) == 8);
static_assert(offsetof(ReadScriptShakeNativeBody28h, intervalAt14) == 0x14);
static_assert(offsetof(ReadScriptShakeNativeBody28h, modeAt18) == 0x18);
static_assert(offsetof(ReadScriptShakeNativeBody28h, counterAt1C) == 0x1C);
static_assert(offsetof(ReadScriptShakeNativeBody28h, indexAt20) == 0x20);
static_assert(offsetof(ReadScriptShakeNativeBody28h, scaleAt24) == 0x24);

class IReadScriptFlashShakeNativeSource
{
public:
    virtual ~IReadScriptFlashShakeNativeSource() = default;
    // 各自保留原413BE0/Draw Ref解析点；不能在构造期缓存同一个receiver。
    virtual bool LoadFlashAt480000(YtlReferenceSlotRuntime& output, const char* name) = 0;
    // output是尚未拥有资源的返回存储；返回之后caller才取得临时Ref的EH释放资格。
    virtual YtlReferenceSlotRuntime* CaptureScreenAt4ACDD0(YtlReferenceSlotRuntime& output) = 0;
};

// 真实原生来源：480000惰建Surface24h并调用4800A0；4ACDD0复用已核原生快照函数。
// loader和draw必须借同一RootRuntimeOwner。图形可直接接NativeGraphicsRuntimeLibrary，
// 不构造RuntimeGraphicsSurface，不把Ref转换为shared_ptr。
class ReadScriptFlashShakeNativeSource final : public IReadScriptFlashShakeNativeSource
{
public:
    ReadScriptFlashShakeNativeSource(IGuiControlResourceLoadServices& loader,
        IDrawManagerNativeRootSource& draw, IGraphicsSurfaceApi& surfaces,
        IGraphicsScreenSizeApi& sizes, IGraphicsSurfaceTransitionApi& capture,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    bool LoadFlashAt480000(YtlReferenceSlotRuntime& output, const char* name) override;
    YtlReferenceSlotRuntime* CaptureScreenAt4ACDD0(YtlReferenceSlotRuntime& output) override;
private:
    IGuiControlResourceLoadServices& loader_;
    IDrawManagerNativeRootSource& draw_;
    IGraphicsSurfaceApi& surfaces_;
    IGraphicsScreenSizeApi& sizes_;
    IGraphicsSurfaceTransitionApi& capture_;
    IRuntimeStringManagerSource& strings_;
};

struct ReadScriptFlashShakeNativeServices final
{
    IGraphicsSurfaceApi& surfaces;
    IGraphicsWindowSurfaceApi& drawing;
    IGraphicsScreenCalls& screen;
};
// 正文外借用服务，同时绑定Surface删除scope。须覆盖安装时旧Effect退役及main2释放。
// 原生drawing/screen各自取Graphics根、按module门控，不能另加ModuleLoaded预查询。
// 空Draw、Ready、Tick早退及不含资源的未ready删除无需服务；不额外捕获根。
class ReadScriptFlashShakeNativeScope final
{
public:
    explicit ReadScriptFlashShakeNativeScope(ReadScriptFlashShakeNativeServices&) noexcept;
    ~ReadScriptFlashShakeNativeScope() noexcept;
    ReadScriptFlashShakeNativeScope(const ReadScriptFlashShakeNativeScope&) = delete;
    ReadScriptFlashShakeNativeScope& operator=(const ReadScriptFlashShakeNativeScope&) = delete;
private:
    ReadScriptFlashShakeNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
};

const ReadScriptEffectNativeVtable& ReadScriptFlashNativeMethods() noexcept;
const ReadScriptEffectNativeVtable& ReadScriptShakeNativeMethods() noexcept;
ReadScriptFlashNativeBody34h* ConstructReadScriptFlashNativeAt49BCA0(ReadScriptFlashNativeBody34h&,
    const char* name, std::int32_t count, IReadScriptFlashShakeNativeSource&, ReadScriptFlashShakeNativeServices&);
ReadScriptShakeNativeBody28h* ConstructReadScriptShakeNativeAt49D4E0(ReadScriptShakeNativeBody28h&,
    std::int32_t mode, std::int32_t scale, std::int32_t interval,
    IReadScriptFlashShakeNativeSource&, ReadScriptFlashShakeNativeServices&);
// 仅申请正文；外层以InstallReadScriptEffectNative(...,0x34/0x28)安装，禁止再套现代owner。
[[nodiscard]] ReadScriptFlashNativeBody34h* NewReadScriptFlashNative(const char* name, std::int32_t count,
    IReadScriptFlashShakeNativeSource&, ReadScriptFlashShakeNativeServices&);
[[nodiscard]] ReadScriptShakeNativeBody28h* NewReadScriptShakeNative(std::int32_t mode,
    std::int32_t scale, std::int32_t interval, IReadScriptFlashShakeNativeSource&, ReadScriptFlashShakeNativeServices&);
std::uint8_t TickReadScriptFlashNativeAt4BB7F0(ReadScriptFlashNativeBody34h&);
std::uint8_t TickReadScriptShakeNativeAt4BBDF0(ReadScriptShakeNativeBody28h&);
void DestroyReadScriptFlashNativeAt4A4080(ReadScriptFlashNativeBody34h&) noexcept;
ReadScriptFlashNativeBody34h* DeleteReadScriptFlashNativeAt4A64B0(ReadScriptFlashNativeBody34h&, std::uint32_t flags) noexcept;
// Shake原删除槽内联完成业务，没有独立Finish；flags=0仍完成并退役成员。
ReadScriptShakeNativeBody28h* DeleteReadScriptShakeNativeAt4A65F0(ReadScriptShakeNativeBody28h&, std::uint32_t flags) noexcept;
// Shake仅使用602120的十五项固定轨迹，不读写rand/srand或新建随机owner。
// mode越出0..14及轨迹负索引的相邻PE数据未还原，不将其转换为现代受控成功分支。
}
