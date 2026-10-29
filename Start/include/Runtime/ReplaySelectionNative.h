#pragma once
#include <Runtime/ParamsSceneLabelNative.h>

namespace KaGuYa::Start::Runtime
{
class ParamsRuntimeRoot;
struct ReplaySelectionNativeBody8h final
{
    std::int32_t fileIndexAt00;
    std::int32_t entryAt04;
};
static_assert(sizeof(ReplaySelectionNativeBody8h)==8);
class IReplaySelectionParamsNativeSource
{
public:
    virtual ~IReplaySelectionParamsNativeSource()=default;
    virtual const ParamsRuntimeRoot* CaptureParamsAt418480(std::uint32_t callSite)=0;
};
// 两入口均构造fresh返回Ref12；不增持标签、不返回shared_ptr或现代值快照。
YtlReferenceSlotRuntime* FindReplayLabelNativeAt4AD300(YtlReferenceSlotRuntime& fresh,
    const ParamsSceneLabelNative&,const char* name,
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory());
YtlReferenceSlotRuntime* FindReplaySelectionNativeAt4ADAE0(YtlReferenceSlotRuntime& fresh,
    IReplaySelectionParamsNativeSource&,std::int32_t category,std::int32_t item,std::int32_t segment,
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory());
// 原4A2F90 EHFlags5；control为空不清base，正常清槽不改bias。
void ReleaseReplaySelectionNativeAt4A2F90(YtlReferenceSlotRuntime&) noexcept;
}
