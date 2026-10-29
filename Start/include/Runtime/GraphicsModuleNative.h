#pragma once

#include <Runtime/EnvironmentRootRuntime.h>

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
// Win32叶的借用来源；不持有HMODULE，测试回调允许按原调用点传播异常。
class IGraphicsModuleNativeWin32Api
{
public:
    virtual ~IGraphicsModuleNativeWin32Api() = default;
    virtual HMODULE LoadLibraryA(const char* path) = 0;
    virtual FARPROC GetProcAddress(HMODULE module, const char* name) = 0;
    virtual BOOL FreeLibrary(HMODULE module) = 0;
};
IGraphicsModuleNativeWin32Api& DefaultGraphicsModuleNativeWin32Api() noexcept;

struct GraphicsModuleNativeBody8h;
struct GraphicsModuleNativeControl18h;

// 5B1D9C仅一槽；5B1DA0已经是字符串，不是第二个虚槽。
struct GraphicsModuleNativeVtable final
{
    GraphicsModuleNativeBody8h* (__thiscall* deletingDestructorAt00)(
        GraphicsModuleNativeBody8h*, std::uint32_t);
};
struct GraphicsModuleNativeBody8h final
{
    const GraphicsModuleNativeVtable* vptrAt00;
    HMODULE moduleAt04;
};

// 5B1E6C=[440E00,4404C0]；用显式ABI表保留删除槽的原指针返回值。
struct GraphicsModuleNativeControlVtable final
{
    void (__thiscall* destroyPayloadAt00)(GraphicsModuleNativeControl18h*);
    GraphicsModuleNativeControl18h* (__thiscall* deletingDestructorAt04)(
        GraphicsModuleNativeControl18h*, std::uint32_t);
};
struct GraphicsModuleNativeControl18h final
{
    const GraphicsModuleNativeControlVtable* vptrAt00;
    std::uint32_t referenceCountAt04;
    std::uint8_t owningAt08;
    std::uint8_t unwrittenAt09[3];
    std::uint32_t strideAt0C;
    std::uint32_t countAt10;
    GraphicsModuleNativeBody8h* payloadAt14;
};
static_assert(sizeof(GraphicsModuleNativeVtable) == 4);
static_assert(sizeof(GraphicsModuleNativeBody8h) == 8);
static_assert(alignof(GraphicsModuleNativeBody8h) == 4);
static_assert(offsetof(GraphicsModuleNativeBody8h, moduleAt04) == 4);
static_assert(sizeof(GraphicsModuleNativeControlVtable) == 8);
static_assert(offsetof(GraphicsModuleNativeControlVtable, deletingDestructorAt04) == 4);
static_assert(sizeof(GraphicsModuleNativeControl18h) == 0x18);
static_assert(alignof(GraphicsModuleNativeControl18h) == 4);
static_assert(offsetof(GraphicsModuleNativeControl18h, referenceCountAt04) == 4);
static_assert(offsetof(GraphicsModuleNativeControl18h, owningAt08) == 8);
static_assert(offsetof(GraphicsModuleNativeControl18h, unwrittenAt09) == 9);
static_assert(offsetof(GraphicsModuleNativeControl18h, strideAt0C) == 0x0C);
static_assert(offsetof(GraphicsModuleNativeControl18h, countAt10) == 0x10);
static_assert(offsetof(GraphicsModuleNativeControl18h, payloadAt14) == 0x14);
static_assert(std::is_trivial_v<GraphicsModuleNativeBody8h>);
static_assert(std::is_trivial_v<GraphicsModuleNativeControl18h>);

// 原虚槽没有服务参数；scope仅借用调用期服务，嵌套恢复，不缓存正文/handle或创建owner。
// 直接调用原表时必须装配此scope；下面显式带服务的入口自行建立调用期scope。
class GraphicsModuleNativeScope final
{
public:
    GraphicsModuleNativeScope(IEnvironmentRootMemory& memory,
        IGraphicsModuleNativeWin32Api& win32) noexcept;
    ~GraphicsModuleNativeScope() noexcept;
    GraphicsModuleNativeScope(const GraphicsModuleNativeScope&) = delete;
    GraphicsModuleNativeScope& operator=(const GraphicsModuleNativeScope&) = delete;
private:
    IEnvironmentRootMemory* previousMemory_;
    IGraphicsModuleNativeWin32Api* previousWin32_;
};

[[nodiscard]] const GraphicsModuleNativeVtable& GraphicsModuleNativeMethods() noexcept;
[[nodiscard]] const GraphicsModuleNativeControlVtable& GraphicsModuleNativeControlMethods() noexcept;
[[nodiscard]] const GraphicsModuleNativeControlVtable& GraphicsModuleNativeRetiredControlMethods() noexcept;

GraphicsModuleNativeBody8h* ConstructGraphicsModuleNativeAt4407D2(
    GraphicsModuleNativeBody8h& body) noexcept;
[[nodiscard]] GraphicsModuleNativeBody8h* CreateGraphicsModuleNativeAt4407C4(
    IEnvironmentRootMemory& memory);
GraphicsModuleNativeBody8h* DeleteGraphicsModuleNativeAt4404F0(
    GraphicsModuleNativeBody8h& body, std::uint32_t flags,
    IEnvironmentRootMemory& memory, IGraphicsModuleNativeWin32Api& win32);
GraphicsModuleNativeControl18h* DeleteGraphicsModuleControlAt4404C0(
    GraphicsModuleNativeControl18h& control, std::uint32_t flags,
    IEnvironmentRootMemory& memory);

// 4407E3先释放旧槽；44081B使用已为0的ECX，不补删回调晚填control。
// 新base先发布；18h分配失败不回滚8h正文，null控制块保留原后续无效写入。
void InstallGraphicsModuleReferenceAt4407E3(YtlReferenceSlotRuntime& slot,
    GraphicsModuleNativeBody8h* created, IEnvironmentRootMemory& memory,
    IGraphicsModuleNativeWin32Api& win32);
[[nodiscard]] GraphicsModuleNativeBody8h* ResolveGraphicsModuleReference(
    const YtlReferenceSlotRuntime& slot) noexcept;
void ReleaseGraphicsModuleReferenceAt440620(YtlReferenceSlotRuntime& slot,
    IEnvironmentRootMemory& memory, IGraphicsModuleNativeWin32Api& win32);

// 4406D5/440B88/440D75的名称CString边界。module已经借到，不在本函数重新Capture/retain。
// GetNil和Set成功后撤销展开清理资格，晚读module+04，解析后释放名称，再返回缓存函数。
// 允许module为空以保留原首次字段访问；缺导出原样返回0，不造诊断或替代函数。
[[nodiscard]] FARPROC ResolveGraphicsModuleExportWithCString(
    GraphicsModuleNativeBody8h* module, const char* name, std::int32_t length,
    IRuntimeStringManagerSource& strings, IGraphicsModuleNativeWin32Api& win32);
}
