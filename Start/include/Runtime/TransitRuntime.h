#pragma once

#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/ScrVmProgramHandler.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
// 4C57F0 的完整 0x30 参数包；source 是 CSurface，其余三个图像字段是 DLL 句柄。
// 只借原24h正文，不拥有图像，不包含调用者的步长或 Present。
struct TransitOperation final
{
    GraphicsSurfaceHandle destination = 0;
    GraphicsSurfaceNativeBody* source = nullptr;
    GraphicsSurfaceHandle previous = 0;
    GraphicsSurfaceHandle mask = 0;
    std::int32_t x = 0;
    std::int32_t y = 0;
    const RECT* clip = nullptr;
    std::int32_t opcode = -1;
    std::int32_t targetKind = 0;
    std::int32_t current = 0;
    std::int32_t target = 255;
    std::int32_t range = 0;
};

// 443A00/44EC00/44EC80/44ED00：只按模块存在门控，保留 DLL 返回的 AL。
// 不借用丢弃返回值的 Pattern filter 或额外要求 Initialized 的直接图形接口。
class IGraphicsTransitApi
{
public:
    virtual ~IGraphicsTransitApi() = default;
    virtual std::uint8_t TransitCopyBlit(GraphicsSurfaceHandle destination,
        GraphicsSurfaceHandle source, std::int32_t x, std::int32_t y,
        const SIZE* size, const RECT* sourceRect, const RECT* clip, std::int32_t anchor) = 0;
    virtual std::uint8_t TransitCopyDraw(GraphicsSurfaceHandle source,
        std::int32_t x, std::int32_t y, const SIZE* size,
        const RECT* sourceRect, const RECT* clip, std::int32_t anchor) = 0;
    virtual std::uint8_t TransitBlendBlit(GraphicsSurfaceHandle destination,
        GraphicsSurfaceHandle source, std::int32_t x, std::int32_t y,
        std::int32_t alpha, const SIZE* size, const RECT* sourceRect,
        const RECT* clip, std::int32_t anchor) = 0;
    virtual std::uint8_t TransitBlendDraw(GraphicsSurfaceHandle source,
        std::int32_t x, std::int32_t y, std::int32_t alpha, const SIZE* size,
        const RECT* sourceRect, const RECT* clip, std::int32_t anchor) = 0;
    virtual std::uint8_t TransitBlit(GraphicsSurfaceHandle destination,
        GraphicsSurfaceHandle source, std::int32_t x, std::int32_t y,
        const SIZE* size, const RECT* sourceRect, const RECT* clip, std::int32_t anchor) = 0;
    virtual std::uint8_t TransitDraw(GraphicsSurfaceHandle source,
        std::int32_t x, std::int32_t y, const SIZE* size,
        const RECT* sourceRect, const RECT* clip, std::int32_t anchor) = 0;
    virtual void TransitUniversalBlit(GraphicsSurfaceHandle destination,
        GraphicsSurfaceHandle mask, const std::int32_t* thresholds,
        std::int32_t x, std::int32_t y, const SIZE* size,
        const RECT* sourceRect, const RECT* clip, std::int32_t anchor) = 0;
    virtual void TransitMosaic(GraphicsSurfaceHandle destination,
        std::uint32_t size, const RECT* clip) = 0;
};

class ICrtRandomSource : public IScrVmProgramRandSource
{
public:
    virtual void Seed(std::uint32_t value) = 0;
};

// 568D8C/568DAD：同线程 CRT 状态，必须与 VM 的 rand 共用，不建转场私有随机流。
class MsvcCrtRandomSource final : public ICrtRandomSource
{
public:
    void Seed(std::uint32_t value) override;
    std::int32_t NextRand() override;
};

class TransitRuntime;

// 449DD0：前3项借用参数包，其余项只有7个栈实参；随机项还读取ECX。
// 这里是普通4字节代码地址，不是可能带调整量的C++成员函数指针。
using TransitPacketFunction = std::uint8_t (__stdcall*)(const TransitOperation*);
using TransitExpandedFunction = std::uint8_t (__stdcall*)(GraphicsSurfaceHandle,
    GraphicsSurfaceNativeBody*, std::int32_t, std::int32_t, const RECT*, std::int32_t, std::int32_t);
using TransitMemberFunction = std::uint8_t (__thiscall*)(const TransitRuntime*,
    GraphicsSurfaceHandle, GraphicsSurfaceNativeBody*, std::int32_t, std::int32_t,
    const RECT*, std::int32_t, std::int32_t);

struct TransitNativeVtable final
{
    TransitRuntime* (__thiscall* deletingDestructorAt00)(TransitRuntime*, std::uint32_t);
};

class TransitRuntime final
{
public:
    explicit TransitRuntime(ICrtRandomSource& random);
    ~TransitRuntime() noexcept;
    [[nodiscard]] std::uint8_t RenderAt449DD0(
        const TransitOperation& operation, IGraphicsTransitApi& graphics) const;
    // 显式现代checked入口，保留范围/空槽拒绝；调用者自行建立Graphics借用scope。
    [[nodiscard]] std::uint8_t RenderNativeAt449DD0(const TransitOperation& operation) const;
    // IDA 449DD7..449DE3：无边界/空函数保护，按x86 DWORD回绕读取实际表槽。
    // 原生caller必须显式借用Graphics；不在raw失败后转checked或改取其它receiver。
    [[nodiscard]] std::uint8_t DispatchRawAt449DD0(const TransitOperation& operation) const;
    [[nodiscard]] const std::array<std::uint8_t, 256>& RandomTable() const noexcept;

    // 448A30的唯一1F8h正文，不含服务、owner或重复随机表；构造前不得清零。
    const TransitNativeVtable* vptrAt00;
    std::array<std::uintptr_t, 61> functionsAt04;
    std::array<std::uint8_t, 256> randomAtF8;
};
static_assert(sizeof(TransitPacketFunction) == 4 && sizeof(TransitExpandedFunction) == 4);
static_assert(sizeof(TransitMemberFunction) == 4 && sizeof(TransitNativeVtable) == 4);
static_assert(std::is_standard_layout_v<TransitRuntime>);
static_assert(sizeof(TransitRuntime) == 0x1F8);
static_assert(offsetof(TransitRuntime, vptrAt00) == 0);
static_assert(offsetof(TransitRuntime, functionsAt04) == 4);
static_assert(offsetof(TransitRuntime, randomAtF8) == 0xF8);
static_assert(sizeof(TransitOperation) == 0x30);
static_assert(offsetof(TransitOperation, source) == 4);
static_assert(offsetof(TransitOperation, opcode) == 0x1C);

// 原Graphics全局取源的显式短期借用，重入及异常都恢复前一服务，不持有所有权。
class TransitGraphicsScope final
{
public:
    explicit TransitGraphicsScope(IGraphicsTransitApi& graphics) noexcept;
    ~TransitGraphicsScope() noexcept;
    TransitGraphicsScope(const TransitGraphicsScope&) = delete;
    TransitGraphicsScope& operator=(const TransitGraphicsScope&) = delete;
private:
    IGraphicsTransitApi* previous_;
};

[[nodiscard]] const TransitNativeVtable& TransitNativeMethods() noexcept;
[[nodiscard]] const std::array<std::uintptr_t, 61>& TransitNativeFunctionTable() noexcept;
}
