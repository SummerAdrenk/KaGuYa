#pragma once

#include <Runtime/AnimationFormatNative.h>
#include <Runtime/ArchiveCodecInputView.h>
#include <Runtime/ArchiveCodecServices.h>
#include <Runtime/GraphicsRootNative.h>

#include <cstddef>
#include <cstdint>
#include <span>

namespace KaGuYa::Start::Runtime
{
class IGraphicsSurfaceApi;

// bridge在原wrapper调用区间显式选Raw；不绑定输入槽，不探查/读取任何Ref元数据。
class AnimationFormatNativeRawInputScope final
{
public:
    explicit AnimationFormatNativeRawInputScope(ArchiveCodecInputMode* mode) noexcept;
    ~AnimationFormatNativeRawInputScope() noexcept;
    AnimationFormatNativeRawInputScope(const AnimationFormatNativeRawInputScope&) = delete;
    AnimationFormatNativeRawInputScope& operator=(const AnimationFormatNativeRawInputScope&) = delete;
private:
    ArchiveCodecInputMode* mode_;
    ArchiveCodecInputMode previous_;
};

// 全部依赖只借用，不进入44h/24h正文，也不建立第二Graphics或codec owner。
// stage默认Bounded并使用inputExtent；正式bridge明确选Raw，不按解析结果或异常回退。
struct AnimationFormatNativePictureServices final
{
    IGraphicsSurfaceApi& surfaces;
    IRuntimeStringManagerSource& strings;
    GraphicsRootOwner& graphics;
    IGraphicsModuleNativeWin32Api& win32;
    ArchiveCodecServices codecs;
    std::span<const std::uint8_t> inputExtent;
    ArchiveCodecInputMode inputMode = ArchiveCodecInputMode::Bounded;
};

// cursor是原局部指针槽，不修改输入Ref.bias；所有发布/推进均在原访问点发生。
// 零帧只消费WORD并返回false；上层4460D0不应把该AL解释为装载失败。
[[nodiscard]] bool LoadAnimationPicturesNativeAt447F60(AnimationFormatNativeBody44h& body,
    const std::uint8_t** cursor, float version, AnimationFormatNativePictureServices& services);
[[nodiscard]] bool LoadAnimationPicturesNativeAt446BD0(AnimationFormatNativeBody44h& body,
    const std::uint8_t** cursor, AnimationFormatNativePictureServices& services);
[[nodiscard]] bool LoadAnimationPicturesNativeAt4465C0(AnimationFormatNativeBody44h& body,
    const std::uint8_t** cursor, AnimationFormatNativePictureServices& services);
[[nodiscard]] bool LoadAnimationPicturesNativeAt446FB0(AnimationFormatNativeBody44h& body,
    const std::uint8_t** cursor, std::int32_t compression,
    AnimationFormatNativePictureServices& services);
[[nodiscard]] bool LoadAnimationPicturesNativeAt447500(AnimationFormatNativeBody44h& body,
    const std::uint8_t** cursor, AnimationFormatNativePictureServices& services);
[[nodiscard]] bool LoadAnimationPicturesNativeAt4479D0(AnimationFormatNativeBody44h& body,
    const std::uint8_t** cursor, AnimationFormatNativePictureServices& services);

// 445B90命令段已经写完mappingCount=1，cursor此时指向frameCount WORD。
// 本段不重读画布；旧AN零帧抛int2，AN00固定channels=4，AN10逐帧读DWORD。
void LoadLegacyAnimationPicturesNativeAt445D2F(AnimationFormatNativeBody44h& body,
    const std::uint8_t** cursor, std::uint8_t hasChannels,
    AnimationFormatNativePictureServices& services);

// 直接承接44h/Commands的正式分流；借用服务，不保存局部cursor或解析结果。
class AnimationFormatNativePictures final : public IAnimationFormatNativePictures
{
public:
    explicit AnimationFormatNativePictures(AnimationFormatNativePictureServices& services) noexcept;
    void LoadLegacyFramesAt445D2F(AnimationFormatNativeBody44h& body,
        const std::uint8_t*& cursor, std::uint8_t readChannels) override;
    std::uint8_t LoadPicture00At446BD0(AnimationFormatNativeBody44h& body,
        const std::uint8_t*& cursor) override;
    std::uint8_t LoadPicture01At4465C0(AnimationFormatNativeBody44h& body,
        const std::uint8_t*& cursor) override;
    std::uint8_t LoadPicture10Or11At446FB0(AnimationFormatNativeBody44h& body,
        const std::uint8_t*& cursor, std::uint8_t is11) override;
    std::uint8_t LoadPicture20At447500(AnimationFormatNativeBody44h& body,
        const std::uint8_t*& cursor) override;
    std::uint8_t LoadPicture30At4479D0(AnimationFormatNativeBody44h& body,
        const std::uint8_t*& cursor) override;
private:
    AnimationFormatNativePictureServices& services_;
};
}
