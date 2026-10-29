#include "TransitEffects.h"
#include "TransitEffects.Random.h"
#include "TransitEffects.Wipe.h"

#include <cstdlib>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace TransitEffects
{
namespace { thread_local bool checkedSource = false; }
SourceAccessScope::SourceAccessScope(bool checked) noexcept : previous_(checkedSource)
    { checkedSource = checked; }
SourceAccessScope::~SourceAccessScope() noexcept { checkedSource = previous_; }
bool CheckedSourceAccess() noexcept { return checkedSource; }
}
namespace
{
thread_local IGraphicsTransitApi* graphicsAtCall = nullptr;

IGraphicsTransitApi& Graphics()
{
    // 正文中没有Graphics字段。现代调用必须显式借用，不暗中创建或改取另一模块。
    if (!graphicsAtCall) throw std::logic_error("CTransit requires a TransitGraphicsScope");
    return *graphicsAtCall;
}

template<int Index>
std::uint8_t __stdcall Packet(const TransitOperation* operation)
{
    static_assert(Index >= 0 && Index <= 2);
    if constexpr (Index == 0) return TransitEffects::BlendAt448CB0(*operation, Graphics());
    if constexpr (Index == 1) return TransitEffects::UniversalAt44ED80(*operation, Graphics());
    if constexpr (Index == 2) return TransitEffects::MosaicAt44DBF0(*operation, Graphics());
}

TransitOperation Expanded(GraphicsSurfaceHandle destination, GraphicsSurfaceNativeBody* source,
    std::int32_t x, std::int32_t y, const RECT* clip, std::int32_t current, std::int32_t kind)
{
    TransitOperation result;
    result.destination = destination;
    result.source = source;
    result.x = x;
    result.y = y;
    result.clip = clip;
    result.current = current;
    result.targetKind = kind;
    return result;
}

// 每个模板实例是独立stdcall入口，算法在编译期绑定；运行分派只消费正文函数表。
template<int Index>
std::uint8_t __stdcall Ordinary(GraphicsSurfaceHandle destination, GraphicsSurfaceNativeBody* source,
    std::int32_t x, std::int32_t y, const RECT* clip, std::int32_t current, std::int32_t kind)
{
    static_assert(Index >= 3 && Index <= 60 && (Index < 33 || Index > 37));
    const auto op = Expanded(destination, source, x, y, clip, current, kind);
    auto& graphics = Graphics();
    using namespace TransitEffects;
    if constexpr (Index == 3) return RevealAt44B3C0(op, graphics);
    else if constexpr (Index == 4) return InterlaceAt44B5A0(op, graphics);
    else if constexpr (Index == 5) return InterlaceAt44B8C0(op, graphics);
    else if constexpr (Index == 6 || Index == 7) return TwoPhaseInterlace(op, graphics, Index == 7);
    else if constexpr (Index == 8 || Index == 9) return OpposingLines(op, graphics, Index == 9);
    else if constexpr (Index >= 10 && Index <= 12)
        return TileRevealAt44DDA0(op, graphics, 1 << (Index - 8), current >> (16 - Index));
    else if constexpr (Index >= 13 && Index <= 20)
        return StripedReveal(op, graphics, Index < 17, (Index & 1) == 0,
            ((Index - 13) % 4 < 2) ? 7 : 15, ((Index - 13) % 4 < 2) ? 5 : 4);
    else if constexpr (Index >= 21 && Index <= 28)
        return StaggeredBands(op, graphics, Index >= 25, (Index & 1) != 0,
            ((Index - 21) % 4 < 2) ? 16 : 8);
    else if constexpr (Index >= 29 && Index <= 32)
        return SlidingCopy(op, graphics, Index >= 31, (Index & 1) != 0);
    else if constexpr (Index == 38) return WaveAt44F000(op, graphics);
    else if constexpr (Index == 39) return WaveAt44F190(op, graphics);
    else if constexpr (Index == 40) return WaveAt44F340(op, graphics);
    else if constexpr (Index == 41) return WaveAt44F4D0(op, graphics);
    else if constexpr (Index >= 42 && Index <= 49)
        return OrderedWipeAt44F7F0(op, graphics, (Index & 1) == 0, (Index - 42) / 2);
    else if constexpr (Index >= 50 && Index <= 54) return CircularReveal(op, graphics, Index);
    else if constexpr (Index == 55) return EdgeBandsAt44E1B0(op, graphics);
    else if constexpr (Index == 56) return CenterBandAt44DFD0(op, graphics);
    else return StripedReveal(op, graphics, Index < 59, (Index & 1) == 0, 63, 2);
}

// 44CCD0等：ECX为CTransit，7个实参仍由被调方ret1Ch清栈。
// fastcall的空EDX参数只用于表达thiscall ABI，不占栈，也不使用成员函数指针。
template<int Index>
std::uint8_t __fastcall Random(const TransitRuntime* self, void*,
    GraphicsSurfaceHandle destination, GraphicsSurfaceNativeBody* source,
    std::int32_t x, std::int32_t y, const RECT* clip, std::int32_t current, std::int32_t kind)
{
    static_assert(Index >= 33 && Index <= 37);
    const auto op = Expanded(destination, source, x, y, clip, current, kind);
    auto& graphics = Graphics();
    if constexpr (Index == 33) return TransitEffects::RandomAt44CCD0(op, graphics, self->RandomTable());
    if constexpr (Index == 34) return TransitEffects::RandomAt44D1B0(op, graphics, self->RandomTable());
    if constexpr (Index == 35) return TransitEffects::RandomAt44D690(op, graphics, self->RandomTable());
    if constexpr (Index == 36) return TransitEffects::RandomAt449E10(op, graphics, self->RandomTable());
    if constexpr (Index == 37) return TransitEffects::RandomAt44A230(op, graphics, self->RandomTable());
}

template<std::size_t Index>
std::uintptr_t Entry() noexcept
{
    if constexpr (Index <= 2) return reinterpret_cast<std::uintptr_t>(&Packet<Index>);
    else if constexpr (Index >= 33 && Index <= 37) return reinterpret_cast<std::uintptr_t>(&Random<Index>);
    else return reinterpret_cast<std::uintptr_t>(&Ordinary<Index>);
}

template<std::size_t... Index>
std::array<std::uintptr_t, sizeof...(Index)> MakeTable(std::index_sequence<Index...>) noexcept
{
    return {Entry<Index>()...};
}

// 两个小ABI桥保留ECX以及原压栈顺序。不能按原索引推测替换后的函数是否读取ECX。
// 桥内无析构对象；被调方异常向外展开到RenderAt449DD0的借用scope。
__declspec(noinline) std::uint8_t InvokePacket(const TransitRuntime* self,
    std::uintptr_t function, const TransitOperation* operation)
{
    std::uint8_t result;
    __asm
    {
        mov ecx, self
        mov edx, operation
        mov eax, function
        push edx
        call eax
        mov result, al
    }
    return result;
}

__declspec(noinline) std::uint8_t InvokeExpanded(const TransitRuntime* self,
    std::uintptr_t function, const TransitOperation* operation)
{
    std::uint8_t result;
    __asm
    {
        mov ecx, self
        mov edx, operation
        mov eax, function
        push dword ptr [edx+20h]
        push dword ptr [edx+24h]
        push dword ptr [edx+18h]
        push dword ptr [edx+14h]
        push dword ptr [edx+10h]
        push dword ptr [edx+4]
        push dword ptr [edx]
        call eax
        mov result, al
    }
    return result;
}
}

static_assert(RAND_MAX == 0x7FFF, "CTransit requires the target MSVC CRT rand contract");
void MsvcCrtRandomSource::Seed(const std::uint32_t value) { std::srand(value); }
std::int32_t MsvcCrtRandomSource::NextRand() { return std::rand(); }

const std::array<std::uintptr_t, 61>& TransitNativeFunctionTable() noexcept
{
    static const auto table = MakeTable(std::make_index_sequence<61>{});
    return table;
}

TransitGraphicsScope::TransitGraphicsScope(IGraphicsTransitApi& graphics) noexcept
    : previous_(graphicsAtCall) { graphicsAtCall = &graphics; }
TransitGraphicsScope::~TransitGraphicsScope() noexcept { graphicsAtCall = previous_; }

TransitRuntime::TransitRuntime(ICrtRandomSource& random)
{
    const auto& table = TransitNativeFunctionTable();
    vptrAt00 = &TransitNativeMethods();
    // 448A3C..448C37：原先写0..20，再57..60，最后21..56。random区尚不写。
    volatile std::uintptr_t* const slots = functionsAt04.data();
    for (std::size_t i = 0; i <= 20; ++i) slots[i] = table[i];
    for (std::size_t i = 57; i <= 60; ++i) slots[i] = table[i];
    for (std::size_t i = 21; i <= 56; ++i) slots[i] = table[i];
    // 448C41..448C63：同线程CRT播种并消耗256次，异常保留已写前缀且不回滚随机流。
    random.Seed(0);
    for (auto& value : randomAtF8) value = static_cast<std::uint8_t>(random.NextRand());
}

TransitRuntime::~TransitRuntime() noexcept { vptrAt00 = &TransitNativeMethods(); }

const std::array<std::uint8_t, 256>& TransitRuntime::RandomTable() const noexcept
{
    return randomAtF8;
}

std::uint8_t TransitRuntime::RenderNativeAt449DD0(const TransitOperation& operation) const
{
    TransitEffects::SourceAccessScope checkedSource(true);
    const auto index = static_cast<std::uint32_t>(operation.opcode);
    // 显式现代checked合同；原无界读取可能命中可调用前后缀，另由raw叶保留。
    if (index >= functionsAt04.size()) throw std::out_of_range("CTransit opcode outside original 0..60 table");
    const auto function = functionsAt04[index];
    if (!function) throw std::invalid_argument("CTransit current table entry is null");
    return index <= 2 ? InvokePacket(this, function, &operation) : InvokeExpanded(this, function, &operation);
}

std::uint8_t TransitRuntime::DispatchRawAt449DD0(const TransitOperation& operation) const
{
    TransitEffects::SourceAccessScope rawSource(false);
    const auto index = static_cast<std::uint32_t>(operation.opcode);
    // 449DDD lea [ecx+eax*4] / 449DE0 mov [esi+4]：不是std::array索引。
    // volatile保留分支前的原读点；空Transit/opcode0读+4，负数及大索引按DWORD回绕。
    const auto address = reinterpret_cast<std::uintptr_t>(this) + 4u + index * 4u;
    const auto function = *reinterpret_cast<const volatile std::uintptr_t*>(address);
    return index <= 2 ? InvokePacket(this, function, &operation) : InvokeExpanded(this, function, &operation);
}

std::uint8_t TransitRuntime::RenderAt449DD0(
    const TransitOperation& operation, IGraphicsTransitApi& graphics) const
{
    TransitGraphicsScope scope(graphics);
    return RenderNativeAt449DD0(operation);
}
}
