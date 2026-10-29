#include <Runtime/AnimationFormatNative.h>
#include <Runtime/ScriptResultLayerNative.h>

#include <bit>
#include <cstdlib>
#include <cstring>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = AnimationFormatNativeBody44h;
using Arguments = EnvironmentVectorStorage<std::int32_t>;

template<class T> T Read(const std::uint8_t* cursor) noexcept
{
    T value;
    std::memcpy(&value, cursor, sizeof(value));
    return value;
}

template<class T> T ReadAndAdvance(const std::uint8_t*& cursor) noexcept
{
    const auto value = Read<T>(cursor);
    cursor += sizeof(T);
    return value;
}

template<class T> T* ResolveAt(const YtlReferenceSlotRuntime& slot, std::uint32_t index) noexcept
{
    if (!slot.baseAt00) return nullptr;
    const auto stride = slot.controlAt08 ?
        static_cast<const EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C : 0u;
    return reinterpret_cast<T*>(slot.baseAt00 + (slot.biasAt04 + index) * stride);
}

std::uint32_t Address(const void* value) noexcept
{ return reinterpret_cast<std::uintptr_t>(value); }

std::int32_t* IntAddress(std::uint32_t base, std::uint32_t bytes) noexcept
{ return reinterpret_cast<std::int32_t*>(base + bytes); }

std::uint32_t SignedElementDistance(const void* last, const void* first) noexcept
{
    // 4447C7/4447CA/4447E3使用SAR；先保留x86指针差的32-bit回绕。
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(Address(last) - Address(first)) >> 2);
}

void Append(Arguments& values, std::int32_t value)
{
    auto* const position = values.end;
    if (position == values.capacity)
        static_cast<void>(InsertAnimationCommandArgumentAt4447B0(values, position, &value));
    else
    {
        *position = value;
        values.end = IntAddress(Address(values.end), 4);
    }
}
template<class T> T* ElementAddress(std::uint32_t base, std::uint32_t bytes) noexcept
{ return reinterpret_cast<T*>(base + bytes); }

// 两种原DWORD vector只共享算法，不互转header；value可能来自未对齐SCR字节流。
template<class T> T* InsertArgumentAt4447B0(EnvironmentVectorStorage<T>& values,
    T* position, const T* value)
{
    static_assert(sizeof(T) == 4);
    const auto offset = SignedElementDistance(position, values.begin);
    const auto size = SignedElementDistance(values.end, values.begin);
    if (size == 0x3FFFFFFFu) throw std::length_error("vector<T> too long");
    const auto nextSize = size + 1u;
    const auto capacity = SignedElementDistance(values.capacity, values.begin);
    const auto half = capacity >> 1;
    auto nextCapacity = nextSize;
    if (capacity <= 0x3FFFFFFFu - half)
    {
        const auto growth = capacity + half;
        if (growth >= nextSize) nextCapacity = growth;
    }
    const auto capacityBytes = nextCapacity * 4u;
    const auto requestedBytes = nextCapacity <= 0x3FFFFFFFu ? capacityBytes : 0xFFFFFFFFu;
    T* created;
    if (requestedBytes >= 0x1000u)
    {
        const auto expanded = requestedBytes + 0x23u;
        void* const raw = ::operator new(expanded <= requestedBytes ? 0xFFFFFFFFu : expanded);
        if (!raw) _invalid_parameter_noinfo_noreturn();
        created = reinterpret_cast<T*>((Address(raw) + 0x23u) & ~0x1Fu);
        reinterpret_cast<void**>(created)[-1] = raw;
    }
    else created = requestedBytes ? static_cast<T*>(::operator new(requestedBytes)) : nullptr;

    // 444859..44486B：value可别名旧vector，且必须在allocator回调之后读取。
    // 此函数原无EH回滚；成功分配后的旧释放失败不能偷偷回收created。
    const auto offsetBytes = offset * 4u;
    *ElementAddress<T>(Address(created), offsetBytes) = Read<T>(reinterpret_cast<const std::uint8_t*>(value));
    if (position == values.end)
        std::memmove(created, values.begin, Address(values.end) - Address(values.begin));
    else
    {
        std::memmove(created, values.begin, Address(position) - Address(values.begin));
        std::memmove(ElementAddress<T>(Address(created), offsetBytes + 4u), position,
            Address(values.end) - Address(position));
    }

    // 4448AD重新读取当前begin/capacity，不能缓存成分配前的旧缓冲。
    if (auto* raw = values.begin)
    {
        auto bytes = (Address(values.capacity) - Address(raw)) & ~3u;
        if (bytes >= 0x1000u)
        {
            auto* const allocation = reinterpret_cast<T**>(raw)[-1];
            bytes += 0x23u;
            if (Address(raw) - Address(allocation) - 4u > 0x1Fu) _invalid_parameter_noinfo_noreturn();
            raw = allocation;
        }
        ::operator delete(raw, bytes);
    }
    values.begin = created;
    values.end = ElementAddress<T>(Address(created), nextSize * 4u);
    values.capacity = ElementAddress<T>(Address(created), capacityBytes);
    return ElementAddress<T>(Address(values.begin), offsetBytes);
}
}

std::int32_t* InsertAnimationCommandArgumentAt4447B0(Arguments& values,
    std::int32_t* position, const std::int32_t* value)
{ return InsertArgumentAt4447B0(values, position, value); }

std::uint32_t* InsertScriptResultLayerExtraAt4447B0(EnvironmentVectorStorage<std::uint32_t>& values,
    std::uint32_t* position, const std::uint32_t* value)
{
    return InsertArgumentAt4447B0(values, position, value);
}

void SetAnimationCommandPairAt448670(SpriteCommandNativeBody10h& command,
    std::int32_t first, std::int32_t second)
{
    command.opcodeAt00 = 1;
    // 448674固定同一vector receiver；第一追加重入不能改为重新解析command Ref。
    auto& arguments = command.argumentsAt04;
    Append(arguments, first);
    Append(arguments, second);
}

void LoadLegacyAnimationCommandsNativeAt445B90(Body& body, const std::uint8_t*& cursor,
    std::uint8_t readChannels, IAnimationFormatNativePictures& pictures)
{
    body.xAt08 = Read<std::int32_t>(cursor);
    cursor += 4;
    body.yAt0C = Read<std::int32_t>(cursor);
    cursor += 4;
    body.widthAt10 = Read<std::int32_t>(cursor);
    cursor += 4;
    body.heightAt14 = Read<std::int32_t>(cursor);
    cursor += 4;
    const auto count = ReadAndAdvance<std::uint16_t>(cursor);
    if (!count) throw 1;
    ReplaceSpriteCommandArrayAt444EB0(body.commandsAt20, static_cast<std::int32_t>(count) * 2);
    if (*cursor) body.repeatAt18 = 1;
    cursor += 2;
    for (std::uint32_t index = 0; index < count; ++index)
    {
        const auto wait = ReadAndAdvance<std::uint16_t>(cursor);
        const auto frame = ReadAndAdvance<std::uint16_t>(cursor);
        auto* const draw = ResolveAt<SpriteCommandNativeBody10h>(body.commandsAt20, index * 2u);
        SetAnimationCommandPairAt448670(*draw, 0, frame == 0xFFFFu ? -1 : static_cast<std::int32_t>(frame));
        // 445CC3在两次可能分配的追加之后重新解析下一元素。
        auto* const delay = ResolveAt<SpriteCommandNativeBody10h>(body.commandsAt20, index * 2u + 1u);
        delay->opcodeAt00 = 2;
        Append(delay->argumentsAt04, wait);
    }
    body.mappingCountAt1C = 1;
    pictures.LoadLegacyFramesAt445D2F(body, cursor, readChannels);
}

std::uint8_t LoadNewAnimationCommandsNativeAt4460D0(Body& body, const std::uint8_t*& cursor,
    float version, IAnimationFormatNativePictures& pictures)
{
    const auto count = ReadAndAdvance<std::uint16_t>(cursor);
    if (!count) throw 1;
    ReplaceSpriteCommandArrayAt444EB0(body.commandsAt20, count);
    if (*cursor) body.repeatAt18 = 1;
    cursor += 2;
    body.mappingCountAt1C = -1;
    for (std::uint32_t index = 0; index < count; ++index)
    {
        const auto opcode = ReadAndAdvance<std::uint8_t>(cursor);
        if (opcode > 5u) throw 5;
        if (opcode == 0) continue;
        const auto first = ReadAndAdvance<std::int32_t>(cursor);
        if (opcode == 1)
        {
            const auto second = ReadAndAdvance<std::int32_t>(cursor);
            auto* const command = ResolveAt<SpriteCommandNativeBody10h>(body.commandsAt20, index);
            SetAnimationCommandPairAt448670(*command, first, second);
            if (body.mappingCountAt1C < first) body.mappingCountAt1C = first;
        }
        else
        {
            auto* const command = ResolveAt<SpriteCommandNativeBody10h>(body.commandsAt20, index);
            command->opcodeAt00 = opcode;
            Append(command->argumentsAt04, first);
        }
    }
    if (body.mappingCountAt1C < 0) throw 5;
    body.mappingCountAt1C = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(body.mappingCountAt1C) + 1u);

    const auto branchCount = ReadAndAdvance<std::uint16_t>(cursor);
    if (!branchCount) ReleaseYtlReference(body.branchesAt2C);
    else
    {
        ReplaceSpriteBranchArrayAt446327(body.branchesAt2C, branchCount);
        for (std::uint32_t index = 0; index < branchCount; ++index)
        {
            auto* branch = ResolveAt<SpriteBranchNativeBody8h>(body.branchesAt2C, index);
            branch->firstAt00 = Read<std::int32_t>(cursor);
            cursor += 4;
            branch = ResolveAt<SpriteBranchNativeBody8h>(body.branchesAt2C, index);
            branch->secondAt04 = Read<std::int32_t>(cursor);
            cursor += 4;
        }
    }

    float pictureVersion = 0.0f;
    if (version >= 2.1f)
    {
        // 446497逐字节短路；标记/版本失败分别保留未消费的5/2字节。
        if (cursor[0] != '[' || cursor[1] != 'P' || cursor[2] != 'I' || cursor[3] != 'C' || cursor[4] != ']')
            throw 3;
        cursor += 5;
        switch (Read<std::uint16_t>(cursor))
        {
        case 0x3030u: pictureVersion = 0.0f; break;
        case 0x3130u: pictureVersion = 0.1f; break;
        case 0x3031u: pictureVersion = 1.0f; break;
        case 0x3131u: pictureVersion = 1.1f; break;
        case 0x3032u: pictureVersion = 2.0f; break;
        case 0x3033u: pictureVersion = 3.0f; break;
        default: throw 6;
        }
        cursor += 2;
    }
    return LoadAnimationPictureNativeAt447F60(body, cursor, pictureVersion, pictures);
}

std::uint8_t LoadAnimationPictureNativeAt447F60(Body& body, const std::uint8_t*& cursor,
    float version, IAnimationFormatNativePictures& pictures)
{
    if (version == 0.0f) return pictures.LoadPicture00At446BD0(body, cursor);
    if (version == 0.1f) return pictures.LoadPicture01At4465C0(body, cursor);
    if (version == 1.0f) return pictures.LoadPicture10Or11At446FB0(body, cursor, 0);
    if (version == 1.1f) return pictures.LoadPicture10Or11At446FB0(body, cursor, 1);
    if (version == 2.0f) return pictures.LoadPicture20At447500(body, cursor);
    if (version == 3.0f) return pictures.LoadPicture30At4479D0(body, cursor);
    // 448000的真实未知float分支，不代表任何已识别PIC的占位成功。
    return 0;
}
}
