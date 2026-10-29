#include <Runtime/PatternResourceExecutor.h>
#include <Runtime/ParamsPatternNativeAccess.h>
#include <Runtime/RuntimeCStringException.h>

#include <mbstring.h>

#include <bit>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原PE 5B5D90/5B5DB4。4CD127与4CD05B分别抛char*，不能合并成现代异常。
char oldDrawableError[] =
    "\x91\x4F\x89\xF1\x95\x5C\x8E\xA6\x82\xB3\x82\xEA\x82\xBD\x97\xCC"
    "\x88\xE6\x82\xCC\x8E\xE6\x93\xBE\x82\xC9\x8E\xB8\x94\x73\x81\x49";
char newDrawableError[] =
    "\x8D\x58\x90\x56\x97\xCC\x88\xE6\x82\xCC\x8E\xE6\x93\xBE\x82\xC9"
    "\x8E\xB8\x94\x73\x81\x49";

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
    void ReleaseNormal(const char* cachedCharacters)
    {
        // 4CC6C3/4CC9C0 的普通尾声先撤局部串资格，再释放缓存 ESI。
        active = false;
        ReleaseRuntimeCStringHeader(reinterpret_cast<ATL::CStringData*>(
            reinterpret_cast<std::uintptr_t>(cachedCharacters) - sizeof(ATL::CStringData)));
    }
    void ReleaseCurrentNormal()
    {
        // 4CD08F..99：Convert 未命中时撤资格后 40CD40 读取当前槽，区别于 Exc 的 ESI。
        active = false;
        DestroyRuntimeCStringAt40CD40(&cell.value);
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

struct PrimaryNameSource final
{
    const IPatternResourceExecutionTarget& target;
    PatternSpriteReceiver receiver;
};

const char* ReadPrimaryName(const void* context)
{
    const auto& source = *static_cast<const PrimaryNameSource*>(context);
    return source.target.ReceiverPrimaryNameCharacters(source.receiver);
}

[[nodiscard]] RECT OffsetToLayer(
    const RECT& local,
    const IPatternResourceExecutionTarget& target) noexcept
{
    RECT result = local;
    const auto y = target.LayerY();
    const auto x = target.LayerX();
    OffsetRect(&result, x, y);
    return result;
}

void InvalidateResult(
    IPatternResourceExecutionTarget& target,
    const PatternSpriteRectResult& drawable,
    char* const failureMessage)
{
    switch (drawable.state)
    {
    case PatternSpriteRectState::Valid:
        // 4CBA7D/4CC67E在加layer偏移前判断全零，不是IsRectEmpty。
        if (drawable.rect.left || drawable.rect.top || drawable.rect.right || drawable.rect.bottom)
            target.Invalidate(OffsetToLayer(drawable.rect, target));
        return;
    case PatternSpriteRectState::Empty:
    case PatternSpriteRectState::InvalidLoadMode:
    default:
        throw failureMessage;
    }
}

class DirectCommit final : public IPatternResourceLoadCommit
{
public:
    DirectCommit(IPatternResourceExecutionTarget& target, std::uint32_t slot) noexcept
        : target_(target), slot_(slot) {}
    void Commit() override
    {
        // 4CCC26..50：装载后重新解析当前槽，不复用装载 receiver，也不再做 load gate。
        const auto receiver = target_.CaptureSpriteReceiver(slot_);
        InvalidateResult(target_, target_.ReceiverDrawableRect(receiver), newDrawableError);
    }
private:
    IPatternResourceExecutionTarget& target_;
    std::uint32_t slot_;
};

template<PatternResourceAccess Access>
class NamedCommit final : public IPatternResourceLoadCommit
{
public:
    NamedCommit(IPatternResourceExecutionTarget& target, PatternSpriteReceiver receiver,
        const PatternResourcePlan& plan, std::uint32_t slot, bool writeOrigin,
        std::int32_t x, std::int32_t y) noexcept
        : target_(target), receiver_(receiver), plan_(plan), slot_(slot),
          writeOrigin_(writeOrigin), x_(x), y_(y) {}
    void Commit() override
    {
        // Exc 先写缓存坐标，再从当前 Plan 解析命名源；Convert 没有坐标写入。
        if (writeOrigin_) target_.SetReceiverOrigin(receiver_, x_, y_);
        target_.SetReceiverLogicalName(receiver_, PatternResourceNameAt<Access>(plan_, slot_));
        InvalidateResult(target_, target_.ReceiverDrawableRect(receiver_), newDrawableError);
    }
private:
    IPatternResourceExecutionTarget& target_;
    PatternSpriteReceiver receiver_;
    const PatternResourcePlan& plan_;
    std::uint32_t slot_;
    bool writeOrigin_;
    std::int32_t x_, y_;
};

template<PatternResourceAccess Access>
void ExecuteFileGroup(
    IPatternResourceExecutionTarget& target,
    const std::uint32_t slotIndex,
    const PatternResourcePlan& plan,
    const RuntimeCStringArrayReference& components,
    IRuntimeStringManagerSource& strings)
{
    // 4CBC05..30 在 mask 和 Prepare 之前固定；整组不得因回调改槽而重新取 Sprite。
    const auto receiver = target.CaptureSpriteReceiver(slotIndex);
    const PrimaryNameSource primary{target, receiver};
    // borrow只记录头地址；count==1的mask不捕获辅助表，也不构造空vector替身。
    const auto auxiliary = target.ReceiverAuxiliaryNames(receiver);
    const FileGroupChange changes = BuildFileGroupChangeMask(
        components, &primary, &ReadPrimaryName, auxiliary, Access);
    if (changes == FileGroupChange::None)
        return;

    // 原函数先清理受变更位影响的状态，再逐个装载；后续失败不会恢复这里的修改。
    target.PrepareReceiverFileGroup(receiver, changes);
    auto* control = static_cast<const EnvironmentObjectReferenceControl*>(components.RawSlot().controlAt08);
    for (std::uint32_t memberIndex = 0;
         std::bit_cast<std::int32_t>(memberIndex) <
             std::bit_cast<std::int32_t>(control ? control->countAt10 : 0u);
         ++memberIndex)
    {
        const auto resolveMember = [&]() -> const RuntimeCString&
        {
            // 4CBE39/85/D5：本轮ECX不因借用辅助表而刷新，base/bias/stride现场读取。
            const auto& current = components.RawSlot();
            if constexpr (Access == PatternResourceAccess::Checked)
                if (!current.baseAt00)
                    throw std::out_of_range("CString array reference has no storage");
            const auto stride = control ? control->strideAt0C : 0u;
            const auto* value = current.baseAt00 ? reinterpret_cast<const RuntimeCString*>(
                current.baseAt00 + (current.biasAt04 + memberIndex) * stride) : nullptr;
            return *value;
        };
        bool changed;
        if (memberIndex == 0)
        {
            const char* const characters = resolveMember().GetString();
            // 4CBE5D：先验目标字符，再取固定receiver的当前primary字符。
            if (!characters) throw ATL::CAtlException(E_FAIL);
            changed = !EqualsIgnoreCase(target.ReceiverPrimaryNameCharacters(receiver), characters);
            control = static_cast<const EnvironmentObjectReferenceControl*>(components.RawSlot().controlAt08);
        }
        else
        {
            const auto current = target.ReceiverAuxiliaryNames(receiver).CaptureRange();
            // 4CBE81/83用signed count；不足时直接装载，不读目标或辅助字符。
            changed = std::bit_cast<std::int32_t>(current.count) <
                std::bit_cast<std::int32_t>(memberIndex);
            if (!changed)
            {
                const char* const characters = resolveMember().GetString();
                if (!characters) throw ATL::CAtlException(E_FAIL);
                changed = !EqualsIgnoreCase(current.Characters(memberIndex - 1u), characters);
                // 4CBEBF：仅实际比较后刷新control，数量不足的直装分支保留原ECX。
                control = static_cast<const EnvironmentObjectReferenceControl*>(components.RawSlot().controlAt08);
            }
        }
        if (changed)
        {
            // 4CBED5重新解析当前字符，不沿用比较时的指针。
            const PatternResourceLoadRequest request{PatternResourceLoadPath::NormalGroup,
                resolveMember().GetString(), memberIndex, 0, Access};
            ArrayFailureName failureName(components, memberIndex);
            EmptyCommit commit;
            target.LoadResource(slotIndex, receiver, request, failureName, commit, strings);
            // 4CC0CA：本member factory退役完成，下一轮才改用当前control。
            control = static_cast<const EnvironmentObjectReferenceControl*>(components.RawSlot().controlAt08);
        }
    }

    // 4CC0E6：全部 member factory 已释放，才做组末命名/active/rect。
    target.SetReceiverLogicalName(receiver, PatternResourceNameAt<Access>(plan, slotIndex));
    target.SetReceiverActive(receiver, true);
    InvalidateResult(target, target.ReceiverDrawableRect(receiver), newDrawableError);
}

template<PatternResourceAccess Access>
void ExecuteEntry(
    const IParamsPatternSource& source,
    IPatternResourceExecutionTarget& target,
    const std::uint32_t slotIndex,
    const PatternResourcePlan& plan,
    IRuntimeStringManagerSource& strings)
{
    const bool inRange = slotIndex < plan.Size();
    const auto* name = inRange ? plan.Resolve(slotIndex) : nullptr;
    const char* characters = "";
    bool empty;
    if constexpr (Access == PatternResourceAccess::Checked)
    {
        characters = name ? name->GetString() : "";
        empty = !name || name->GetLength() == 0;
    }
    else empty = !inRange || name->GetLength() == 0;
    if (!empty)
    {
        const char* const currentCharacters = target.LogicalNameCharacters(slotIndex);
        // 4CB8B3/4CD11D：当前logical的空字符槽不是空逻辑名。
        if (!currentCharacters) throw ATL::CAtlException(E_FAIL);
        // 4CB892先取logical，4CB8BC才读当前Plan字符；不沿用长度门控时的槽。
        if constexpr (Access == PatternResourceAccess::Native)
            characters = PatternResourceNameAt<Access>(plan, slotIndex).GetString();
        if (EqualsIgnoreCase(currentCharacters, characters)) return;
    }

    // 4CB905..4CB9B5先构造/释放suffix链并固定分类，之后才失效旧显示区域。
    const bool directFile = !empty && HasPatternResourceSuffixAt4CB905(PatternResourceNameAt<Access>(plan, slotIndex), strings);

    // 0x4CB980..0x4CBA23：当前逻辑名非空时，变更前先失效旧显示区域。
    if (target.LogicalNameLength(slotIndex) != 0)
        InvalidateResult(target, target.LocalDrawableRect(slotIndex), oldDrawableError);

    if (empty)
    {
        target.ClearSlot(slotIndex);
        return;
    }
    if (directFile)
    {
        const PatternResourceLoadRequest request{PatternResourceLoadPath::NormalDirect,
            PatternResourceNameAt<Access>(plan, slotIndex).GetString(), 0, 0, Access};
        ArrayFailureName failureName(plan, slotIndex);
        DirectCommit commit(target, slotIndex);
        // Direct 不提前 capture；target action 在 reader/registry 之后解析。
        target.LoadResource(slotIndex, 0, request, failureName, commit, strings);
        return;
    }
    {
        // 4CBB05 先缓存字符，4CBBBB 再取当前根；查询只借用本次正文。
        const char* const logicalCharacters = PatternResourceNameAt<Access>(plan, slotIndex).GetString();
        const auto* pattern = source.CapturePattern();
        if constexpr (Access == PatternResourceAccess::Checked)
            if (!pattern) throw std::logic_error("Pattern source resolves to null (safe boundary)");
        auto components = FindPatternFileGroupByResourceName(*pattern, logicalCharacters, Access);
        if (components.RawSlot().baseAt00)
        {
            ExecuteFileGroup<Access>(target, slotIndex, plan, components, strings);
            return;
        }
        // 4CC223/4CC27A 正常与展开均按当前槽释放；析构 noexcept 对应 state1E/1F。
    }
    {
        LocalName physical(strings);
        const char* const initialCharacters = physical.Value().GetString();
        // 4CC28D..4CC3AC：局部串 GetNil 完成后缓存搜索名，再独立取根。
        const char* const logicalCharacters = PatternResourceNameAt<Access>(plan, slotIndex).GetString();
        const auto* pattern = source.CapturePattern();
        if constexpr (Access == PatternResourceAccess::Checked)
            if (!pattern) throw std::logic_error("Pattern source resolves to null (safe boundary)");
        const auto index = FindPatternItemByResourceName(*pattern, logicalCharacters, Access);
        if (index && ParamsPatternItemKindAt(*pattern, *index, Access) == 2u)
        {
            AssignRuntimeCStringAt410220(physical.Value(), ParamsPatternExcPositionAt(*pattern, *index, Access).subNameAt08);
            // 4CC429 只重解本次 Pattern 的当前槽，不重取根或再验 kind。
            const auto* position = ResolveParamsPatternExcPosition(
                ParamsPatternReferenceAt(pattern->itemsAt04, *index, Access));
            const char* const cachedCharacters = physical.Value().GetString();
            if constexpr (Access == PatternResourceAccess::Checked)
                if (!position) throw std::logic_error("Pattern ExcPosition body is null (safe boundary)");
            const auto x = std::bit_cast<std::int32_t>(position->xAt0C);
            const auto y = std::bit_cast<std::int32_t>(position->yAt10);
            const auto receiver = target.CaptureSpriteReceiver(slotIndex);
            const PatternResourceLoadRequest request{PatternResourceLoadPath::NormalExc, cachedCharacters, 0, 0, Access};
            LocalFailureName failureName(physical.Value());
            NamedCommit<Access> commit(target, receiver, plan, slotIndex, true, x, y);
            target.LoadResource(slotIndex, receiver, request, failureName, commit, strings);
            physical.ReleaseNormal(cachedCharacters);
            return;
        }
        physical.ReleaseNormal(initialCharacters);
    }
    {
        LocalName physical(strings);
        // 4CC704..4CC813：Exc 未命中并正常释放后，重新构串、缓存名、取根。
        const char* const logicalCharacters = PatternResourceNameAt<Access>(plan, slotIndex).GetString();
        const auto* pattern = source.CapturePattern();
        if constexpr (Access == PatternResourceAccess::Checked)
            if (!pattern) throw std::logic_error("Pattern source resolves to null (safe boundary)");
        const auto index = FindPatternItemByResourceName(*pattern, logicalCharacters, Access);
        if (index && ParamsPatternItemKindAt(*pattern, *index, Access) == 3u)
        {
            AssignRuntimeCStringAt410220(physical.Value(), ParamsPatternFileConvertAt(*pattern, *index, Access).subNameAt08);
            // 4CC890..8A9 在赋值后 raw 重解并读取 flags，没有第二次 kind 检查。
            const auto* converted = ResolveParamsPatternFileConvert(
                ParamsPatternReferenceAt(pattern->itemsAt04, *index, Access));
            const char* const cachedCharacters = physical.Value().GetString();
            if constexpr (Access == PatternResourceAccess::Checked)
                if (!converted) throw std::logic_error("Pattern FileConvert body is null (safe boundary)");
            const auto flags = converted->flagsAt0C;
            const auto receiver = target.CaptureSpriteReceiver(slotIndex);
            const PatternResourceLoadRequest request{PatternResourceLoadPath::NormalFileConvert,
                cachedCharacters, 0, flags, Access};
            LocalFailureName failureName(physical.Value());
            NamedCommit<Access> commit(target, receiver, plan, slotIndex, false, 0, 0);
            target.LoadResource(slotIndex, receiver, request, failureName, commit, strings);
            physical.ReleaseNormal(cachedCharacters);
            return;
        }
        physical.ReleaseCurrentNormal();
    }
    // 4CD09E..4CD16F：从当前名称表重解CString，以原CP932后缀拼接并抛真实CStringT。
    const auto& prefix = [&]() -> const RuntimeCString&
    {
        if constexpr (Access == PatternResourceAccess::Checked) return plan.At(slotIndex);
        else return PatternResourceFailurePrefixAt466797(plan.Resolve(slotIndex));
    }();
    ThrowRuntimeCStringConcatAt466760(prefix,
        " \x83\x74\x83\x40\x83\x43\x83\x8B\x82\xAA\x82\xA0\x82\xE8"
        "\x82\xDC\x82\xB9\x82\xF1\x81\x49\x81\x49", strings);
}
}

PatternResourceExecutionException::PatternResourceExecutionException(
    const PatternResourceExecutionError error,
    const std::size_t spriteSlotIndex,
    std::string resourceNameCp932)
    : std::runtime_error("Pattern resource execution failed"),
      error_(error),
      spriteSlotIndex_(spriteSlotIndex),
      resourceNameCp932_(std::move(resourceNameCp932))
{
}

PatternResourceExecutionError PatternResourceExecutionException::Error() const noexcept
{
    return error_;
}

std::size_t PatternResourceExecutionException::SpriteSlotIndex() const noexcept
{
    return spriteSlotIndex_;
}

const std::string& PatternResourceExecutionException::ResourceNameCp932() const noexcept
{
    return resourceNameCp932_;
}

void ExecutePatternResourcePlan(
    const IParamsPatternSource& source,
    IPatternResourceExecutionTarget& target,
    const PatternResourcePlan& plan,
    IRuntimeStringManagerSource& strings)
{
    for (std::uint32_t slotIndex = 0; slotIndex < target.SpriteSlotCount(); ++slotIndex)
        ExecuteEntry<PatternResourceAccess::Checked>(source, target, slotIndex, plan, strings);
}

void ExecutePatternResourcePlanNative(const IParamsPatternSource& source,
    IPatternResourceExecutionTarget& target, const PatternResourcePlan& plan,
    IRuntimeStringManagerSource& strings)
{
    for (std::uint32_t slotIndex = 0; slotIndex < target.SpriteSlotCount(); ++slotIndex)
        ExecuteEntry<PatternResourceAccess::Native>(source, target, slotIndex, plan, strings);
}

ScrPatternResourceItemLoader::ScrPatternResourceItemLoader(
    const IParamsPatternSource& source,
    IScrResultPatternLayer& resultLayer,
    IPatternResourceExecutionTarget& executionTarget) noexcept
    : source_(source),
      resultLayer_(resultLayer),
      executionTarget_(executionTarget)
{
}

void ScrPatternResourceItemLoader::LoadPatternItem(
    IScrResultPatternLayer& layer,
    const std::int32_t patternItemIndex)
{
    if (&layer != &resultLayer_)
        throw std::invalid_argument("Pattern resource loader was bound to another layer");
    if (patternItemIndex < 0)
        throw std::invalid_argument("Pattern item index must not be negative");

    // 4CB7D6..4CB800：到生成名称表时才捕获正文；执行阶段不沿用这个借用。
    const auto* pattern = source_.CapturePattern();
    if (!pattern) throw std::logic_error("Pattern source resolves to null (safe boundary)");
    const auto plan = BuildPatternResourcePlan(*pattern, static_cast<std::uint32_t>(patternItemIndex));
    ExecutePatternResourcePlan(source_, executionTarget_, plan);
}
}
