#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
// 样本18运行根+1AC..1BF；不是完整21Ch根，也不含现代发布/初始化有效位。
// +1B3确为非对齐WORD，不能让默认成员对齐把它移到+1B4。
#pragma pack(push, 1)
struct alignas(4) RootControlNativeState
{
    std::uint16_t valueAt1AC;
    std::uint8_t unknownAt1AE;
    std::uint8_t byteAt1AF;
    std::uint8_t waitAt1B0;
    std::uint8_t popupAt1B1;
    std::uint8_t minimizeAt1B2;
    std::uint16_t valueAt1B3;
    std::uint8_t unknownAt1B5[3];
    std::uint32_t valueAt1B8;
    std::uint32_t valueAt1BC;

    // 413D18构造计时器后直接清+1C0起的Ref，跳过这20字节。
    // 必须是用户提供的空构造，连RootControlNativeState{}也不能预清正文。
    RootControlNativeState() noexcept {}
    RootControlNativeState(const RootControlNativeState&) = delete;
    RootControlNativeState& operator=(const RootControlNativeState&) = delete;

    // 用字符别名访问WORD的原BYTE，不读取相邻未初始化byte，也不引入union活动成员切换。
    [[nodiscard]] std::uint8_t& ByteAt1AC() noexcept
        { return reinterpret_cast<std::uint8_t*>(&valueAt1AC)[0]; }
    [[nodiscard]] const std::uint8_t& ByteAt1AC() const noexcept
        { return reinterpret_cast<const std::uint8_t*>(&valueAt1AC)[0]; }
    [[nodiscard]] std::uint8_t& ByteAt1AD() noexcept
        { return reinterpret_cast<std::uint8_t*>(&valueAt1AC)[1]; }
    [[nodiscard]] const std::uint8_t& ByteAt1AD() const noexcept
        { return reinterpret_cast<const std::uint8_t*>(&valueAt1AC)[1]; }
    // 48A345写BYTE、4777B6读BYTE；+1B4只在4778ED的WORD清零中一同写入。
    [[nodiscard]] std::uint8_t& ByteAt1B3() noexcept
        { return reinterpret_cast<std::uint8_t*>(&valueAt1B3)[0]; }
    [[nodiscard]] const std::uint8_t& ByteAt1B3() const noexcept
        { return reinterpret_cast<const std::uint8_t*>(&valueAt1B3)[0]; }

    void ResetAt4778B0() noexcept
    {
        // 4778E4/4778ED/4778F6/477900：WORD、WORD、DWORD、DWORD，顺序与宽度均独立。
        valueAt1AC = 0;
        valueAt1B3 = 0;
        valueAt1B8 = 0;
        valueAt1BC = 0;
    }

    void BeginFrameAt483524() noexcept
    {
        // 483524/48352B仅写BYTE；不能用WORD读改写触及+1AD。
        ByteAt1AC() = 1;
        waitAt1B0 = 1;
    }

    void InitializeWindowFlagsAt48ADEA() noexcept
    {
        // 原指令为mov word ptr [edi+1B1h],100h；仅这两个byte，不清+1B0/+1B3。
        // memcpy表示非对齐WORD写入，避免将BYTE成员强转为uint16_t引用。
        constexpr std::uint16_t flags = 0x0100;
        std::memcpy(reinterpret_cast<unsigned char*>(this) + 0x05, &flags, sizeof(flags));
    }
};
#pragma pack(pop)

static_assert(std::endian::native == std::endian::little);
static_assert(std::is_same_v<std::uint8_t, unsigned char>);
static_assert(std::is_standard_layout_v<RootControlNativeState>);
static_assert(alignof(RootControlNativeState) == 4);
static_assert(sizeof(RootControlNativeState) == 0x14);
static_assert(offsetof(RootControlNativeState, valueAt1AC) == 0x00);
static_assert(offsetof(RootControlNativeState, unknownAt1AE) == 0x02);
static_assert(offsetof(RootControlNativeState, byteAt1AF) == 0x03);
static_assert(offsetof(RootControlNativeState, waitAt1B0) == 0x04);
static_assert(offsetof(RootControlNativeState, popupAt1B1) == 0x05);
static_assert(offsetof(RootControlNativeState, minimizeAt1B2) == 0x06);
static_assert(offsetof(RootControlNativeState, valueAt1B3) == 0x07);
static_assert(offsetof(RootControlNativeState, unknownAt1B5) == 0x09);
static_assert(offsetof(RootControlNativeState, valueAt1B8) == 0x0C);
static_assert(offsetof(RootControlNativeState, valueAt1BC) == 0x10);

// 现代调用视图兼容原PreWindowLifecycleState字段语法；基类就是唯一20字节正文。
// 本类型含正文外有效位，不得当作原21Ch根中的20字节字段嵌入。
struct PreWindowLifecycleState final : RootControlNativeState
{
    struct NativeConstructionAt413BE0Tag final {};

    // 生产根用此入口：正文全不写，只初始化现代有效位。
    explicit PreWindowLifecycleState(NativeConstructionAt413BE0Tag) noexcept {}

    // 旧独立预窗口调用/测试保留默认四字段零值及{a,b,c,d[,valid]}语法。
    // 不调用Reset，不把这些现代初值冒充原根构造或4778B0已经执行。
    PreWindowLifecycleState(std::uint16_t at1AC = 0, std::uint16_t at1B3 = 0,
        std::uint32_t at1B8 = 0, std::uint32_t at1BC = 0, bool initialized = false) noexcept
    {
        valueAt1AC = at1AC;
        valueAt1B3 = at1B3;
        valueAt1B8 = at1B8;
        valueAt1BC = at1BC;
        initializedAt4778B0 = initialized;
    }

    // 兼容旧四字段快照/恢复。不能复制新加入的帧byte或尚无初始化来源的保留字节。
    PreWindowLifecycleState(const PreWindowLifecycleState& other) noexcept
        : PreWindowLifecycleState(other.valueAt1AC, other.valueAt1B3,
            other.valueAt1B8, other.valueAt1BC, other.initializedAt4778B0) {}
    PreWindowLifecycleState& operator=(const PreWindowLifecycleState& other) noexcept
    {
        valueAt1AC = other.valueAt1AC;
        valueAt1B3 = other.valueAt1B3;
        valueAt1B8 = other.valueAt1B8;
        valueAt1BC = other.valueAt1BC;
        initializedAt4778B0 = other.initializedAt4778B0;
        return *this;
    }

    [[nodiscard]] RootControlNativeState& Native() noexcept { return *this; }
    [[nodiscard]] const RootControlNativeState& Native() const noexcept { return *this; }
    void Reset() noexcept;

    // 现代有效位始终在20字节正文外；只有Reset或显式旧状态快照会发布它。
    bool initializedAt4778B0 = false;
};
}
