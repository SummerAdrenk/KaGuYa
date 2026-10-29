#pragma once
#include <Runtime/LikabilitySceneNativeBody.h>
#include <Runtime/LikabilitySceneNativeScope.h>

namespace KaGuYa::Start::Runtime
{
class ILikabilitySceneNativeSource
{
public:
    virtual ~ILikabilitySceneNativeSource()=default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t site)=0;
    virtual void EnsureRootAt413BE0(std::uint32_t site)=0;
    virtual void* RootAt6198C4(std::uint32_t site)=0;
    virtual YtlReferenceSlotRuntime& RootScenesSlotAt30(void* capturedRoot)=0;
    virtual void AdvanceScenesAt4C1760(void* capturedScenes)=0;
    virtual GuiDialogNativeBody5Ch* ConstructDialogAt4E0E30(void* raw94)=0;
};
struct LikabilitySceneNativeServices final
{
    ILikabilitySceneNativeSource& source;
    LikabilityDialogNativeServices& gui;
    IEnvironmentRootMemory& memory;
};
LikabilitySceneNativeServices& RequireLikabilitySceneNativeServices();
const LikabilitySceneNativeVtable4& LikabilitySceneNativeMethods()noexcept;
LikabilitySceneNativeBody20h* ConstructLikabilitySceneNativeAt49DFB0(LikabilitySceneNativeBody20h&,LikabilitySceneNativeServices&);
void DestroyLikabilitySceneNativeAt4A69B0(LikabilitySceneNativeBody20h&)noexcept;
LikabilitySceneNativeBody20h* DeleteLikabilitySceneNativeAt4A69B0(LikabilitySceneNativeBody20h*,std::uint32_t)noexcept;
void UpdateLikabilitySceneNativeAt4B6280(LikabilitySceneNativeBody20h&,LikabilitySceneNativeServices&);
}
