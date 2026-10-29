#include <Runtime/PatternResourcePlan.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/ParamsPatternNativeAccess.h>

#include <mbstring.h>

#include <bit>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
[[nodiscard]] const char* ReadBorrowedPrimaryCharacters(const void* characters)
{
    return static_cast<const char*>(characters);
}

[[nodiscard]] const char* ReadNativeNameCharacters(const void* begin, const std::uint32_t index)
{
    // 4CBD02/4CBEB6：按DWORD回绕寻址，取本轮begin上的当前CString字符槽。
    const auto address = reinterpret_cast<std::uintptr_t>(begin) + index * 4u;
    return reinterpret_cast<const RuntimeCString*>(address)->GetString();
}

[[nodiscard]] PatternResourceNamesRange ReadNativeNameRange(const void* header)
{
    const auto& names = *static_cast<const EnvironmentVectorStorage<RuntimeCString>*>(header);
    const auto* const begin = names.begin;
    const auto bytes = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(names.end) - reinterpret_cast<std::uintptr_t>(begin));
    // 4CBCB1/4CBD20/4CBE7E/4A88A5：SUB后SAR 2，保留结果的全部DWORD位。
    const auto count = std::bit_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) >> 2);
    return {begin, count, &ReadNativeNameCharacters};
}

[[nodiscard]] const char* ReadModernNameCharacters(const void* begin, const std::uint32_t index)
{
    return static_cast<const std::string*>(begin)[index].c_str();
}

[[nodiscard]] PatternResourceNamesRange ReadModernNameRange(const void* header)
{
    const auto& names = *static_cast<const std::vector<std::string>*>(header);
    return {names.data(), static_cast<std::uint32_t>(names.size()), &ReadModernNameCharacters};
}

[[nodiscard]] bool EqualsIgnoreCase(
    const char* first,
    const char* second) noexcept
{
    return _mbsicmp(
               reinterpret_cast<const unsigned char*>(first),
               reinterpret_cast<const unsigned char*>(second)) == 0;
}

void ScanSuffixAt45DA40(RuntimeCString& suffix, const char* source,
    IRuntimeStringManagerSource& strings)
{
    // caller先按40C760完成nil与空Set；45DA70之后才测量原借用源。
    const auto length = source ? static_cast<std::uint32_t>(std::strlen(source)) : 0u;
    SetRuntimeCStringAt437680(suffix, source, std::bit_cast<std::int32_t>(length));
    const auto* dot = _mbsrchr(reinterpret_cast<const unsigned char*>(suffix.GetString()), '.');
    if (dot)
    {
        const int offset = static_cast<int>(dot -
            reinterpret_cast<const unsigned char*>(suffix.GetString()));
        if (offset >= 0 && offset + 1 < suffix.GetLength())
        {
            auto tail = ArchiveResourceNameMidAt420C80(
                suffix, offset + 1, suffix.GetLength() - offset - 1, strings);
            AssignRuntimeCStringAt410220(suffix, tail.Value());
            // 45DAE6读取Assign后的当前临时，45DAEC先撤state1；失败只清scanner。
            tail.ReleaseNormal();
            return;
        }
    }
    SetRuntimeCStringAt437680(suffix, "", 0);
}

[[nodiscard]] FileGroupChange AddChange(
    const FileGroupChange first,
    const FileGroupChange second) noexcept
{
    return static_cast<FileGroupChange>(
        static_cast<std::uint8_t>(first) | static_cast<std::uint8_t>(second));
}

bool Before(std::size_t index, std::size_t count, PatternResourceAccess access) noexcept
{
    if (access == PatternResourceAccess::Checked) return index < count;
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(index)) <
        std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(count));
}
}

PatternResourceNamesBorrow::PatternResourceNamesBorrow(
    const EnvironmentVectorStorage<RuntimeCString>& names) noexcept
    : header_(&names), readRange_(&ReadNativeNameRange)
{
}

void ScanPatternResourceSuffixAt45DA40(RuntimeCString& suffix,const char* source,
    IRuntimeStringManagerSource& strings)
{ScanSuffixAt45DA40(suffix,source,strings);}

PatternResourceNamesBorrow::PatternResourceNamesBorrow(const std::vector<std::string>& names) noexcept
    : header_(&names), readRange_(&ReadModernNameRange)
{
}

bool HasCgDisplayResourceSuffixAt4A8680(const char* source,
    IRuntimeStringManagerSource& strings)
{
    // 仅复原scanner的CString有效载荷；40C760允许构造失败外传，
    // 40F980/40EE90的展开清理由raw owner的noexcept析构承担。
    ArchiveRawResourceNameRuntime scanner("", strings);
    ScanSuffixAt45DA40(scanner.Value(), source, strings);
    union Result
    {
        Result() noexcept {}
        ~Result() noexcept {}
        RuntimeCString value;
    } result;
    ConstructArchiveResourceNameAt40B530(&result.value, scanner.Value().GetString(), strings);
    // 4A869A/4CB941已撤scanner资格，结果串没有外层EH资格。
    // 此Free失败时原结果串不回滚，不能给result加自动析构guard。
    scanner.ReleaseNormal();
    const bool hasSuffix = result.value.GetLength() != 0;
    DestroyRuntimeCStringAt40CD40(&result.value);
    return hasSuffix;
}

bool HasPatternResourceSuffixAt4CB905(const RuntimeCString& source,
    IRuntimeStringManagerSource& strings)
{
    ArchiveRawResourceNameRuntime input(ArchiveRawResourceNameRuntime::ProducerTag{},
        [&](RuntimeCString* out)
        {
            auto* const header = reinterpret_cast<ATL::CStringData*>(
                reinterpret_cast<std::uintptr_t>(source.GetString()) - sizeof(ATL::CStringData));
            auto* const cloned = CloneRuntimeCStringDataAt414540(header);
            const char* const characters = static_cast<const char*>(cloned->data());
            std::memcpy(out, &characters, sizeof(characters));
        });
    const char* const cachedCharacters = input.Value().GetString();
    const bool hasSuffix = HasCgDisplayResourceSuffixAt4A8680(cachedCharacters, strings);
    // 4CB98B先撤stateD，再释放缓存ESI；此前任意失败展开仍清当前输入槽。
    input.ReleaseNormalCached(cachedCharacters);
    return hasSuffix;
}

std::optional<std::size_t> FindPatternItemByResourceName(
    const ParamsPatternNative& pattern,
    const char* resourceNameCp932)
{ return FindPatternItemByResourceName(pattern, resourceNameCp932, PatternResourceAccess::Checked); }

std::optional<std::size_t> FindPatternItemByResourceName(
    const ParamsPatternNative& pattern, const char* resourceNameCp932, PatternResourceAccess access)
{
    for (std::size_t index = 0; Before(index, ParamsPatternVectorSize(pattern.itemsAt04, access), access); ++index)
    {
        const auto* body = ResolveParamsPatternFileName(ParamsPatternReferenceAt(pattern.itemsAt04, index, access));
        // 47A077先检查搜索名；空表不检查。只有Checked将空正文解引用改为明确拒绝。
        if (!resourceNameCp932) throw ATL::CAtlException(E_FAIL);
        if (access == PatternResourceAccess::Checked && !body) throw std::logic_error("Pattern item body is null");
        if (EqualsIgnoreCase(body->nameAt04.GetString(), resourceNameCp932)) return index;
    }
    return std::nullopt;
}

namespace
{
template<PatternResourceAccess Access>
PatternResourcePlan BuildPlan(
    const ParamsPatternNative& pattern,
    const std::uint32_t patternItemIndex,
    IRuntimeStringManagerSource& strings)
{
    PatternResourcePlan plan;
    if (!Before(patternItemIndex, ParamsPatternVectorSize(pattern.integersAt10, Access), Access)) return plan;
    const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(
        ParamsPatternReferenceAt(pattern.integersAt10, patternItemIndex, Access).controlAt08);
    const auto count = control ? control->countAt10 : 0u;
    plan.RebuildAt412870(std::bit_cast<std::int32_t>(count), strings);
    for (std::uint32_t slotIndex = 0;
         std::bit_cast<std::int32_t>(slotIndex) < std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(plan.Size()));
         ++slotIndex)
    {
        // 4AD9B1 每轮重读原外表与内 Ref；412870/SetString 回调不能冻结输入数组。
        auto selected = ParamsPatternReferenceAt(pattern.integersAt10, patternItemIndex, Access);
        selected.biasAt04 += slotIndex;
        const auto* itemIndex = ResolveEnvironmentObjectReference<const std::int32_t>(selected);
        if constexpr (Access == PatternResourceAccess::Checked)
            if (!itemIndex) throw std::logic_error("Pattern integer array body is null");
        const char* name = nullptr;
        if (*itemIndex >= 0 && Before(static_cast<std::size_t>(*itemIndex), ParamsPatternVectorSize(pattern.itemsAt04, Access), Access))
            name = ParamsPatternItemNameAt(pattern, static_cast<std::size_t>(*itemIndex), Access).GetString();
        auto& destination = const_cast<RuntimeCString&>(PatternResourceNameAt<Access>(plan, slotIndex));
        // 4ADA7D 使用首 NUL 长度，不是 Clone，也不复制 DTO 的 UTF-16 字段。
        const auto length = name ? static_cast<std::uint32_t>(std::strlen(name)) : 0u;
        SetRuntimeCStringAt437680(destination, name, std::bit_cast<std::int32_t>(length));
    }
    return plan;
}
}

PatternResourcePlan BuildPatternResourcePlan(const ParamsPatternNative& pattern,
    std::uint32_t index, IRuntimeStringManagerSource& strings)
{ return BuildPlan<PatternResourceAccess::Checked>(pattern, index, strings); }

PatternResourcePlan BuildPatternResourcePlanNativeAt4AD900(const ParamsPatternNative& pattern,
    std::uint32_t index, IRuntimeStringManagerSource& strings)
{ return BuildPlan<PatternResourceAccess::Native>(pattern, index, strings); }

RuntimeCStringArrayReference FindPatternFileGroupByResourceName(
    const ParamsPatternNative& pattern, const char* resourceNameCp932)
{ return FindPatternFileGroupByResourceName(pattern, resourceNameCp932, PatternResourceAccess::Checked); }

RuntimeCStringArrayReference FindPatternFileGroupByResourceName(
    const ParamsPatternNative& pattern, const char* resourceNameCp932, PatternResourceAccess access)
{
    RuntimeCStringArrayReference result;
    const auto index = FindPatternItemByResourceName(pattern, resourceNameCp932, access);
    if (index && ParamsPatternItemKindAt(pattern, *index, access) == 1u)
        result = ParamsPatternFileGroupAt(pattern, *index, access).stringsAt08;
    return result;
}

void ReleasePatternResourceArrayNormal(RuntimeCStringArrayReference& reference,
    YtlReferenceControlRuntime* cachedControl) noexcept
{
    // CG 4A8C45 state0B：正常释放异常终止；普通 Pattern 用43B790重读当前槽。
    if (!cachedControl) return;
    if (--cachedControl->referenceCountAt04 == 0)
    {
        if (cachedControl->owningAt08) cachedControl->DestroyPayloadAtVslot00();
        cachedControl->DeleteControlAtVslot04(1);
    }
    reference.RawSlot().controlAt08 = nullptr;
    reference.RawSlot().baseAt00 = 0;
}

PatternSpriteUpdateKind ClassifyPatternSpriteUpdate(
    const std::string& currentLogicalNameCp932,
    const RuntimeCString& targetName,
    const ParamsPatternNative& pattern)
{
    const char* const name = targetName.GetString();
    if (targetName.GetLength() == 0)
        return PatternSpriteUpdateKind::ClearSlot;
    if (EqualsIgnoreCase(currentLogicalNameCp932.c_str(), name))
        return PatternSpriteUpdateKind::KeepCurrent;
    if (HasPatternResourceSuffixAt4CB905(targetName))
        return PatternSpriteUpdateKind::LoadDirectFile;

    // suffix的正常Free可能改写目标槽；逻辑查名按后续分支重新借用chars。
    const auto index = FindPatternItemByResourceName(pattern, targetName.GetString());
    if (!index) return PatternSpriteUpdateKind::MissingLogicalResource;
    switch (static_cast<PatternResourceKind>(ParamsPatternItemKindAt(pattern, *index)))
    {
    case PatternResourceKind::FileGroup:
        return ParamsPatternFileGroupAt(pattern, *index).stringsAt08.RawSlot().baseAt00
                   ? PatternSpriteUpdateKind::LoadFileGroup
                   : PatternSpriteUpdateKind::MissingLogicalResource;
    case PatternResourceKind::ExcPosition:
        return PatternSpriteUpdateKind::LoadExcPosition;
    case PatternResourceKind::FileConvert:
        return PatternSpriteUpdateKind::LoadFileConvert;
    case PatternResourceKind::FileNameItem:
    default:
        return PatternSpriteUpdateKind::MissingLogicalResource;
    }
}

FileGroupChange BuildFileGroupChangeMask(
    const RuntimeCStringArrayReference& targetComponents,
    const void* const primaryContext,
    const PatternResourcePrimaryNameReader readPrimary,
    const PatternResourceNamesBorrow currentAuxiliaryResources)
{
    return BuildFileGroupChangeMask(targetComponents, primaryContext, readPrimary,
        currentAuxiliaryResources, PatternResourceAccess::Checked);
}

FileGroupChange BuildFileGroupChangeMask(
    const RuntimeCStringArrayReference& targetComponents,
    const void* const primaryContext, const PatternResourcePrimaryNameReader readPrimary,
    const PatternResourceNamesBorrow currentAuxiliaryResources, PatternResourceAccess access)
{
    // 4CBBFD 仅检查 base；部分发布的非空 base/空 control 仍须读取首项。
    if (access == PatternResourceAccess::Checked && !targetComponents.RawSlot().baseAt00)
        throw std::invalid_argument("FileGroup requires array storage");

    const char* const primaryCharacters = (access == PatternResourceAccess::Checked ?
        targetComponents.At(0) : *targetComponents.Resolve(0)).GetString();
    // 4CBC59/4CD11D：目标字符槽为空抛E_FAIL，不交给CRT比较函数处理。
    if (!primaryCharacters) throw ATL::CAtlException(E_FAIL);
    // 4CBC65在目标检查之后才读当前primary；回调后不再重读目标字符槽。
    const char* const currentPrimaryResourceCharacters = readPrimary(primaryContext);
    FileGroupChange changes = FileGroupChange::AuxiliaryCount;
    if (!EqualsIgnoreCase(currentPrimaryResourceCharacters, primaryCharacters))
        changes = AddChange(changes, FileGroupChange::PrimaryResource);

    auto* control = static_cast<const EnvironmentObjectReferenceControl*>(
        targetComponents.RawSlot().controlAt08);
    const std::uint32_t targetCount = control ? control->countAt10 : 0u;
    if (targetCount == 1u)
        return changes;

    auto auxiliary = currentAuxiliaryResources.CaptureRange();
    if (targetCount - 1u != auxiliary.count)
        return changes;

    changes = static_cast<FileGroupChange>(
        static_cast<std::uint8_t>(changes) &
        ~static_cast<std::uint8_t>(FileGroupChange::AuxiliaryCount));
    if (auxiliary.count == 0u) return changes;
    for (std::uint32_t index = 0;;)
    {
        // 4CBCD0..4CBCF7：control保留上轮ECX，base/bias/stride在比较点读取。
        const auto& current = targetComponents.RawSlot();
        if (access == PatternResourceAccess::Checked && !current.baseAt00)
            throw std::out_of_range("CString array reference has no storage");
        const auto stride = control ? control->strideAt0C : 0u;
        const auto offset = (current.biasAt04 + index + 1u) * stride;
        const auto* const target = current.baseAt00 ?
            reinterpret_cast<const RuntimeCString*>(current.baseAt00 + offset) : nullptr;
        const char* const characters = target->GetString();
        if (!characters) throw ATL::CAtlException(E_FAIL);
        if (!EqualsIgnoreCase(auxiliary.Characters(index), characters))
        {
            changes = AddChange(changes, FileGroupChange::AuxiliaryResource);
            break;
        }
        // 4CBD17..27：同名后重捕获begin/count；unsigned门控通过才重取ECX。
        ++index;
        auxiliary = currentAuxiliaryResources.CaptureRange();
        if (index >= auxiliary.count) break;
        control = static_cast<const EnvironmentObjectReferenceControl*>(
            targetComponents.RawSlot().controlAt08);
    }
    return changes;
}

FileGroupChange BuildFileGroupChangeMask(
    const RuntimeCStringArrayReference& targetComponents,
    const char* const currentPrimaryResourceCharacters,
    const PatternResourceNamesBorrow currentAuxiliaryResources)
{
    return BuildFileGroupChangeMask(targetComponents, currentPrimaryResourceCharacters,
        &ReadBorrowedPrimaryCharacters, currentAuxiliaryResources);
}

FileGroupChange BuildFileGroupChangeMask(
    const RuntimeCStringArrayReference& targetComponents,
    const std::string& currentPrimaryResourceCp932,
    const std::vector<std::string>& currentAuxiliaryResourcesCp932)
{
    return BuildFileGroupChangeMask(targetComponents, currentPrimaryResourceCp932.c_str(),
        PatternResourceNamesBorrow(currentAuxiliaryResourcesCp932));
}
}
