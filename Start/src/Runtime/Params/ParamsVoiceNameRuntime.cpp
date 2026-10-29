#include <Runtime/ParamsVoiceNameRuntime.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/ParamsPatternNativeAccess.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
ParamsVoiceNameRuntime::ParamsVoiceNameRuntime(ParamsRuntimeRootOwner& params) noexcept
    : params_(params)
{
}

const char* ParamsVoiceNameRuntime::FindVoiceNameAt4D3286(const std::int32_t index) const
{
    // 4D31DA..4D3284先取得静态根及其+10h Pattern，再4D3286判断索引符号。
    // 只借Ref槽，不增持保活；允许读取已发布但尚未完成整根读取的Pattern。
    const auto& pattern = params_.GetOrCreateAt418480().Pattern();
    if (index < 0) return nullptr;
    // 4D328A对空Pattern正文解引用；仅非负索引进入此现代安全拒绝。
    if (!pattern) throw std::logic_error("VOICE name lookup Pattern body is null");

    // 434E93 -> 41DFD0的首表对应items；4D328A..4D32A7按12字节Ref计算项数。
    const auto position = static_cast<std::size_t>(index);
    if (position >= ParamsPatternVectorSize(pattern->itemsAt04)) return nullptr;
    // 4D32D2/4D32E1直接借项正文+04的CString指针；不复制、不展开首NUL后的内容。
    return ParamsPatternItemNameAt(*pattern, position).GetString();
}
}
