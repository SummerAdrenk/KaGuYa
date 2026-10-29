#pragma once
#include <Runtime/ModeButtonNative.h>
#include <Runtime/MusicDialogNative.h>
#include <Runtime/MusicSceneNativeButtonConstruction.h>
#include <new>
#include <span>

namespace KaGuYa::Start::Runtime
{
struct MusicButtonNativeBody70h final
{
    union{ModeButtonNativeBody50h common;};
    union{YtlReferenceSlotRuntime dialogAt50;};
    union{YtlReferenceSlotRuntime reservedAt5C;};
    std::int32_t soundSelectionAt68;
    std::uint8_t movieAt6C;
    std::uint8_t unwrittenAt6D[3];
    MusicButtonNativeBody70h() noexcept{}
    ~MusicButtonNativeBody70h() noexcept{}
};
static_assert(sizeof(MusicButtonNativeBody70h)==0x70&&offsetof(MusicButtonNativeBody70h,dialogAt50)==0x50);
static_assert(offsetof(MusicButtonNativeBody70h,soundSelectionAt68)==0x68&&offsetof(MusicButtonNativeBody70h,movieAt6C)==0x6C);
class IMusicButtonNativeSource
{
public:
    virtual ~IMusicButtonNativeSource()=default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t site)=0;
    virtual void EnsureRootAt413BE0(std::uint32_t site)=0;
    virtual void* RootAt6198C4(std::uint32_t site)=0;
    virtual YtlReferenceSlotRuntime& RootScenesSlotAt30(void*)=0;
    virtual YtlReferenceSlotRuntime& RootSoundSlotAt24(void*)=0;
    virtual const YtlReferenceSlotRuntime* PublicGuiAt47B7B0(void* scenes)=0;
    virtual const char* ExitNameAt61A56C()=0;
    virtual void StopSoundAt4383B0(void* sound,std::uint32_t slot)=0;
    virtual void PlayMusicAt4BCCC0(void* sound,RuntimeCString& mapping,const char* name,std::int32_t value,std::int32_t loop)=0;
    virtual void PlayCueAt4BCD70(void* sound,const char* name,std::int32_t slot,std::int32_t value,std::uint8_t checkType)=0;
    virtual void DrawSurfaceAt441980(GraphicsSurfaceNativeBody*,std::int32_t x,std::int32_t y,const RECT*)=0;
    // 4ABB40的inline图形根捕获/提交，不再捕获Root或改变已有dirty状态。
    virtual void InvalidateRectAfter4182F0(const RECT*)=0;
};
struct MusicButtonNativeServices final
{
    ModeButtonNativeServices& mode;
    IMusicDialogNativeConstruction& dialogs;
    IMusicButtonNativeSource& source;
    IRuntimeStringManagerSource& strings;
};
class MusicButtonNativeScope final
{
public:
    explicit MusicButtonNativeScope(MusicButtonNativeServices&) noexcept;
    ~MusicButtonNativeScope() noexcept;
    MusicButtonNativeScope(const MusicButtonNativeScope&)=delete;
    MusicButtonNativeScope& operator=(const MusicButtonNativeScope&)=delete;
private:
    MusicButtonNativeServices* previous_;
    ModeButtonNativeScope mode_;
    MusicDialogNativeScope dialog_;
};
MusicButtonNativeServices& RequireMusicButtonNativeServices();
MusicButtonNativeBody70h* ConstructMusicButtonNativeAt4A1C70(MusicButtonNativeBody70h&,MusicButtonNativeServices&);
MusicButtonNativeBody70h* DeleteMusicButtonNativeAt4A70B0(MusicButtonNativeBody70h*,std::uint32_t) noexcept;
std::int32_t ProcessMusicButtonNativeAt4BE610(MusicButtonNativeBody70h&);
// 原调用方不消费4ABB40的EAX；不伪造一个统一“成功返回码”。
void SynchronizeMusicButtonNativeAt4ABB40(MusicButtonNativeBody70h&);
void SelectMusicButtonSoundNativeAt4C1A20(MusicButtonNativeBody70h&,std::int32_t);
const ModeButtonNativeVtable13& MusicButtonNativeVtable() noexcept;
std::span<const char* const,18> MusicButtonNativeSongsSample18() noexcept;
class MusicButtonNativeConstruction final:public IMusicSceneNativeButtonConstruction
{
public:
    explicit MusicButtonNativeConstruction(MusicButtonNativeServices& s) noexcept:services_(s){}
    void* ConstructAt4A1C70(void* raw) override{return ConstructMusicButtonNativeAt4A1C70(*::new(raw) MusicButtonNativeBody70h,services_);}
    MusicButtonNativeServices& Services() const noexcept{return services_;}
    MusicButtonNativeServices* NativeButtonServices() const noexcept override{return &services_;}
private:MusicButtonNativeServices& services_;
};
}
