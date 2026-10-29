#pragma once

#include <Runtime/ScrSaveParamsRuntimeState.h>
#include <Runtime/ScrSaveParams2Native.h>
#include <Runtime/ScriptCallStacksNative.h>

namespace KaGuYa::Start::Runtime
{
// 45B020：CParams是70h非虚对象；业务数据不混入现代分配器或有效性记录。
struct ScriptParamsNativeBody final
{
    static constexpr std::size_t ValueCount = ScrSaveParams2Native::ValueCount;
    explicit ScriptParamsNativeBody(
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    ~ScriptParamsNativeBody() noexcept;
    ScriptParamsNativeBody(const ScriptParamsNativeBody&) = delete;
    ScriptParamsNativeBody& operator=(const ScriptParamsNativeBody&) = delete;

    void ResetAt46CE30(IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    [[nodiscard]] ScrSaveParamsRuntimeState* ResolveSaveParamsAt08() const noexcept;
    [[nodiscard]] ScrSaveParams2Native* ResolveSaveParams2At30() const noexcept;
    [[nodiscard]] ScrSaveParamsRuntimeState& SaveParamsAt08() noexcept { return *ResolveSaveParamsAt08(); }
    [[nodiscard]] const ScrSaveParamsRuntimeState& SaveParamsAt08() const noexcept { return *ResolveSaveParamsAt08(); }
    [[nodiscard]] ScrSaveParams2Native& SaveParams2At30() noexcept { return *ResolveSaveParams2At30(); }
    [[nodiscard]] const ScrSaveParams2Native& SaveParams2At30() const noexcept { return *ResolveSaveParams2At30(); }
    [[nodiscard]] YtlReferenceSlotRuntime& SaveParamsRefAt08() noexcept { return saveParamsAt08; }
    [[nodiscard]] const YtlReferenceSlotRuntime& SaveParamsRefAt08() const noexcept { return saveParamsAt08; }
    [[nodiscard]] YtlReferenceSlotRuntime& SaveParams2RefAt30() noexcept { return saveParams2At30; }
    [[nodiscard]] const YtlReferenceSlotRuntime& SaveParams2RefAt30() const noexcept { return saveParams2At30; }
    [[nodiscard]] ScriptFileCallStorage& FileCallStackAt18() noexcept { return fileCallsAt18; }
    [[nodiscard]] const ScriptFileCallStorage& FileCallStackAt18() const noexcept { return fileCallsAt18; }
    [[nodiscard]] ScriptLocalCallStorage& LocalCallStackAt24() noexcept { return localCallsAt24; }
    [[nodiscard]] const ScriptLocalCallStorage& LocalCallStackAt24() const noexcept { return localCallsAt24; }
    [[nodiscard]] std::uint64_t GetSaveTimestamp() const noexcept;
    void SetSaveTimestamp(std::uint64_t value) noexcept;

    std::uint32_t timestampLowAt00;
    std::uint32_t timestampHighAt04;
    YtlReferenceSlotRuntime saveParamsAt08;
    std::uint32_t programCounter;
    union { ScriptFileCallStorage fileCallsAt18; };
    union { ScriptDwordVectorStorage localCallsAt24; };
    YtlReferenceSlotRuntime saveParams2At30;
    union { ScriptDwordVectorStorage historyAt3C; };
    union { ScriptDwordVectorStorage valuesAt48; };
    YtlReferenceSlotRuntime referenceAt54;
    std::int32_t messageFormatIndexAt60;
    union { ScriptDwordVectorStorage menuCandidatesAt64; };

private:
    void DestroyMembers() noexcept;
};
static_assert(sizeof(ScriptParamsNativeBody) == 0x70);
static_assert(alignof(ScriptParamsNativeBody) == 4);
static_assert(offsetof(ScriptParamsNativeBody, saveParamsAt08) == 0x08);
static_assert(offsetof(ScriptParamsNativeBody, programCounter) == 0x14);
static_assert(offsetof(ScriptParamsNativeBody, fileCallsAt18) == 0x18);
static_assert(offsetof(ScriptParamsNativeBody, localCallsAt24) == 0x24);
static_assert(offsetof(ScriptParamsNativeBody, saveParams2At30) == 0x30);
static_assert(offsetof(ScriptParamsNativeBody, historyAt3C) == 0x3C);
static_assert(offsetof(ScriptParamsNativeBody, valuesAt48) == 0x48);
static_assert(offsetof(ScriptParamsNativeBody, referenceAt54) == 0x54);
static_assert(offsetof(ScriptParamsNativeBody, messageFormatIndexAt60) == 0x60);
static_assert(offsetof(ScriptParamsNativeBody, menuCandidatesAt64) == 0x64);

// 两个入口都不Ensure根；+1C0可处于base已发布而control分配失败的合法失败前缀。
ScriptParamsNativeBody* GetParamsAt47BDB0(YtlReferenceSlotRuntime& slot,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
void ReplaceParamsAt481720(YtlReferenceSlotRuntime& slot,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
