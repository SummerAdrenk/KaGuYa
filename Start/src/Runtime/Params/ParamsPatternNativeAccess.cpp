#include <Runtime/ParamsPatternNativeAccess.h>
#include <Runtime/ParamsRuntimeProfile.h>
#include <Runtime/ParamsLegacyTables.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>

#include <Windows.h>
#include <bit>
#include <limits>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T> const T& Require(const T* body)
{
    if (!body) throw std::logic_error("Pattern reference resolves to null (safe boundary)");
    return *body;
}
void RequireKind(const ParamsPatternNative& pattern, std::size_t index, std::uint32_t expected)
{
    if (ParamsPatternItemKindAt(pattern, index) != expected)
        throw std::logic_error("Pattern typed accessor kind mismatch");
}
std::uint32_t Count32(std::size_t size)
{
    if (size > static_cast<std::size_t>((std::numeric_limits<std::int32_t>::max)()))
        throw std::length_error("Pattern snapshot count exceeds signed DWORD");
    return static_cast<std::uint32_t>(size);
}
void CopyText(RuntimeCString& destination, std::string_view text)
{ SetRuntimeCStringAt437680(destination, text.data(), static_cast<int>(Count32(text.size()))); }
ParamsText SnapshotText(const RuntimeCString& source)
{
    ParamsText result;
    result.cp932.assign(RuntimeStringView(source));
    if (result.cp932.empty()) return result;
    const auto count = static_cast<int>(Count32(result.cp932.size()));
    const int length = MultiByteToWideChar(CP_THREAD_ACP, 0, result.cp932.data(), count, nullptr, 0);
    if (length <= 0) throw std::runtime_error("Pattern snapshot text conversion failed");
    result.utf16.resize(static_cast<std::size_t>(length));
    if (MultiByteToWideChar(CP_THREAD_ACP, 0, result.cp932.data(), count,
        reinterpret_cast<wchar_t*>(result.utf16.data()), length) != length)
        throw std::runtime_error("Pattern snapshot text conversion changed length");
    // utf16是当前字节的显式导出结果，不是已丢弃的原wire Unicode镜像。
    return result;
}
struct OwnedSlot final
{
    YtlReferenceSlotRuntime slot{};
    ~OwnedSlot() noexcept { ReleaseYtlReference(slot); }
};
void ImportIntegers(YtlReferenceSlotRuntime& slot, const std::vector<std::uint32_t>& values)
{
    const auto count = Count32(values.size());
    static_cast<void>(RebuildParamsPatternGroupIndices(slot, count));
    for (std::uint32_t index = 0; index < count; ++index)
        *ResolveParamsLegacyInteger(slot, index) = std::bit_cast<std::int32_t>(values[index]);
}
std::vector<std::uint32_t> ExportIntegers(const YtlReferenceSlotRuntime& slot)
{
    std::vector<std::uint32_t> result;
    const auto count = ParamsLegacyTableCount(slot);
    result.reserve(count);
    for (std::uint32_t index = 0; index < count; ++index)
        result.push_back(std::bit_cast<std::uint32_t>(Require(ResolveParamsLegacyInteger(slot, index))));
    return result;
}
}

std::size_t ParamsPatternVectorSize(const ParamsPatternReferenceVector& table) noexcept
{
    return (reinterpret_cast<std::uintptr_t>(table.end) -
        reinterpret_cast<std::uintptr_t>(table.begin)) / sizeof(YtlReferenceSlotRuntime);
}
const YtlReferenceSlotRuntime& ParamsPatternReferenceAt(const ParamsPatternReferenceVector& table, std::size_t index)
{
    if (index >= ParamsPatternVectorSize(table)) throw std::out_of_range("Pattern Ref vector index");
    return table.begin[index];
}
std::uint32_t ParamsPatternItemKindAt(const ParamsPatternNative& pattern, std::size_t index)
{ return Require(ResolveParamsPatternFileName(ParamsPatternReferenceAt(pattern.itemsAt04, index))).kindAt00; }
const RuntimeCString& ParamsPatternItemNameAt(const ParamsPatternNative& pattern, std::size_t index)
{ return Require(ResolveParamsPatternFileName(ParamsPatternReferenceAt(pattern.itemsAt04, index))).nameAt04; }
const ParamsPatternFileGroupNative& ParamsPatternFileGroupAt(const ParamsPatternNative& pattern, std::size_t index)
{
    RequireKind(pattern, index, 1);
    return Require(ResolveParamsPatternFileGroup(ParamsPatternReferenceAt(pattern.itemsAt04, index)));
}
const ParamsPatternExcPositionNative& ParamsPatternExcPositionAt(const ParamsPatternNative& pattern, std::size_t index)
{
    RequireKind(pattern, index, 2);
    return Require(ResolveParamsPatternExcPosition(ParamsPatternReferenceAt(pattern.itemsAt04, index)));
}
const ParamsPatternFileConvertNative& ParamsPatternFileConvertAt(const ParamsPatternNative& pattern, std::size_t index)
{
    RequireKind(pattern, index, 3);
    return Require(ResolveParamsPatternFileConvert(ParamsPatternReferenceAt(pattern.itemsAt04, index)));
}
const ParamsPatternGroupNative& ParamsPatternGroupAt(const ParamsPatternReferenceVector& table, std::size_t index)
{ return Require(ResolveParamsPatternGroup(ParamsPatternReferenceAt(table, index))); }

std::size_t ParamsPatternVectorSize(const ParamsPatternReferenceVector& table,
    PatternResourceAccess access) noexcept
{
    if (access == PatternResourceAccess::Checked) return ParamsPatternVectorSize(table);
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(table.end) -
        reinterpret_cast<std::uintptr_t>(table.begin));
    // 4AD956/47A035：signed除12，再保留DWORD位型；比较的signedness由原调用点决定。
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
}
const YtlReferenceSlotRuntime& ParamsPatternReferenceAt(const ParamsPatternReferenceVector& table,
    std::size_t index, PatternResourceAccess access)
{
    if (access == PatternResourceAccess::Checked) return ParamsPatternReferenceAt(table, index);
    return *reinterpret_cast<const YtlReferenceSlotRuntime*>(
        reinterpret_cast<std::uintptr_t>(table.begin) + static_cast<std::uint32_t>(index) * 12u);
}
std::uint32_t ParamsPatternItemKindAt(const ParamsPatternNative& pattern, std::size_t index,
    PatternResourceAccess access)
{
    if (access == PatternResourceAccess::Checked) return ParamsPatternItemKindAt(pattern, index);
    return ResolveParamsPatternFileName(ParamsPatternReferenceAt(pattern.itemsAt04, index, access))->kindAt00;
}
const RuntimeCString& ParamsPatternItemNameAt(const ParamsPatternNative& pattern, std::size_t index,
    PatternResourceAccess access)
{
    if (access == PatternResourceAccess::Checked) return ParamsPatternItemNameAt(pattern, index);
    return ResolveParamsPatternFileName(ParamsPatternReferenceAt(pattern.itemsAt04, index, access))->nameAt04;
}
const ParamsPatternFileGroupNative& ParamsPatternFileGroupAt(const ParamsPatternNative& pattern,
    std::size_t index, PatternResourceAccess access)
{
    if (access == PatternResourceAccess::Checked) return ParamsPatternFileGroupAt(pattern, index);
    return *ResolveParamsPatternFileGroup(ParamsPatternReferenceAt(pattern.itemsAt04, index, access));
}
const ParamsPatternExcPositionNative& ParamsPatternExcPositionAt(const ParamsPatternNative& pattern,
    std::size_t index, PatternResourceAccess access)
{
    if (access == PatternResourceAccess::Checked) return ParamsPatternExcPositionAt(pattern, index);
    return *ResolveParamsPatternExcPosition(ParamsPatternReferenceAt(pattern.itemsAt04, index, access));
}
const ParamsPatternFileConvertNative& ParamsPatternFileConvertAt(const ParamsPatternNative& pattern,
    std::size_t index, PatternResourceAccess access)
{
    if (access == PatternResourceAccess::Checked) return ParamsPatternFileConvertAt(pattern, index);
    return *ResolveParamsPatternFileConvert(ParamsPatternReferenceAt(pattern.itemsAt04, index, access));
}

ParamsPatternReference ImportParamsPatternSnapshot(const ParamsPattern& snapshot)
{
    auto result = ParamsPatternReference::CreateFixture();
    for (const auto& item : snapshot.items)
    {
        OwnedSlot owner;
        switch (item.kind)
        {
        case 0:
            CopyText(CreateParamsPatternFileNameAt41E904(owner.slot)->nameAt04, item.name.cp932);
            break;
        case 1:
        {
            auto* body = CreateParamsPatternFileGroupAt41E972(owner.slot);
            CopyText(body->nameAt04, item.name.cp932);
            body->stringsAt08.RebuildAt412870(static_cast<std::int32_t>(Count32(item.strings.size())));
            for (std::uint32_t index = 0; index < item.strings.size(); ++index)
                CopyText(body->stringsAt08.At(index), item.strings[index].cp932);
            break;
        }
        case 2:
        {
            auto* body = CreateParamsPatternExcPositionAt41EB42(owner.slot);
            CopyText(body->nameAt04, item.name.cp932);
            CopyText(body->subNameAt08, item.subName.cp932);
            body->xAt0C = item.value1;
            body->yAt10 = item.value2;
            break;
        }
        case 3:
        {
            auto* body = CreateParamsPatternFileConvertAt41ECE8(owner.slot);
            CopyText(body->nameAt04, item.name.cp932);
            CopyText(body->subNameAt08, item.subName.cp932);
            body->flagsAt0C = item.value1;
            break;
        }
        default:
            throw std::invalid_argument("Pattern snapshot has an unsupported kind");
        }
        AppendEnvironmentArchiveReferenceAt49852D(result->itemsAt04, owner.slot);
    }
    for (const auto& values : snapshot.integerArrays)
    {
        OwnedSlot owner;
        ImportIntegers(owner.slot, values);
        AppendEnvironmentArchiveReferenceAt49852D(result->integersAt10, owner.slot);
    }
    for (std::size_t tableIndex = 0; tableIndex != 2; ++tableIndex)
    {
        auto& table = tableIndex ? result->secondGroupsAt28 : result->firstGroupsAt1C;
        for (const auto& group : snapshot.groupTables[tableIndex])
        {
            OwnedSlot owner;
            auto* body = RebuildParamsPatternGroupAt412490(owner.slot);
            CopyText(body->nameAt00, group.name.cp932);
            ImportIntegers(body->indicesAt04, group.indices);
            AppendEnvironmentArchiveReferenceAt49852D(table, owner.slot);
        }
    }
    return result;
}

ParamsPattern ExportParamsPatternSnapshot(const ParamsPatternNative& pattern)
{
    ParamsPattern result;
    for (std::size_t index = 0; index < ParamsPatternVectorSize(pattern.itemsAt04); ++index)
    {
        ParamsPatternItem item;
        const auto kind = ParamsPatternItemKindAt(pattern, index);
        if (kind > 3) throw std::logic_error("Pattern snapshot has an invalid native kind");
        item.kind = static_cast<std::uint8_t>(kind);
        item.name = SnapshotText(ParamsPatternItemNameAt(pattern, index));
        switch (kind)
        {
        case 1:
        {
            const auto& strings = ParamsPatternFileGroupAt(pattern, index).stringsAt08;
            std::vector<ParamsText> values;
            values.reserve(strings.size());
            for (std::uint32_t n = 0; n < strings.size(); ++n) values.push_back(SnapshotText(strings.At(n)));
            item.strings.AssignSnapshot(values);
            break;
        }
        case 2:
        {
            const auto& body = ParamsPatternExcPositionAt(pattern, index);
            item.subName = SnapshotText(body.subNameAt08);
            item.value1 = body.xAt0C;
            item.value2 = body.yAt10;
            break;
        }
        case 3:
        {
            const auto& body = ParamsPatternFileConvertAt(pattern, index);
            item.subName = SnapshotText(body.subNameAt08);
            item.value1 = body.flagsAt0C;
            break;
        }
        }
        result.items.push_back(std::move(item));
    }
    for (auto* slot = pattern.integersAt10.begin; slot != pattern.integersAt10.end; ++slot)
        result.integerArrays.push_back(ExportIntegers(*slot));
    for (std::size_t tableIndex = 0; tableIndex != 2; ++tableIndex)
    {
        const auto& table = tableIndex ? pattern.secondGroupsAt28 : pattern.firstGroupsAt1C;
        for (std::size_t index = 0; index < ParamsPatternVectorSize(table); ++index)
        {
            const auto& group = ParamsPatternGroupAt(table, index);
            result.groupTables[tableIndex].push_back({SnapshotText(group.nameAt00), ExportIntegers(group.indicesAt04)});
        }
    }
    return result;
}
}
