#pragma once
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/GuiDialogNative.h>
#include <Runtime/LogDialogNativeConstruction.h>
#include <Runtime/LogDialogNative.h>
#include <array>
#include <span>
#include <optional>

namespace KaGuYa::Start::Runtime
{
struct LogSceneNativeBody20h;
struct LogSceneNativeVtable4 final
{
    std::uint8_t(__thiscall* runAt00)(LogSceneNativeBody20h*,std::int32_t);
    LogSceneNativeBody20h*(__thiscall* deleteAt04)(LogSceneNativeBody20h*,std::uint32_t);
    void(__thiscall* updateAt08)(LogSceneNativeBody20h*);
    void(__thiscall* drawAt0C)(LogSceneNativeBody20h*);
};
struct LogSceneNativeBody20h final
{
    const LogSceneNativeVtable4* vptrAt00;
    std::uint32_t phaseAt04;
    std::uint32_t savedRegistryAt08;
    std::uint32_t unwrittenAt0C;
    union{YtlReferenceSlotRuntime dialogAt10;};
    std::uint8_t savedRootByteAt1C;
    std::uint8_t savedRootByteAt1D;
    std::uint8_t unwrittenAt1E[2];
    LogSceneNativeBody20h() noexcept{}
    ~LogSceneNativeBody20h() noexcept=default;
};
static_assert(sizeof(LogSceneNativeBody20h)==0x20&&sizeof(LogSceneNativeVtable4)==0x10);
static_assert(std::is_standard_layout_v<LogSceneNativeBody20h>);
static_assert(offsetof(LogSceneNativeBody20h,dialogAt10)==0x10&&offsetof(LogSceneNativeBody20h,savedRootByteAt1C)==0x1C);
static_assert(offsetof(LogSceneNativeBody20h,unwrittenAt0C)==0x0C&&offsetof(LogSceneNativeBody20h,unwrittenAt1E)==0x1E);
class ILogSceneNativeSource:public ILogDialogNativeConstruction
{
public:
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t site)=0;
    virtual void EnsureRootAt413BE0(std::uint32_t site)=0;
    virtual void* RootAt6198C4(std::uint32_t site)=0;
    virtual std::uint8_t RootByteAt218(void* fixedRoot)=0;
    virtual std::uint8_t RootByteAt219(void* fixedRoot)=0;
    virtual void WriteRootByteAt218(void* fixedRoot,std::uint8_t)=0;
    virtual void WriteRootByteAt219(void* fixedRoot,std::uint8_t)=0;
    virtual YtlReferenceSlotRuntime& RootSceneManagerSlotAt30(void* fixedRoot)=0;
    virtual void PopThenActivatePendingAt4C1760(void* fixedManager)=0;
};
struct LogSceneNativeServices final{ILogSceneNativeSource& source;IEnvironmentRootMemory& memory;};
class LogSceneNativeScope final
{
public:
    explicit LogSceneNativeScope(LogSceneNativeServices&) noexcept;
    ~LogSceneNativeScope() noexcept;
    LogSceneNativeScope(const LogSceneNativeScope&)=delete;
    LogSceneNativeScope& operator=(const LogSceneNativeScope&)=delete;
private:
    LogSceneNativeServices* previous_;
    std::optional<LogDialogNativeScope> dialog_;
};
LogSceneNativeServices& RequireLogSceneNativeServices();
const LogSceneNativeVtable4& LogSceneNativeMethods() noexcept;
LogSceneNativeBody20h* ConstructLogSceneNativeAt49E180(LogSceneNativeBody20h&,LogSceneNativeServices&);
void DestroyLogSceneNativeAt4A6BA0(LogSceneNativeBody20h&) noexcept;
LogSceneNativeBody20h* DeleteLogSceneNativeAt4A6BA0(LogSceneNativeBody20h*,std::uint32_t) noexcept;
void UpdateLogSceneNativeAt4B6900(LogSceneNativeBody20h&,LogSceneNativeServices&);
std::uint8_t RunLogSceneNativeAt4C3630(LogSceneNativeBody20h&,std::int32_t,LogSceneNativeServices&);
struct LogSceneNativeEhEvidence final
{
    std::uint32_t functionVA,funcInfoVA;
    std::span<const std::uint32_t,9> funcInfo;
    std::span<const std::uint32_t> unwind;
};
std::span<const std::uint32_t,4> LogSceneNativeOriginalTableSample18() noexcept;
std::span<const std::uint32_t,2> LogSceneNativeControlOriginalTableSample18() noexcept;
std::span<const LogSceneNativeEhEvidence,3> LogSceneNativeEhSample18() noexcept;
}
