#include <Runtime/ParamsResourceNameRuntime.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/ParamsPatternNativeAccess.h>

#include <atlbase.h>
#include <bit>
#include <mbstring.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
ParamsResourceNameRuntime::ParamsResourceNameRuntime(ParamsRuntimeRootOwner& params) noexcept
    : params_(params) {}

const ParamsPatternNative* ParamsResourceNameRuntime::CapturePattern() const
{
    return params_.GetOrCreateAt418480().Pattern().get();
}

ScrResourceNameReceiver ParamsResourceNameRuntime::CapturePatternNames() const
{
    // 4D5A7B等调用点只取正文指针，不增持原Ref延长旧Pattern寿命。
    return CaptureParamsForNames().CapturePatternNames();
}

ScrResourceNameRootReceiver ParamsResourceNameRuntime::CaptureParamsForNames() const
{
    return {&params_.GetOrCreateAt418480(), &CaptureRootPattern};
}

ScrResourceNameReceiver ParamsResourceNameRuntime::CaptureRootPattern(const void* receiver)
{
    const auto* const root = static_cast<const ParamsRuntimeRoot*>(receiver);
    return {root->Pattern().get(), &FindName};
}

std::int32_t ParamsResourceNameRuntime::FindResourceIdAt47A020(const char* const nameCp932) const
{
    // 4D54C1..54EC/4D55E4..560D：调用时取根，循环只借同一Pattern，不AddRef。
    const auto* const pattern = CapturePattern();
    if (!pattern) throw std::logic_error("Resource reverse lookup Pattern body is null");
    for (std::size_t index = 0; index < ParamsPatternVectorSize(pattern->itemsAt04); ++index)
    {
        // 47A026..4D先判断数量；47A077仅在非空表迭代中检查输入null。
        if (!nameCp932) throw ATL::CAtlException(E_FAIL);
        if (_mbsicmp(reinterpret_cast<const unsigned char*>(ParamsPatternItemNameAt(*pattern, index).GetString()),
                reinterpret_cast<const unsigned char*>(nameCp932)) == 0)
            return static_cast<std::int32_t>(index);
        // 47A094..AF每轮重新取同receiver的数量，不快照上界。
    }
    return -1;
}

const char* ParamsResourceNameRuntime::FindName(const void* receiver, const std::int32_t id)
{return ReadParamsPatternNameNativeAt4AD1B0(static_cast<const ParamsPatternNative*>(receiver),id);}

const char* ReadParamsPatternNameNativeAt4AD1B0(const ParamsPatternNative* receiver,const std::int32_t id)
{
    // 4AD1B8..1BA负索引先于receiver解引用；缺正文不等同于合法越界。
    if (id < 0) return nullptr;
    const auto index = static_cast<std::size_t>(id);
    // 4AD1BC / 46F3FB直接读正文；signed数量由原DWORD差除12形成，不能改为安全异常或unsigned上界。
    const auto* fields=static_cast<const volatile ParamsPatternNative*>(receiver);
    const auto* begin=fields->itemsAt04.begin; // 4AD1BC先读+04；null正文的原故障地址是4。
    const auto* end=fields->itemsAt04.end;
    const auto bytes=static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end)-reinterpret_cast<std::uintptr_t>(begin));
    const auto count=std::bit_cast<std::int32_t>(bytes)/12;
    if (id >= count) return nullptr;
    // 4AD204取实际项Ref所解正文+4的CString指针，不strlen、不复制名称。
    // 4AD1DB沿已缓存begin选项，不在数量比较后重新读取vector header。
    const auto* slot=reinterpret_cast<const YtlReferenceSlotRuntime*>(
        reinterpret_cast<std::uintptr_t>(begin)+static_cast<std::uint32_t>(index)*12u);
    return ResolveParamsPatternFileName(*slot)->nameAt04.GetString();
}
}
