#include <Runtime/ParamsSettingTagReader.h>
#include <Runtime/ParamsRuntimeReadCursor.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>

#include <bit>
#include <cstdlib>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
ATL::CStringData* Header(const char* characters) noexcept
{
    return reinterpret_cast<ATL::CStringData*>(
        reinterpret_cast<std::uintptr_t>(characters) - sizeof(ATL::CStringData));
}

class LoopText final
{
public:
    explicit LoopText(IRuntimeStringManagerSource& strings)
    {
        InitializeEmptyRuntimeCString(cell_.value, strings);
        cachedCharacters = cell_.value.GetString();
    }
    ~LoopText() noexcept { if (active_) DestroyRuntimeCStringAt40CD40(&cell_.value); }
    void Read(ParamsRuntimeReadCursor& cursor)
    {
        const auto before = cursor.Offset();
        cursor.SettingText16At420045(cell_.value);
        // 420160/420286仅非零S重新捕获字符；零S沿用跨迭代的EDI/ESI。
        if (cursor.Offset() - before != 2) cachedCharacters = cell_.value.GetString();
    }
    void ReleaseNormal()
    {
        active_ = false;
        ReleaseRuntimeCStringHeader(Header(cachedCharacters));
    }
    const char* cachedCharacters = nullptr;
private:
    union Cell { RuntimeCString value; Cell() noexcept {} ~Cell() noexcept {} } cell_;
    bool active_ = true;
};

class TemporaryPair final
{
public:
    TemporaryPair(const char* key, const char* value)
    {
        Cell borrowed;
        std::memcpy(&borrowed.value.keyAt00, &key, sizeof(key));
        std::memcpy(&borrowed.value.valueAt04, &value, sizeof(value));
        CloneParamsSettingPairAt42029C(cell_.value, borrowed.value);
    }
    ~TemporaryPair() noexcept { if (active_) DestroyParamsSettingPairAt40CE90(cell_.value); }
    ParamsSettingPairRuntime& Value() noexcept { return cell_.value; }
    void ReleaseNormal()
    {
        active_ = false; // 420324先降到state1，正常Free失败不能重试整个pair。
        ReleaseParamsSettingPairAt420324(cell_.value);
    }
private:
    union Cell { ParamsSettingPairRuntime value; Cell() noexcept {} ~Cell() noexcept {} } cell_;
    bool active_ = true;
};

class ReferenceScope final
{
public:
    explicit ReferenceScope(YtlReferenceSlotRuntime& reference) noexcept : reference_(reference) {}
    ~ReferenceScope() noexcept { if (active_) ReleaseYtlReference(reference_); }
    void Disarm() noexcept { active_ = false; }
private:
    YtlReferenceSlotRuntime& reference_;
    bool active_ = true;
};

void ReleaseAppendTemporary(YtlReferenceSlotRuntime& temporary,
    YtlReferenceControlRuntime* control) noexcept
{
    // 420461 state0B：payload后仍用缓存control；展开state0A才走currentRef。
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
    temporary.controlAt08 = nullptr;
    temporary.baseAt00 = 0;
}

void ReleaseLoopChild(YtlReferenceSlotRuntime& child) noexcept
{
    // 4204A8 state0C：payload后重读control；原尾声不清这三个局部槽。
    auto* control = child.controlAt08;
    if (!control || --control->referenceCountAt04 != 0) return;
    if (control->owningAt08) control->DestroyPayloadAtVslot00();
    control = child.controlAt08;
    if (control) control->DeleteControlAtVslot04(1);
}

ParamsSettingTagRuntime& RequireCurrentBody(const YtlReferenceSlotRuntime& reference) noexcept
{
    auto* const body = ResolveParamsSettingTagAt410560(reference);
    // 原实际成员访问处是坏地址故障；明确终止，不静默跳过节点或构造空替代品。
    if (!body) _invalid_parameter_noinfo_noreturn();
    return *body;
}
}

void ReadParamsSettingTagAt41FE80(ParamsRuntimeReadCursor& cursor, YtlReferenceSlotRuntime& target,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    auto local = cursor;
    RebuildParamsSettingTagAt41FE80(target, strings, memory);
    auto* const nameBody = ResolveParamsSettingTagAt410560(target);
    local.Text16(nameBody ? &nameBody->nameAt00 : nullptr);
    const auto pairCount = std::bit_cast<std::int32_t>(local.U32());
    if (pairCount != 0)
    {
        // 负signed非零仍构造两串再正常释放，只有零计数跳过Capture/GetNil。
        LoopText key(strings);
        LoopText value(strings);
        for (std::int32_t index = 0; index < pairCount; ++index)
        {
            key.Read(local);
            value.Read(local);
            TemporaryPair pair(key.cachedCharacters, value.cachedCharacters);
            auto& current = RequireCurrentBody(target);
            AppendParamsSettingPairAt4202CF(current.pairsAt04, pair.Value(), memory);
            pair.ReleaseNormal();
        }
        value.ReleaseNormal();
        key.ReleaseNormal();
    }
    const auto childCount = std::bit_cast<std::int32_t>(local.U32());
    if (childCount != 0)
    {
        YtlReferenceSlotRuntime child;
        ReferenceScope childScope(child);
        for (std::int32_t index = 0; index < childCount; ++index)
        {
            ReadParamsSettingTagAt41FE80(local, child, strings, memory);
            auto& children = RequireCurrentBody(target).childrenAt10;
            auto* control = child.controlAt08;
            auto temporary = child;
            ReferenceScope temporaryScope(temporary);
            if (control) ++control->referenceCountAt04;
            const bool grows = children.end == children.capacity;
            AppendEnvironmentArchiveReferenceAt49852D(children, temporary, memory);
            if (grows) control = temporary.controlAt08;
            temporaryScope.Disarm();
            ReleaseAppendTemporary(temporary, control);
        }
        childScope.Disarm();
        ReleaseLoopChild(child);
    }
    // 4204D1只返回已消费量，不能在尾声释放回调之后再校验一次输入长度。
    // local始终借用同一输入owner；只提交这次完整读取，不回滚已发布正文/引用。
    cursor = local;
}
}
