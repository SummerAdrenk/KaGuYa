#include <Runtime/ParamsHeaderRuntime.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/ParamsRuntimeProfile.h>

#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
bool MatchParamsHeaderAt413B30(const RuntimeCString& globalHeader,
    const ParamsInputBytesBorrow input, std::uint32_t& matchedBytes)
{
    ArchiveRawResourceNameRuntime parameter(ArchiveRawResourceNameRuntime::ProducerTag{},
        [&](RuntimeCString* output)
        {
            auto* source = reinterpret_cast<ATL::CStringData*>(
                reinterpret_cast<std::uintptr_t>(globalHeader.GetString()) - sizeof(ATL::CStringData));
            auto* copied = CloneRuntimeCStringDataAt414540(source);
            const char* characters = static_cast<const char*>(copied->data());
            std::memcpy(output, &characters, sizeof(characters));
        });

    // 参数复制完成才进入413B30；Clone抛出时不能提前把调用方输出清零。
    const char* const characters = parameter.Value().GetString();
    const auto* const header = reinterpret_cast<const ATL::CStringData*>(characters) - 1;
    matchedBytes = 0;
    bool matched = true;
    auto length = header->nDataLength;
    for (std::int32_t index = 0; index < length;)
    {
        if (index < 0 || index > length) throw ATL::CAtlException(E_INVALIDARG);
        const auto boundedSize = input.BoundedSize();
        if (boundedSize && static_cast<std::size_t>(index) >= *boundedSize)
            throw ParamsRuntimeException(ParamsRuntimeError::TruncatedInput,
                static_cast<std::size_t>(index),
                "413B30: params header comparison exceeds input (safe boundary)");
        // 原头字符movsx，输入movzx；高位字节不能按memcmp当作相等。
        const auto expected = static_cast<int>(std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(characters[index])));
        const auto actual = std::to_integer<unsigned char>(input.Begin()[static_cast<std::size_t>(index)]);
        if (expected != static_cast<int>(actual))
        {
            matched = false;
            break;
        }
        ++index;
        matchedBytes = static_cast<std::uint32_t>(index);
        length = header->nDataLength;
    }
    // 413B9B先撤参数清理资格，再释放缓存header；Free失败不能二次清同一参数。
    parameter.ReleaseNormalCached(characters);
    return matched;
}
}
