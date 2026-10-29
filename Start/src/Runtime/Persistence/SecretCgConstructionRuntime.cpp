#include <Runtime/SecretCgConstructionRuntime.h>

#include <bit>
#include <cstdlib>
#include <cstring>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>
T Read(const void* body, std::uint32_t offset)
{
    // 原空正文会AV；现代明确终止，不抛可被45CAC4吞掉的C++异常。
    if (!body) _invalid_parameter_noinfo_noreturn();
    T value;
    std::memcpy(&value, static_cast<const std::byte*>(body) + offset, sizeof(T));
    return value;
}

const YtlReferenceSlotRuntime& ReferenceAt(const void* body, std::uint32_t offset)
{
    if (!body) _invalid_parameter_noinfo_noreturn();
    return *reinterpret_cast<const YtlReferenceSlotRuntime*>(
        static_cast<const std::byte*>(body) + offset);
}

const void* Resolve(const YtlReferenceSlotRuntime& slot, std::uint32_t index = 0) noexcept
{
    if (!slot.baseAt00) return nullptr;
    const auto stride = slot.controlAt08 ?
        static_cast<const EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C : 0u;
    return reinterpret_cast<const void*>(slot.baseAt00 + (slot.biasAt04 + index) * stride);
}

std::int32_t Count(const YtlReferenceControlRuntime* control) noexcept
{
    return control ? std::bit_cast<std::int32_t>(
        static_cast<const EnvironmentObjectReferenceControl*>(control)->countAt10) : 0;
}

std::int32_t ReferenceCount(const void* body, std::uint32_t offset)
{
    const auto begin = Read<std::uintptr_t>(body, offset);
    const auto end = Read<std::uintptr_t>(body, offset + 4);
    return std::bit_cast<std::int32_t>(end - begin) / 12;
}

const void* GameSystem(ISecretCgParamsRootSource& source, std::uint32_t callSite)
{
    return Resolve(ReferenceAt(source.CaptureAt418480(callSite), 4));
}

const void* Pattern(ISecretCgParamsRootSource& source, std::uint32_t callSite)
{
    return Resolve(ReferenceAt(source.CaptureAt418480(callSite), 0x10));
}

const void* Mapping(ISecretCgParamsRootSource& source, std::uint32_t callSite, std::uint32_t index)
{
    const auto* begin = Read<const YtlReferenceSlotRuntime*>(GameSystem(source, callSite), 0x104);
    return Resolve(ReferenceAt(begin, index * 12));
}

class ReferenceUnwind final
{
public:
    explicit ReferenceUnwind(YtlReferenceSlotRuntime& slot) noexcept : slot_(&slot) {}
    ~ReferenceUnwind() noexcept { if (slot_) ReleaseYtlReference(*slot_); }
    void Dismiss() noexcept { slot_ = nullptr; }
private:
    YtlReferenceSlotRuntime* slot_;
};

void ReleaseCached(YtlReferenceSlotRuntime& slot, YtlReferenceControlRuntime* control) noexcept
{
    // 45C187/45C1C5/45C9BB/45C9EF：正常析构区固定control；不能改成重取slot。
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
    slot.controlAt08 = nullptr;
    slot.baseAt00 = 0;
}

void FindFlagList(YtlReferenceSlotRuntime& result, const void* pattern, const char* name)
{
    // 45C010/4786FC：仅进入非空表后检查name；首匹配且不排序、不合并重名项。
    auto* item = Read<const YtlReferenceSlotRuntime*>(pattern, 0x1C);
    while (item != Read<const YtlReferenceSlotRuntime*>(pattern, 0x20))
    {
        const auto* group = Resolve(*item);
        if (!name) throw ATL::CAtlException(E_FAIL);
        const auto* groupName = Read<const unsigned char*>(group, 0);
        if (_mbsicmp(groupName, reinterpret_cast<const unsigned char*>(name)) == 0)
        {
            // strcmp之后原程序重新解析当前Ref，不能沿用此前借用的group正文。
            const auto& selected = ReferenceAt(Resolve(*item), 4);
            if (&result != &selected)
            {
                ReleaseYtlReference(result);
                result = selected;
                if (result.controlAt08) ++result.controlAt08->referenceCountAt04;
            }
            return;
        }
        ++item;
    }
}

std::int32_t FindFlagIndex(const SecretCgReferenceVector& records, std::uint32_t id)
{
    auto* item = records.begin;
    while (item != records.end)
    {
        if (Read<std::uint32_t>(Resolve(*item), 0) == id)
            return std::bit_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(item) -
                reinterpret_cast<std::uintptr_t>(records.begin)) / 12;
        ++item;
    }
    return -1;
}

void BuildMappingItem(SecretCgReferenceVector& records, SecretCgMappingVectorStorage& mappings,
    std::uint32_t mappingIndex, const void* pattern, const char* name, IEnvironmentRootMemory& memory)
{
    YtlReferenceSlotRuntime source;
    ReferenceUnwind sourceUnwind(source);
    FindFlagList(source, pattern, name);
    YtlReferenceSlotRuntime indices;
    ReferenceUnwind indicesUnwind(indices);
    // 45C825先array-new，后control；分配失败时只按当时已发布的Ref字段展开。
    AllocateSecretCgIndicesAt45C825(indices,
        static_cast<std::uint32_t>(Count(source.controlAt08)), memory);
    auto* const indicesControl = indices.controlAt08;
    for (std::int32_t index = 0; index < Count(indicesControl); ++index)
    {
        const auto id = Read<std::uint32_t>(Resolve(source, index), 0);
        const auto resolved = FindFlagIndex(records, id);
        auto* target = const_cast<void*>(Resolve(indices, index));
        if (!target) _invalid_parameter_noinfo_noreturn();
        std::memcpy(target, &resolved, sizeof(resolved));
    }
    // 45C960重读mapping begin；两个临时Ref各有独立清理资格。
    auto& destination = mappings.begin[mappingIndex].referencesAt04;
    auto temporary = indices;
    if (temporary.controlAt08) ++temporary.controlAt08->referenceCountAt04;
    ReferenceUnwind temporaryUnwind(temporary);
    AppendSecretCgReferenceAt4551B0(destination, temporary, memory);
    temporaryUnwind.Dismiss();
    ReleaseCached(temporary, temporary.controlAt08);
    indicesUnwind.Dismiss();
    ReleaseCached(indices, indicesControl);
    sourceUnwind.Dismiss();
    // 45CA20..45CA50：唯独此处payload释放后重新读取source.control。
    // 5E84BC state31 -> 541E55：正常释放异常终止，不能落到外层ready=0 catch。
    const auto releaseSource = [](YtlReferenceSlotRuntime& slot) noexcept { ReleaseYtlReference(slot); };
    releaseSource(source);
}
}

void ConstructSecretCgTablesAt45BCA0(bool& ready, std::uint32_t& baseKey,
    SecretCgReferenceVector& records, SecretCgMappingVectorStorage& mappings,
    ISecretCgParamsRootSource& params, IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory)
{
    records = {};
    mappings = {};
    ready = false;
    baseKey = 0;
    try
    {
        ClearSecretCgReferencesAt45BD08(records, memory);
        for (std::int32_t group = 0;
             group < ReferenceCount(GameSystem(params, 0x45BD17), 0xEC); ++group)
        {
            const auto* begin = Read<const YtlReferenceSlotRuntime*>(
                GameSystem(params, 0x45BE15), 0xEC);
            const auto* name = Read<const char*>(Resolve(ReferenceAt(begin, group * 12)), 0);
            const auto* pattern = Pattern(params, 0x45BF1F);
            YtlReferenceSlotRuntime ids;
            ReferenceUnwind idsUnwind(ids);
            FindFlagList(ids, pattern, name);
            auto* const cachedControl = static_cast<SecretCgFlagControl*>(ids.controlAt08);
            for (std::int32_t index = 0; index < Count(cachedControl); ++index)
            {
                YtlReferenceSlotRuntime record;
                // 45C0AF先分配8字节，之后才取源ID；control资格到45C158才建立。
                CreateRecordReference(record, ids, index, cachedControl, memory);
                ReferenceUnwind recordUnwind(record);
                AppendSecretCgReferenceAt4551B0(records, record, memory);
                recordUnwind.Dismiss();
                ReleaseCached(record, record.controlAt08);
            }
            idsUnwind.Dismiss();
            ReleaseCached(ids, cachedControl);
        }

        const auto count = ReferenceCount(GameSystem(params, 0x45C203), 0x104);
        ResizeSecretCgMappingVectorAt4577F0(mappings, static_cast<std::uint32_t>(count), strings, memory);
        for (std::int32_t index = 0;
             index < ReferenceCount(GameSystem(params, 0x45C259), 0x104); ++index)
        {
            const auto* source = Mapping(params, 0x45C353, index);
            if (!source) _invalid_parameter_noinfo_noreturn();
            // 45C464..45C4B6与410220逐分支同序：同header、同manager共享、否则437680。
            AssignRuntimeCStringAt410220(mappings.begin[index].nameAt00,
                *static_cast<const RuntimeCString*>(source));
            auto& items = mappings.begin[index].referencesAt04;
            ClearSecretCgReferencesAt45BD08(items, memory);

            // 原程序只缓存begin，每个循环条件都重新捕获params并读取当代end。
            auto* item = Read<const YtlReferenceSlotRuntime*>(Mapping(params, 0x45C4DD, index), 4);
            while (item != Read<const YtlReferenceSlotRuntime*>(Mapping(params, 0x45C5E0, index), 8))
            {
                const auto* name = Read<const char*>(Resolve(*item), 0);
                const auto* pattern = Pattern(params, 0x45C716);
                BuildMappingItem(records, mappings, index, pattern, name, memory);
                ++item;
            }
        }

        // 45CA72最后遍历完整flag表，不是只清本次刚刚追加的记录。
        for (auto* item = records.begin; item != records.end; ++item)
        {
            auto* record = ResolveSecretCgFlag(*item);
            if (!record) _invalid_parameter_noinfo_noreturn();
            record->byteFlagAt04 = 0;
        }
        ready = true;
    }
    catch (...)
    {
        // 45CAC4 -> 45CACA：保留同一对象与部分表，正常返回；并非强异常保证。
    }
}
}
