#include <Runtime/PictureLayerFormatNative.h>

#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::uint16_t ReadWord(const std::uint8_t* input) noexcept
{
    std::uint16_t value;
    std::memcpy(&value, input, sizeof(value));
    return value;
}
}

std::int32_t LoadPictureLayerFormatNativeAt4482A0(AnimationFormatNativeBody44h& body,
    const YtlReferenceSlotRuntime& input, IAnimationFormatNativePictures& pictures)
{
    try
    {
        // 4482D0先进入try，再解析一次byte Ref；不用count构造现代span，也不增持输入。
        const auto* const bytes = ResolveEnvironmentObjectReference<const std::uint8_t>(input);
        if (ReadWord(bytes) != 0x4C50u) throw 3;
        float version;
        switch (ReadWord(bytes + 2))
        {
        case 0x3030u: version = 0.0f; break;
        case 0x3130u: version = 0.1f; break;
        case 0x3031u: version = 1.0f; break;
        case 0x3131u: version = 1.1f; break;
        case 0x3032u: version = 2.0f; break;
        case 0x3033u: version = 3.0f; break;
        default: throw 6;
        }
        const auto* cursor = bytes + 4;
        // 44837F检查有效AL，不限返回1；失败不清理或回滚PIC已写入的44h前缀。
        if (!LoadAnimationPictureNativeAt447F60(body, cursor, version, pictures)) throw 2;
        return 0;
    }
    catch (int value) { return value; }
    catch (...) { return -1; }
}
}
