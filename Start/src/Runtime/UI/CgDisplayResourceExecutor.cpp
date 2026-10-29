#include <Runtime/CgDisplayResourceExecutor.h>

#include <Runtime/PatternResourcePlan.h>
#include <Runtime/ParamsPatternNativeAccess.h>
#include <Runtime/RuntimeCStringException.h>

#include <mbstring.h>

#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
[[nodiscard]] bool EqualsIgnoreCase(
    const char* first,
    const char* second) noexcept
{
    return _mbsicmp(
               reinterpret_cast<const unsigned char*>(first),
               reinterpret_cast<const unsigned char*>(second)) == 0;
}

struct LocalName final
{
    explicit LocalName(IRuntimeStringManagerSource& strings) { InitializeEmptyRuntimeCString(cell.value, strings); }
    ~LocalName() noexcept { if (active) DestroyRuntimeCStringAt40CD40(&cell.value); }
    RuntimeCString& Value() noexcept { return cell.value; }
    void ReleaseNormal()
    {
        // 4A8C39 调用40CD40时仍有state8清理资格；失败展开读取当前串，不能提前撤销。
        DestroyRuntimeCStringAt40CD40(&cell.value);
        active = false;
    }
    union Cell
    {
        Cell() noexcept {}
        ~Cell() noexcept {}
        RuntimeCString value;
    } cell;
    bool active = true;
};

class ArrayFailureName final : public IPatternResourceFailureName
{
public:
    ArrayFailureName(const RuntimeCStringArrayReference& array, std::uint32_t index) noexcept
        : array_(array), index_(index) {}
    const RuntimeCString* ResolveFailurePrefix() const override { return array_.Resolve(index_); }
private:
    const RuntimeCStringArrayReference& array_;
    std::uint32_t index_;
};

class LocalFailureName final : public IPatternResourceFailureName
{
public:
    explicit LocalFailureName(const RuntimeCString& value) noexcept : value_(value) {}
    const RuntimeCString* ResolveFailurePrefix() const override { return &value_; }
private:
    const RuntimeCString& value_;
};

class EmptyCommit final : public IPatternResourceLoadCommit
{
public:
    void Commit() override {}
};

class ExcCommit final : public IPatternResourceLoadCommit
{
public:
    ExcCommit(ICgDisplayResourceExecutionTarget& target, PatternSpriteReceiver receiver,
        const PatternResourcePlan& plan, std::uint32_t slot, std::uint32_t x, std::uint32_t y) noexcept
        : target_(target), receiver_(receiver), plan_(plan), slot_(slot), x_(x), y_(y) {}
    void Commit() override
    {
        // 4A8C17..29：factory 仍存活，先写缓存 DWORD，再晚取 Plan 的命名源。
        target_.SetReceiverOpaqueValuesAt2CAnd30(receiver_, x_, y_);
        target_.SetReceiverLogicalName(receiver_, plan_.At(slot_));
    }
private:
    ICgDisplayResourceExecutionTarget& target_;
    PatternSpriteReceiver receiver_;
    const PatternResourcePlan& plan_;
    std::uint32_t slot_, x_, y_;
};

void ExecuteFileGroup(
    ICgDisplayResourceExecutionTarget& target,
    const std::uint32_t slotIndex,
    const PatternResourcePlan& plan,
    const RuntimeCStringArrayReference& components,
    YtlReferenceControlRuntime*& cachedControl,
    IRuntimeStringManagerSource& strings)
{
    const auto cachedBase = components.RawSlot().baseAt00;
    // 4A884B 的43F140位于成员装载前；base在此前缓存，control在其后4A8850读取。
    const auto receiver = target.CaptureSpriteReceiver(slotIndex);
    target.ClearReceiverFileGroup(receiver);
    cachedControl = components.RawSlot().controlAt08;
    const auto* const control = static_cast<const EnvironmentObjectReferenceControl*>(cachedControl);
    for (std::uint32_t memberIndex = 0;
         std::bit_cast<std::int32_t>(memberIndex) <
             std::bit_cast<std::int32_t>(control ? control->countAt10 : 0u);
         ++memberIndex)
    {
        const auto resolveMember = [&]() -> const RuntimeCString&
        {
            // EBX/EDI 保留 base/control；每次解析仍读取当前 bias 与控制块 stride。
            const YtlReferenceSlotRuntime member{
                cachedBase, components.RawSlot().biasAt04 + memberIndex, cachedControl};
            const auto* text = ResolveEnvironmentObjectReference<const RuntimeCString>(member);
            if (!text) throw std::logic_error("CG FileGroup member body is null (safe boundary)");
            return *text;
        };
        bool changed = true;
        if (memberIndex == 0)
        {
            const char* const characters = resolveMember().GetString();
            if (!characters) throw ATL::CAtlException(E_FAIL);
            changed = !EqualsIgnoreCase(target.ReceiverPrimaryNameCharacters(receiver), characters);
        }
        else
        {
            const auto auxiliary = target.ReceiverAuxiliaryNames(receiver).CaptureRange();
            // 4A88A8 的 count < memberIndex 直接装载，不提前解析待比较的串。
            if (std::bit_cast<std::int32_t>(memberIndex) <=
                std::bit_cast<std::int32_t>(auxiliary.count))
            {
                const char* const characters = resolveMember().GetString();
                if (!characters) throw ATL::CAtlException(E_FAIL);
                changed = !EqualsIgnoreCase(auxiliary.Characters(memberIndex - 1u), characters);
            }
        }
        if (!changed) continue;
        // 4A88ED 在实际装载前重新解析，不复用比较时的字符指针。
        const PatternResourceLoadRequest request{PatternResourceLoadPath::CgGroup,
            resolveMember().GetString(), memberIndex, 0};
        // 错误前缀用当前整个 Ref 重解，不借用成功路径缓存的 EBX/EDI。
        ArrayFailureName failureName(components, memberIndex);
        EmptyCommit commit;
        target.LoadResource(slotIndex, receiver, request, failureName, commit, strings);
    }
    // 各 member factory 已释放；整组始终写入同一借用 receiver。
    target.SetReceiverLogicalName(receiver, plan.At(slotIndex));
    target.SetReceiverActive(receiver, true);
}

void ExecuteSlot(
    const IParamsPatternSource& source,
    ICgDisplayResourceExecutionTarget& target,
    const std::uint32_t slotIndex,
    const PatternResourcePlan& plan,
    IRuntimeStringManagerSource& strings)
{
    if (slotIndex >= plan.Size() || plan.At(slotIndex).GetLength() == 0)
    {
        target.ClearPrimarySprite(slotIndex);
        return;
    }
    const char* const currentCharacters = target.PrimaryLogicalNameCharacters(slotIndex);
    // 4A863D/4A8EE5：当前logical字符槽为空走E_FAIL，再由外层catch-all折叠。
    if (!currentCharacters) throw ATL::CAtlException(E_FAIL);
    if (EqualsIgnoreCase(currentCharacters, plan.At(slotIndex).GetString()))
    {
        return;
    }
    // 4A867B在scanner GetNil前捕获chars；CG没有普通Pattern的额外414540输入副本。
    if (HasCgDisplayResourceSuffixAt4A8680(plan.At(slotIndex).GetString(), strings))
    {
        const PatternResourceLoadRequest request{PatternResourceLoadPath::CgDirect,
            plan.At(slotIndex).GetString(), 0, 0};
        ArrayFailureName failureName(plan, slotIndex);
        EmptyCommit commit;
        target.LoadResource(slotIndex, 0, request, failureName, commit, strings);
        return;
    }

    // 4A8732 先缓存搜索字符，4A8734 再取得当前 Pattern，不强持有正文。
    const char* const groupCharacters = plan.At(slotIndex).GetString();
    const auto* groupPattern = source.CapturePattern();
    if (!groupPattern) throw std::logic_error("Pattern source resolves to null (safe boundary)");
    auto components = FindPatternFileGroupByResourceName(*groupPattern, groupCharacters);
    auto* cachedControl = components.RawSlot().controlAt08;
    if (components.RawSlot().baseAt00)
    {
        ExecuteFileGroup(
            target, slotIndex, plan, components, cachedControl, strings);
        ReleasePatternResourceArrayNormal(components, cachedControl);
        return;
    }

    LocalName physical(strings);
    // 4A8A2D..4A8AAD：先构造临时串，再缓存名称并重新捕获 Pattern。
    const char* const logicalCharacters = plan.At(slotIndex).GetString();
    const auto* pattern = source.CapturePattern();
    if (!pattern) throw std::logic_error("Pattern source resolves to null (safe boundary)");
    const auto index = FindPatternItemByResourceName(*pattern, logicalCharacters);
    if (!index || ParamsPatternItemKindAt(*pattern, *index) != 2u)
    {
        // 4A8FA5..4A8FCB：物理名局部串和FileGroup Ref在外层catch前展开。
        ThrowRuntimeCStringConcatAt466760(plan.At(slotIndex),
            " \x83\x74\x83\x40\x83\x43\x83\x8B\x82\xAA\x82\xA0\x82\xE8"
            "\x82\xDC\x82\xB9\x82\xF1\x81\x49\x81\x49", strings);
    }

    AssignRuntimeCStringAt410220(physical.Value(), ParamsPatternExcPositionAt(*pattern, *index).subNameAt08);
    // 4A8B19 只 raw 重解本次正文的当前槽；不重新取根或重验 kind。
    const auto* position = ResolveParamsPatternExcPosition(
        ParamsPatternReferenceAt(pattern->itemsAt04, *index));
    if (!position) throw std::logic_error("Pattern ExcPosition body is null (safe boundary)");
    const auto x = position->xAt0C;
    const auto y = position->yAt10;
    // 4A8B45 在两 DWORD 之后缓存 chars，随后固定 Sprite；后续不再按 index 取目标。
    const char* const characters = physical.Value().GetString();
    const auto receiver = target.CaptureSpriteReceiver(slotIndex);
    const PatternResourceLoadRequest request{PatternResourceLoadPath::CgExc, characters, 0, 0};
    LocalFailureName failureName(physical.Value());
    ExcCommit commit(target, receiver, plan, slotIndex, x, y);
    target.LoadResource(slotIndex, receiver, request, failureName, commit, strings);
    physical.ReleaseNormal();
    ReleasePatternResourceArrayNormal(components, cachedControl);
}
}

bool ExecuteCgDisplayResourceSelectionAt4A84E0(
    const IParamsPatternSource& source,
    const CgDisplaySelectionState& selection,
    ICgDisplayResourceExecutionTarget& target,
    IRuntimeStringManagerSource& strings)
{
    try
    {
        // 与原函数相同：选择索引由前一阶段保证有效，本函数不新增范围分支。
        const auto selectedIndex = static_cast<std::size_t>(selection.selectedMappingIndexAt28);
        // 4A8527 固定 pair 地址，4A852A 取根，4A8550 才读取该 pair 的 second。
        const auto* selectedMapping = &selection.mappingPairsAt2C[selectedIndex];
        const auto* pattern = source.CapturePattern();
        if (!pattern) throw std::logic_error("Pattern source resolves to null (safe boundary)");
        const PatternResourcePlan plan =
            BuildPatternResourcePlan(
                *pattern, static_cast<std::uint32_t>(selectedMapping->second), strings);

        for (std::uint32_t slotIndex = 0;
             slotIndex < target.PrimarySpriteSlotCount();
             ++slotIndex)
        {
            ExecuteSlot(
                source, target, slotIndex, plan, strings);
        }

        target.SetCgDisplayWindowActiveAt0C(true);
        // 4A8E37 只保留选择索引，装载完成后从当前 mapping begin 读取 first。
        const auto selectedValue = selection.mappingPairsAt2C[selectedIndex].first;
        target.SetCgOperationWindowActiveAt0C(true);
        target.SetCgOperationDialogSelectionValueAt7C(selectedValue);
        target.OpenCgOperationDialogAtVslot44();
        return true;
    }
    catch (...)
    {
        // 4A8ECB 的 catch(...) 不回滚已经产生的 Sprite 副作用。
        return false;
    }
}
}
