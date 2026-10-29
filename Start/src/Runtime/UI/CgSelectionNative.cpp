#include <Runtime/CgSelectionNative.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <bit>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T* Indexed(T* base,std::uint32_t index,std::uint32_t stride) noexcept
{ return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(base)+index*stride); }
std::int32_t Span(const void* begin,const void* end,unsigned stride) noexcept
{
    const auto bytes=std::uint32_t(reinterpret_cast<std::uintptr_t>(end)-reinterpret_cast<std::uintptr_t>(begin));
    return std::bit_cast<std::int32_t>(bytes)/std::int32_t(stride);
}
const ParamsRegistCgItemRuntime* CaptureItem(ISecretCgParamsRootSource& source,
    std::uint32_t site,std::uint32_t category,std::uint32_t item)
{
    const auto* root=static_cast<const ParamsRuntimeRoot*>(source.CaptureAt418480(site));
    const auto* system=root->GameSystem().get();
    const auto* group=ResolveEnvironmentObjectReference<ParamsRegistCgGroupRuntime>(
        *Indexed(system->registCg.storage.begin,category,12));
    return ResolveEnvironmentObjectReference<ParamsRegistCgItemRuntime>(
        *Indexed(group->itemsAt04.storage.begin,item,12));
}
}
bool ReadCgSelectionNativeAt4AD780(const SecretCgNativeArchive& secret,
    ISecretCgParamsRootSource& params,std::int32_t category,std::uint32_t item,
    std::int32_t* count,POINT* position,std::int32_t* value)
{
    // 4AD783..795是SAR4；负category并无前置拒绝，不套现代容器的at()。
    const auto bytes=std::uint32_t(reinterpret_cast<std::uintptr_t>(secret.mappingsAt18.end)-
        reinterpret_cast<std::uintptr_t>(secret.mappingsAt18.begin));
    if(category >= (std::bit_cast<std::int32_t>(bytes)>>4))return false;
    const auto* mapping=Indexed(secret.mappingsAt18.begin,std::uint32_t(category),16);
    const auto& items=mapping->referencesAt04;
    if(item>=std::uint32_t(Span(items.begin,items.end,12)))return false;
    const auto* selected=Indexed(items.begin,item,12);
    const auto* control=static_cast<const EnvironmentObjectReferenceControl*>(selected->controlAt08);
    *count=control?std::bit_cast<std::int32_t>(control->countAt10):0;
    // count写入早于首个捕获；第二次失败不能回滚已经写入的count/坐标。
    const auto* first=CaptureItem(params,0x4AD7DB,std::uint32_t(category),item);
    position->x=std::bit_cast<LONG>(first->xAt04);
    position->y=std::bit_cast<LONG>(first->yAt08);
    const auto* second=CaptureItem(params,0x4AD861,std::uint32_t(category),item);
    *value=std::bit_cast<std::int32_t>(second->valueAt0C);
    return true;
}
void ResetCgSelectionNativeAt4B7433(CgSelectionNativeBody20h& body) noexcept
{
    body.countAt00=0;body.valueAt04=0;body.positionAt08.y=0;body.positionAt08.x=0;
    body.selectedPairAt10=0;body.pairsAt14.end=body.pairsAt14.begin;
}
}
