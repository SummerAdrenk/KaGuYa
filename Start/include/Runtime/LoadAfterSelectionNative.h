#pragma once
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/TransitRuntime.h>
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct LoadAfterSelectionNativeBody58h;
struct LoadAfterSelectionNativeVtable4 final
{
    std::uint8_t (__thiscall* runAt00)(LoadAfterSelectionNativeBody58h*,std::int32_t);
    LoadAfterSelectionNativeBody58h* (__thiscall* deleteAt04)(LoadAfterSelectionNativeBody58h*,std::uint32_t);
    void (__thiscall* updateAt08)(LoadAfterSelectionNativeBody58h*);
    void (__thiscall* drawAt0C)(LoadAfterSelectionNativeBody58h*);
};
struct LoadAfterSelectionNativeBody58h final
{
    const LoadAfterSelectionNativeVtable4* vptrAt00;
    std::uint32_t phaseAt04;
    union{YtlReferenceSlotRuntime blackAt08;};
    union{YtlReferenceSlotRuntime backupAt14;};
    std::uint32_t savedModeAt20;
    union{TransitOperation operationAt24;};
    std::int32_t stepAt54;
    // 4AB47B自行写前缀；phase/保存值/step在原写点之前保持分配字节。
    LoadAfterSelectionNativeBody58h() noexcept{}
};
static_assert(sizeof(LoadAfterSelectionNativeVtable4)==16);
static_assert(sizeof(LoadAfterSelectionNativeBody58h)==0x58);
static_assert(offsetof(LoadAfterSelectionNativeBody58h,blackAt08)==8);
static_assert(offsetof(LoadAfterSelectionNativeBody58h,backupAt14)==0x14);
static_assert(offsetof(LoadAfterSelectionNativeBody58h,operationAt24)==0x24);
static_assert(offsetof(LoadAfterSelectionNativeBody58h,stepAt54)==0x54);
class ILoadAfterSelectionNativeSource
{
public:
    virtual ~ILoadAfterSelectionNativeSource()=default;
    virtual bool LoadBlackAt480000(YtlReferenceSlotRuntime&,const char*)=0;
    virtual std::uint32_t ReadModeAt04()=0;
    virtual void WriteModeAt04(std::uint32_t)=0;
    virtual void WriteRootByteAt218(std::uint8_t)=0;
    virtual void WriteRootByteAt219(std::uint8_t)=0;
    // 先slot1+70，再独立slot0/497900；不提前捕获第二个receiver。
    virtual std::int32_t ReadStepAt497900()=0;
    virtual bool ReadGateAt6F()=0;
    virtual bool AccelerateAt4AA970()=0;
    virtual void RenderAt4C39E0(TransitOperation&)=0;
    virtual void CommitAt4B02E0()=0;
    virtual void WriteRootSubobjectDwordAt10(std::uint32_t)=0;
    virtual void StopMusicAt4383B0()=0;
    virtual void ClearVoicesAt4384D0()=0;
    virtual void StopMappedEffectsAt413270()=0;
    virtual std::int32_t RestoreScriptAt4D4CC0()=0;
    virtual void WriteSceneMarkerAt38(std::int32_t)=0;
    virtual void RestorePatternsAt4C14E0()=0;
    virtual void RebuildTextAt4C1650(std::uint32_t)=0;
    virtual void RestorePrimaryGauge();
    virtual void PropagatePatternsAt4C4250()=0;
    virtual void DrawRegisteredAt4AC490()=0;
    // fresh返回槽；正文按465870复制，正常/EH释放临时的资格不同。
    virtual void CaptureScreenAt4ACDD0(YtlReferenceSlotRuntime&)=0;
    virtual void ClearScreenAt4AA890()=0;
    virtual void ClearCommandAt437680()=0;
    virtual void ClearScenesAt4C1460()=0;
    virtual void ClearScriptFlagsAt4B6766()=0;
    virtual void PushActiveAt4A9030(const char*)=0;
    virtual void ResetAndQueueTitleAt47E180()=0;
    virtual void QueueAt47F920(const char*)=0;
};
struct LoadAfterSelectionNativeServices final
{
    ILoadAfterSelectionNativeSource& source;
    IGraphicsSurfaceApi& surfaces;
    IEnvironmentRootMemory& memory;
};
class LoadAfterSelectionNativeScope final
{
public:
    explicit LoadAfterSelectionNativeScope(LoadAfterSelectionNativeServices&) noexcept;
    ~LoadAfterSelectionNativeScope() noexcept;
    LoadAfterSelectionNativeScope(const LoadAfterSelectionNativeScope&)=delete;
    LoadAfterSelectionNativeScope& operator=(const LoadAfterSelectionNativeScope&)=delete;
private:
    LoadAfterSelectionNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
};
const LoadAfterSelectionNativeVtable4& LoadAfterSelectionNativeMethods() noexcept;
LoadAfterSelectionNativeBody58h* ConstructLoadAfterSelectionNativeAt4AB47B(
    LoadAfterSelectionNativeBody58h&,LoadAfterSelectionNativeServices&);
LoadAfterSelectionNativeBody58h* NewLoadAfterSelectionNativeAt4AB45F(LoadAfterSelectionNativeServices&);
void DestroyLoadAfterSelectionNativeAt4A6A40(LoadAfterSelectionNativeBody58h&) noexcept;
LoadAfterSelectionNativeBody58h* DeleteLoadAfterSelectionNativeAt4A6A40(
    LoadAfterSelectionNativeBody58h*,std::uint32_t) noexcept;
std::uint8_t RunLoadAfterSelectionNativeAt4C3630(
    LoadAfterSelectionNativeBody58h&,std::int32_t,LoadAfterSelectionNativeServices&);
void UpdateLoadAfterSelectionNativeAt4B6380(LoadAfterSelectionNativeBody58h&,LoadAfterSelectionNativeServices&);
}
