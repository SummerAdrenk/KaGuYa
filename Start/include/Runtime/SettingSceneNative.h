#pragma once

#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/GuiDialogNative.h>
#include <Runtime/SettingDialogNativeRuntime.h>
#include <optional>
#include <array>
#include <span>

namespace KaGuYa::Start::Runtime
{
struct SettingSceneNativeBody20h;
// PE 5B57FC..5B5808 只有四槽；5B580C 是下一类 RTTI，不是 Finish。
struct SettingSceneNativeVtable4 final
{
    std::uint8_t (__thiscall* runAt00)(SettingSceneNativeBody20h*, std::int32_t);
    SettingSceneNativeBody20h* (__thiscall* deleteAt04)(SettingSceneNativeBody20h*, std::uint32_t);
    void (__thiscall* updateAt08)(SettingSceneNativeBody20h*);
    void (__thiscall* drawAt0C)(SettingSceneNativeBody20h*);
};
struct SettingSceneNativeBody20h final
{
    const SettingSceneNativeVtable4* vptrAt00;
    std::uint32_t phaseAt04;
    std::uint32_t savedRegistryAt08;
    std::uint32_t savedFlagsAt0C;
    std::uint32_t unwrittenAt10;
    union { YtlReferenceSlotRuntime dialogAt14; };
    SettingSceneNativeBody20h() noexcept {}
    ~SettingSceneNativeBody20h() noexcept = default;
};
static_assert(sizeof(SettingSceneNativeVtable4) == 0x10);
static_assert(sizeof(SettingSceneNativeBody20h) == 0x20);
static_assert(alignof(SettingSceneNativeBody20h) == 4);
static_assert(std::is_standard_layout_v<SettingSceneNativeBody20h>);
static_assert(std::is_trivially_copyable_v<SettingSceneNativeBody20h>);
static_assert(std::is_trivially_destructible_v<SettingSceneNativeBody20h>);
static_assert(offsetof(SettingSceneNativeBody20h, phaseAt04) == 4);
static_assert(offsetof(SettingSceneNativeBody20h, savedRegistryAt08) == 8);
static_assert(offsetof(SettingSceneNativeBody20h, savedFlagsAt0C) == 0x0C);
static_assert(offsetof(SettingSceneNativeBody20h, unwrittenAt10) == 0x10);
static_assert(offsetof(SettingSceneNativeBody20h, dialogAt14) == 0x14);
static_assert(offsetof(SettingSceneNativeVtable4, drawAt0C) == 0x0C);

class ISettingSceneNativeSource : public ISettingDialogNativeConstruction
{
public:
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0() = 0;
    virtual void EnsureRootAt413BE0() = 0;
    virtual void* RootAt6198C4() = 0;
    // 仅借固定根的原 Ref；不得再次 Ensure/选根或增持。
    virtual YtlReferenceSlotRuntime& RootSceneManagerSlotAt30(void* fixedRoot) = 0;
    virtual void PopThenActivatePendingAt4C1760(void* nativeManager48h) = 0;
};
struct SettingSceneNativeServices final
{
    ISettingSceneNativeSource& source;
    IEnvironmentRootMemory& memory;
};
// TLS 仅借服务，不存 scene/dialog 身份或业务镜像。原 GUI 自己的长寿 scope 仍是必需依赖。
class SettingSceneNativeScope final
{
public:
    explicit SettingSceneNativeScope(SettingSceneNativeServices&) noexcept;
    ~SettingSceneNativeScope() noexcept;
    SettingSceneNativeScope(const SettingSceneNativeScope&) = delete;
    SettingSceneNativeScope& operator=(const SettingSceneNativeScope&) = delete;
private:
    SettingSceneNativeServices* previous_;
    std::optional<SettingDialogNativeRuntimeScope> dialog_;
};
SettingSceneNativeServices& RequireSettingSceneNativeServices();
const SettingSceneNativeVtable4& SettingSceneNativeMethods() noexcept;
SettingSceneNativeBody20h* ConstructSettingSceneNativeAt4A1A00(
    SettingSceneNativeBody20h&, SettingSceneNativeServices&);
SettingSceneNativeBody20h* NewSettingSceneNativeAt4AB649(SettingSceneNativeServices&);
void DestroySettingSceneNativeAt4A7020(SettingSceneNativeBody20h&) noexcept;
SettingSceneNativeBody20h* DeleteSettingSceneNativeAt4A7020(
    SettingSceneNativeBody20h*, std::uint32_t flags) noexcept;
std::uint8_t RunSettingSceneNativeAt4C3630(
    SettingSceneNativeBody20h&, std::int32_t argument, SettingSceneNativeServices&);
void UpdateSettingSceneNativeAt4BA800(SettingSceneNativeBody20h&, SettingSceneNativeServices&);

// 原 VA 只作审计数据，不映射样本、不转换成可执行地址。
std::span<const std::uint32_t, 4> SettingSceneNativeOriginalVtableSample18() noexcept;
std::span<const std::uint32_t, 37> SettingSceneNativeOriginalDialogVtableSample18() noexcept;
std::span<const std::array<std::uint32_t, 2>, 3> SettingSceneNativeConstructorUnwindSample18() noexcept;
}
