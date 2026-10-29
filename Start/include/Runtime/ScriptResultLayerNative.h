#pragma once

#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/PatternFilterNative.h>

namespace KaGuYa::Start::Runtime
{
class ScrContainer;

// 4CB222/4D3796：无顶层vptr，只有+14的真实CFilterParams；+04与padding默认不写。
struct ScriptResultLayerNativeBody34h final
{
    std::uint32_t valueAt00;
    std::uint8_t valueAt04;
    std::uint8_t unwrittenAt05[3];
    std::int32_t valueAt08;
    std::int32_t valueAt0C;
    std::uint8_t valueAt10;
    std::uint8_t unwrittenAt11[3];
    union { PatternFilterNativeBody20h filterAt14; };
    ScriptResultLayerNativeBody34h() noexcept {}
    ~ScriptResultLayerNativeBody34h() noexcept {}
    ScriptResultLayerNativeBody34h(const ScriptResultLayerNativeBody34h&) = delete;
    ScriptResultLayerNativeBody34h& operator=(const ScriptResultLayerNativeBody34h&) = delete;
};
static_assert(sizeof(ScriptResultLayerNativeBody34h) == 0x34);
static_assert(offsetof(ScriptResultLayerNativeBody34h, valueAt04) == 4);
static_assert(offsetof(ScriptResultLayerNativeBody34h, valueAt08) == 8);
static_assert(offsetof(ScriptResultLayerNativeBody34h, valueAt0C) == 0x0C);
static_assert(offsetof(ScriptResultLayerNativeBody34h, valueAt10) == 0x10);
static_assert(offsetof(ScriptResultLayerNativeBody34h, filterAt14) == 0x14);

ScriptResultLayerNativeBody34h* ConstructScriptResultLayerNativeAt4CB222(
    ScriptResultLayerNativeBody34h& body) noexcept;
ScriptResultLayerNativeBody34h* NewScriptResultLayerNativeAt4CB208();
// 4CB27C：首清不释放；refs/owning/stride先写控制块，再发布out.control，最后写count。
void ConstructScriptResultLayerReferenceAt4CB27C(YtlReferenceSlotRuntime& out,
    ScriptResultLayerNativeBody34h* completed);
// 4D7500的正文部分；直接销毁filter，不按filter当前虚表动态分派。
void DestroyScriptResultLayerNative(ScriptResultLayerNativeBody34h& body) noexcept;
ScriptResultLayerNativeBody34h* ResolveScriptResultLayerReferenceAt4CA1C0(
    const YtlReferenceSlotRuntime& slot) noexcept;
void ReleaseScriptResultLayerReferenceAt4D71D0(YtlReferenceSlotRuntime& slot);
void ReleaseScriptResultLayerReferenceAt4C93A0(YtlReferenceSlotRuntime& slot) noexcept;

// 真实2Ch输入：不调用现代Bytes/DTO，不限制layerCount/recordSize，不提前拒绝负entry。
// out首清不release，原展开只释放当前out；control分配失败不会额外回收裸34h。
YtlReferenceSlotRuntime* ReadScriptResultLayerNativeAt4D3670(const ScrContainer& container,
    YtlReferenceSlotRuntime& out, std::int32_t layerIndex, std::int32_t entryIndex);

// 与signed动画参数共用4447B0的同一模板核；不转换两种vector header。
std::uint32_t* InsertScriptResultLayerExtraAt4447B0(EnvironmentVectorStorage<std::uint32_t>& values,
    std::uint32_t* position, const std::uint32_t* value);
}
