#pragma once

#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
class ParamsRuntimeRootOwner;

// CScript+0C/18实际持有的正文；不能值初始化或附加optional/分配器字段。
struct ScriptWaitNative final
{
    std::int32_t valueAt00, valueAt04, valueAt08;
};
struct ScriptUpdatePairNative final
{
    std::int32_t first, second;
    bool operator==(const ScriptUpdatePairNative&) const = default;
};
struct ScriptEffectShellTag final {};
class ScriptEffectNative final
{
public:
    explicit ScriptEffectNative(ScriptEffectShellTag) noexcept {}
    ~ScriptEffectNative() noexcept;
    ScriptEffectNative(const ScriptEffectNative&) = delete;
    ScriptEffectNative& operator=(const ScriptEffectNative&) = delete;

    std::uint16_t status;
    std::uint8_t modeFlags, auxiliaryByte;
    union { RuntimeCString resourceNameAt04; };
    union { YtlReferenceSlotRuntime pairsAt08; };

    [[nodiscard]] std::size_t PairCount() const noexcept;
    [[nodiscard]] ScriptUpdatePairNative& PairAt(std::size_t index);
    [[nodiscard]] const ScriptUpdatePairNative& PairAt(std::size_t index) const;
    [[nodiscard]] const YtlReferenceSlotRuntime& PairsReference() const noexcept { return pairsAt08; }
    [[nodiscard]] std::string_view ResourceName() const noexcept { return RuntimeStringView(resourceNameAt04); }
    void SetResourceName(const char* name);
    void Reset();
};
static_assert(sizeof(ScriptWaitNative) == 0x0C);
static_assert(sizeof(ScriptUpdatePairNative) == 8);
static_assert(sizeof(ScriptEffectNative) == 0x14);
static_assert(offsetof(ScriptEffectNative, resourceNameAt04) == 4);
static_assert(offsetof(ScriptEffectNative, pairsAt08) == 8);

// 原构造中可定位的连续片段。只改变传入的原Ref；调用者承担CScript外层EH。
// memory的正文/control分配必须与全局operator delete兼容，不在24-byte control内藏现代allocator。
void CreateScriptEffectAt4C8ECC(YtlReferenceSlotRuntime& effect,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void CreateScriptEffectPairsAt4C9025(ScriptEffectNative& effect, std::int32_t count,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void CreateScriptWaitAt4C917D(YtlReferenceSlotRuntime& wait,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void ResetScriptEffectAt4CD180(ScriptEffectNative& effect);
// 4C8FA1起取GS count，创建pairs、memset count字节、Reset，最后创建wait。
// 与effect创建分开，保留此前parser/handler登记及外层失败清理的装配边界。
void InitializeScriptEffectAndWaitAt4C8FA1(YtlReferenceSlotRuntime& effect,
    YtlReferenceSlotRuntime& wait, ParamsRuntimeRootOwner& params,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
[[nodiscard]] std::int32_t ScriptPairCount(const ScriptEffectNative& effect) noexcept;
[[nodiscard]] ScriptUpdatePairNative* ResolveScriptPair(
    const ScriptEffectNative& effect, std::uint32_t index) noexcept;
}
