#include <Runtime/ParamsThumbnailCatalogRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>

#include <atlbase.h>
#include <cstdlib>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
const ParamsThumbnailRuntime* ResolveThumbnail(const YtlReferenceSlotRuntime& reference) noexcept
{
    // 477820..47783E：先测base，再读取control的stride；无control时stride为0。
    const auto base = reference.baseAt00;
    if (!base) return nullptr;
    const auto* const control = static_cast<const EnvironmentObjectReferenceControl*>(reference.controlAt08);
    const auto stride = control ? control->strideAt0C : 0u;
    return reinterpret_cast<const ParamsThumbnailRuntime*>(base + reference.biasAt04 * stride);
}

void ReleaseLookupControl(YtlReferenceControlRuntime* const control) noexcept
{
    // 两个调用点的state均指向541E55；payload之后重新取同一个control的虚表。
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
}
}

YtlReferenceSlotRuntime& FindParamsThumbnailReferenceAt477800(
    const ParamsThumbnailTable& thumbnails, YtlReferenceSlotRuntime& outRef,
    const char* const nameCp932)
{
    // 477808只取一次begin；477856每次比较当前end，不通过range-for缓存尾指针。
    for (auto* item = thumbnails.storage.begin; item != thumbnails.storage.end; ++item)
    {
        const auto* const thumbnail = ResolveThumbnail(*item);
        // 477840在正文解引用前检查搜索名；空表不会触发这个异常。
        if (!nameCp932) throw ATL::CAtlException(E_FAIL);
        // 原空正文在477845访问违规；现代边界明确终止，不作为“未命中”。
        if (!thumbnail) _invalid_parameter_noinfo_noreturn();
        if (_mbscmp(
                reinterpret_cast<const unsigned char*>(thumbnail->nameAt00.GetString()),
                reinterpret_cast<const unsigned char*>(nameCp932)) == 0)
        {
            // 47787C..477893是逐槽复制后增持，不先释放outRef，也不深复制正文。
            outRef.baseAt00 = item->baseAt00;
            outRef.biasAt04 = item->biasAt04;
            outRef.controlAt08 = item->controlAt08;
            if (outRef.controlAt08) ++outRef.controlAt08->referenceCountAt04;
            return outRef;
        }
    }
    outRef = {};
    return outRef;
}

const char* const* BorrowParamsThumbnailAliasAt477800(
    const ParamsGameSystem& gameSystem, const char* const nameCp932)
{
    YtlReferenceSlotRuntime reference;
    static_cast<void>(FindParamsThumbnailReferenceAt477800(gameSystem.thumbnails, reference, nameCp932));
    auto* const control = reference.controlAt08;
    const auto* const body = ResolveThumbnail(reference);
    const char* const* const result = body ? &body->aliasesAt04[0] : nullptr;
    // 此前只有不抛的地址运算；正常释放异常必须终止，不进入额外RAII减持。
    ReleaseLookupControl(control);
    return result;
}

const ParamsThumbnailRuntime* FindParamsThumbnailAt477800(
    const ParamsGameSystem& gameSystem, const char* const nameCp932)
{
    YtlReferenceSlotRuntime reference;
    static_cast<void>(FindParamsThumbnailReferenceAt477800(gameSystem.thumbnails, reference, nameCp932));
    auto* const control = reference.controlAt08;
    const auto* const result = ResolveThumbnail(reference);
    ReleaseLookupControl(control);
    return result;
}
}
