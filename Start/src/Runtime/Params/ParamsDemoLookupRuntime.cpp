#include <Runtime/ParamsDemoLookupRuntime.h>
#include <Runtime/ParamsDemoStorage.h>
#include <Runtime/ParamsRuntimeRoot.h>

#include <bit>
#include <cstdlib>
#include <mbstring.h>
#include <atlexcept.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
const ParamsDemoEntryRuntime* ResolveComparedEntry(const YtlReferenceSlotRuntime& array,
    const EnvironmentObjectReferenceControl* control, std::uint32_t index) noexcept
{
    const auto base = array.baseAt00;
    if (!base) return nullptr;
    const auto stride = control ? control->strideAt0C : 0u;
    const std::uint32_t offset = (array.biasAt04 + index) * stride;
    return reinterpret_cast<const ParamsDemoEntryRuntime*>(base + offset);
}
}

const ParamsGameSystem* ParamsDemoRootSource::CaptureGameSystemAt418480()
{
    return owner_.GetOrCreateAt418480().GameSystem().get();
}

void FindParamsDemoAt4ACC40(const ParamsGameSystem* gameSystem,
    const char* searchName, YtlReferenceSlotRuntime& out)
{
    // 原入口首次访问 GS+C0；显式硬失败代替空 receiver 的非法内存访问，不伪造空表。
    if (!gameSystem) _invalid_parameter_noinfo_noreturn();
    const auto& array = gameSystem->demos.storage;
    for (std::uint32_t index = 0;; ++index)
    {
        auto* const control = static_cast<const EnvironmentObjectReferenceControl*>(array.controlAt08);
        const auto count = control ? control->countAt10 : 0u;
        if (std::bit_cast<std::int32_t>(index) >= std::bit_cast<std::int32_t>(count)) break;
        const auto* entry = ResolveComparedEntry(array, control, index);
        // 4ACC8F 先拒绝空 searchName，然后才访问可能为空的元素正文。
        if (!searchName) throw ATL::CAtlException(E_FAIL);
        if (!entry) _invalid_parameter_noinfo_noreturn();
        if (_mbscmp(reinterpret_cast<const unsigned char*>(entry->nameAt00.GetString()),
            reinterpret_cast<const unsigned char*>(searchName)) != 0) continue;

        // 4ACCA9 命中后重取整个外 Ref；不能沿用比较前的 entry 或预拷贝内 Ref。
        entry = ResolveParamsDemoEntry(array, index);
        if (!entry) _invalid_parameter_noinfo_noreturn();
        out.baseAt00 = entry->dataAt04.baseAt00;
        const auto bias = entry->dataAt04.biasAt04;
        auto* const foundControl = entry->dataAt04.controlAt08;
        out.biasAt04 = bias;
        out.controlAt08 = foundControl;
        if (foundControl) ++foundControl->referenceCountAt04;
        return;
    }
    out.baseAt00 = 0;
    out.biasAt04 = 0;
    out.controlAt08 = nullptr;
}

void ReleaseParamsDemoLookupAt4A32A0(YtlReferenceSlotRuntime& reference) noexcept
{
    ReleaseParamsDemoDataAt43B7D0(reference);
}
}
