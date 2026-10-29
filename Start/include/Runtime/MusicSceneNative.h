#pragma once
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/InputRootNative.h>
#include <Runtime/TransitRuntime.h>
#include <Runtime/MusicButtonNative.h>
#include <array>
#include <span>
#include <optional>

namespace KaGuYa::Start::Runtime
{
struct MusicSceneNativeBody64h;
struct MusicSceneNativeVtable4 final
{
    std::uint8_t(__thiscall* runAt00)(MusicSceneNativeBody64h*,std::int32_t);
    MusicSceneNativeBody64h*(__thiscall* deleteAt04)(MusicSceneNativeBody64h*,std::uint32_t);
    void(__thiscall* updateAt08)(MusicSceneNativeBody64h*);
    void(__thiscall* drawAt0C)(MusicSceneNativeBody64h*);
};
// 4A0730：04/44/60仅在各原业务写点初始化；不能由DMI提前清零。
struct MusicSceneNativeBody64h final
{
    const MusicSceneNativeVtable4* vptrAt00;
    std::uint32_t phaseAt04;
    union{YtlReferenceSlotRuntime buttonAt08;};
    union{TransitOperation operationAt14;};
    std::int32_t stepAt44;
    union{YtlReferenceSlotRuntime blackAt48;};
    union{YtlReferenceSlotRuntime backupAt54;};
    std::int32_t routeAt60;
    MusicSceneNativeBody64h() noexcept{}
    ~MusicSceneNativeBody64h() noexcept{}
};
static_assert(sizeof(MusicSceneNativeBody64h)==0x64&&sizeof(MusicSceneNativeVtable4)==16);
static_assert(offsetof(MusicSceneNativeBody64h,buttonAt08)==8);
static_assert(offsetof(MusicSceneNativeBody64h,operationAt14)==0x14);
static_assert(offsetof(MusicSceneNativeBody64h,stepAt44)==0x44);
static_assert(offsetof(MusicSceneNativeBody64h,blackAt48)==0x48);
static_assert(offsetof(MusicSceneNativeBody64h,backupAt54)==0x54);
static_assert(offsetof(MusicSceneNativeBody64h,routeAt60)==0x60);

class IMusicSceneNativeSource
{
public:
    virtual ~IMusicSceneNativeSource()=default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t site)=0;
    virtual InputRootNativeBody24h* CaptureInputAt418210(std::uint32_t site)=0;
    virtual void EnsureRootAt413BE0(std::uint32_t site)=0;
    virtual void* RootAt6198C4(std::uint32_t site)=0;
    virtual YtlReferenceSlotRuntime& RootDrawSlotAt18(void*)=0;
    virtual YtlReferenceSlotRuntime& RootSoundSlotAt24(void*)=0;
    virtual YtlReferenceSlotRuntime& RootScenesSlotAt30(void*)=0;
    virtual void WriteRootByte(void*,std::uint32_t offset,std::uint8_t)=0;
    virtual bool LoadSurfaceAt480000(void* draw,YtlReferenceSlotRuntime&,const char*)=0;
    virtual YtlReferenceSlotRuntime* CaptureScreenAt4ACDD0(void* draw,YtlReferenceSlotRuntime*)=0;
    virtual void ConfigureScenesAt4703D0(void* scenes,std::int32_t)=0;
    // 原4182F0 + 紧接的图形叶是一个非拥有调用边界，各次独立Capture。
    virtual void ClearScreenAfter4182F0(std::uint32_t site,const RECT*)=0;
    virtual void ClearDirtyAfter4182F0(std::uint32_t site)=0;
    virtual void InvalidateAfter4182F0(std::uint32_t site,const RECT*)=0;
    virtual void StopSoundAt4383B0(void* sound,std::uint32_t slot)=0;
    virtual bool AccelerateAt4AA970(void* archive5)=0;
    virtual void RenderAt449DD0(void* transit,TransitOperation&)=0;
    virtual void PushSceneAt4A9030(void* scenes,const char*)=0;
    virtual void QueueSceneAt47F920(void* scenes,const char*)=0;
    virtual void SynchronizeButtonAt4ABB40(void* button)=0;
    virtual void* ButtonItemAt4ACF20(void* button,std::int32_t)=0;
};
struct MusicSceneNativeServices final
{
    IMusicSceneNativeSource& source;
    IMusicSceneNativeButtonConstruction& buttons;
    IEnvironmentRootMemory& memory;
    IGraphicsSurfaceApi& surfaces;
};
class MusicSceneNativeScope final
{
public:
    explicit MusicSceneNativeScope(MusicSceneNativeServices&) noexcept;
    ~MusicSceneNativeScope() noexcept;
    MusicSceneNativeScope(const MusicSceneNativeScope&)=delete;
    MusicSceneNativeScope& operator=(const MusicSceneNativeScope&)=delete;
private:
    MusicSceneNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    std::optional<MusicButtonNativeScope> button_;
};
MusicSceneNativeServices& RequireMusicSceneNativeServices();
const MusicSceneNativeVtable4& MusicSceneNativeMethods() noexcept;
MusicSceneNativeBody64h* ConstructMusicSceneNativeAt4A0730(MusicSceneNativeBody64h&,MusicSceneNativeServices&);
void UpdateMusicSceneNativeAt4B8C90(MusicSceneNativeBody64h&,MusicSceneNativeServices&);
void DestroyMusicSceneNativeAt4A6DA0(MusicSceneNativeBody64h&) noexcept;
MusicSceneNativeBody64h* DeleteMusicSceneNativeAt4A6DA0(MusicSceneNativeBody64h*,std::uint32_t) noexcept;
struct MusicSceneNativeEhEvidence final
{
    std::uint32_t functionVA,funcInfoVA;
    std::array<std::uint32_t,9> funcInfo;
    std::span<const std::uint32_t> unwind;
};
std::span<const std::uint32_t,4> MusicSceneNativeOriginalTableSample18() noexcept;
std::span<const MusicSceneNativeEhEvidence,3> MusicSceneNativeEhSample18() noexcept;
}
