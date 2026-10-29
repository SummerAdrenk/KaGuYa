#pragma once
#include <Runtime/CgSceneNativeBody.h>
#include <Runtime/CgButtonNative.h>
#include <Runtime/CgDisplayWindowNative.h>
#include <Runtime/CgOperationWindowNative.h>
#include <Runtime/CgDisplayResourceNative.h>
#include <Runtime/CgSceneNativeScope.h>

namespace KaGuYa::Start::Runtime
{
class ICgSceneNativeSource : public ICgDisplayResourceNativeSource,public ISecretCgParamsRootSource
{
public:
    const void* CaptureAt418480(std::uint32_t site) final{return CaptureParamsAt418480(site);}
    virtual InputRootNativeBody24h* CaptureInputAt418210(std::uint32_t site)=0;
    virtual void EnsureRootAt413BE0(std::uint32_t callSite)=0;
    virtual void* RootAt6198C4(std::uint32_t readSite)=0;
    virtual YtlReferenceSlotRuntime& RootDrawSlotAt18(void*)=0;
    virtual YtlReferenceSlotRuntime& RootSoundSlotAt24(void*)=0;
    virtual YtlReferenceSlotRuntime& RootScenesSlotAt30(void*)=0;
    virtual void WriteRootByte(void*,std::uint32_t offset,std::uint8_t)=0;
    virtual bool LoadSurfaceAt480000(void* draw,YtlReferenceSlotRuntime&,const char*)=0;
    virtual YtlReferenceSlotRuntime* CaptureScreenAt4ACDD0(void* draw,YtlReferenceSlotRuntime*)=0;
    virtual void ConfigureScenesAt4703D0(void* scenes,std::int32_t)=0;
    virtual SIZE ReadScreenSizeAfter4182F0(std::uint32_t site)=0;
    virtual void ClearDirtyAfter4182F0(std::uint32_t site)=0;
    virtual void InvalidateAfter4182F0(std::uint32_t site,const RECT*)=0;
    virtual bool AccelerateAt4AA970(void* archive5)=0;
    virtual void RenderAt449DD0(void* transit,TransitOperation&)=0;
    virtual void PushSceneAt4A9030(void* scenes,const char*)=0;
    virtual void QueueSceneAt47F920(void* scenes,const char*)=0;
    virtual void QueueTitleAfterConstructorAt47E180(void* scenes)=0;
    virtual void PlayMusicAt4BCCC0(void* sound,RuntimeCString& mapping,const char*,std::int32_t value,std::int32_t loop)=0;
    virtual void PlayCueAt4BCD70(void* sound,const char*,std::int32_t slot,std::int32_t value,std::uint8_t)=0;
};
// 均为正文外借用；Root宿主须令buttons/operation/resource指向同一实际对象图。
struct CgSceneNativeServices final
{
    ICgSceneNativeSource& source;
    CgButtonNativeServices& buttons;
    CgDisplayWindowNativeServices& display;
    CgOperationWindowNativeServices& operation;
    CgDisplayResourceNativeServices& resources;
    IEnvironmentRootMemory& memory;
    IGraphicsSurfaceApi& surfaces;
};
CgSceneNativeServices& RequireCgSceneNativeServices();
const CgSceneNativeVtable4& CgSceneNativeMethods() noexcept;
CgSceneNativeBodyB4h* ConstructCgSceneNativeAt49E3B0(CgSceneNativeBodyB4h&,CgSceneNativeServices&);
void DestroyCgSceneNativeAt4A4720(CgSceneNativeBodyB4h&) noexcept;
CgSceneNativeBodyB4h* DeleteCgSceneNativeAt4A6C30(CgSceneNativeBodyB4h*,std::uint32_t) noexcept;
void UpdateCgSceneNativeAt4B6A60(CgSceneNativeBodyB4h&,CgSceneNativeServices&);
void DrawCgSceneNativeAt4B5970(CgSceneNativeBodyB4h&,CgSceneNativeServices&);
}
