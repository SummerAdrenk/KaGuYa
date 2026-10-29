#include <Runtime/SpriteArchiveResourceNativeLoader.h>

#include <Runtime/AnimationFormatNativePictures.h>
#include <Runtime/PatternResourceLoadRuntime.h>
#include <Runtime/SpriteFileGroupNativeLoad.h>

#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
bool LoadPatternSpriteFromReaderNative(SpriteNativeBodyA0h& sprite,
    const ArchiveReaderReference& reader, const std::uint8_t initialization,
    const PatternResourceLoadRequest& request, AnimationFormatNativePictureServices& services)
{
    AnimationFormatNativePictures pictures(services);
    // 4CB030/4A84E0仅交付本次已读reader和配置BYTE；不读取名称参数或重取配置。
    switch (request.path)
    {
    case PatternResourceLoadPath::NormalGroup:
    case PatternResourceLoadPath::CgGroup:
        return LoadSpriteFileGroupNativeAt4B0840(sprite,
            std::bit_cast<std::int32_t>(request.memberIndex), reader, initialization,
            pictures, services.surfaces, services.strings, &services.inputMode);
    case PatternResourceLoadPath::NormalDirect:
    case PatternResourceLoadPath::NormalExc:
    case PatternResourceLoadPath::NormalFileConvert:
    case PatternResourceLoadPath::CgDirect:
    case PatternResourceLoadPath::CgExc:
        return LoadSpriteArchiveNativeAt480650(sprite, reader, initialization,
            request.loadArgument, pictures, services.surfaces, services.strings, &services.inputMode);
    default:
        throw std::invalid_argument("Unknown Pattern resource load path");
    }
}
}
