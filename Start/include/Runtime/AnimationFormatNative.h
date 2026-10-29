#pragma once

#include <Runtime/SpriteNativeStorage.h>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
struct AnimationFormatNativeBody44h;
struct AnimationFormatNativeVtable final
{
    AnimationFormatNativeBody44h* (__thiscall* deletingDestructorAt00)(
        AnimationFormatNativeBody44h*, std::uint32_t flags);
};

// 4580A0/47EECB/4685B0：真实44h正文；服务借用与输入长度不进入原对象。
struct AnimationFormatNativeBody44h final
{
    const AnimationFormatNativeVtable* vptrAt00;
    union { RuntimeCString nameAt04; };
    std::int32_t xAt08;
    std::int32_t yAt0C;
    std::int32_t widthAt10;
    std::int32_t heightAt14;
    std::uint8_t repeatAt18;
    std::uint8_t unwrittenAt19[3];
    std::int32_t mappingCountAt1C;
    union { YtlReferenceSlotRuntime commandsAt20; };
    union { YtlReferenceSlotRuntime branchesAt2C; };
    union { YtlReferenceSlotRuntime surfacesAt38; };
    AnimationFormatNativeBody44h() noexcept {}
    // 显式原地址析构负责唯一释放；自动析构不能再销毁CString/Ref。
    ~AnimationFormatNativeBody44h() noexcept {}
    AnimationFormatNativeBody44h(const AnimationFormatNativeBody44h&) = delete;
    AnimationFormatNativeBody44h& operator=(const AnimationFormatNativeBody44h&) = delete;
};
static_assert(sizeof(AnimationFormatNativeVtable) == 4);
static_assert(sizeof(AnimationFormatNativeBody44h) == 0x44 && alignof(AnimationFormatNativeBody44h) == 4);
static_assert(std::is_standard_layout_v<AnimationFormatNativeBody44h>);
static_assert(offsetof(AnimationFormatNativeBody44h, vptrAt00) == 0);
static_assert(offsetof(AnimationFormatNativeBody44h, nameAt04) == 4);
static_assert(offsetof(AnimationFormatNativeBody44h, xAt08) == 8);
static_assert(offsetof(AnimationFormatNativeBody44h, yAt0C) == 0x0C);
static_assert(offsetof(AnimationFormatNativeBody44h, widthAt10) == 0x10);
static_assert(offsetof(AnimationFormatNativeBody44h, heightAt14) == 0x14);
static_assert(offsetof(AnimationFormatNativeBody44h, repeatAt18) == 0x18);
static_assert(offsetof(AnimationFormatNativeBody44h, unwrittenAt19) == 0x19);
static_assert(offsetof(AnimationFormatNativeBody44h, mappingCountAt1C) == 0x1C);
static_assert(offsetof(AnimationFormatNativeBody44h, commandsAt20) == 0x20);
static_assert(offsetof(AnimationFormatNativeBody44h, branchesAt2C) == 0x2C);
static_assert(offsetof(AnimationFormatNativeBody44h, surfacesAt38) == 0x38);

const AnimationFormatNativeVtable& AnimationFormatNativeMethods() noexcept;
AnimationFormatNativeBody44h* ConstructAnimationFormatNativeAt4580A0(AnimationFormatNativeBody44h& body,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
void DestroyAnimationFormatNativeAt4628B0(AnimationFormatNativeBody44h& body) noexcept;
AnimationFormatNativeBody44h* DeleteAnimationFormatNativeAt4685B0(
    AnimationFormatNativeBody44h* body, std::uint32_t flags) noexcept;
AnimationFormatNativeBody44h* NewAnimationFormatNativeAt47EECB(
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

// 两入口均构造未拥有的Ref，不释放旧槽；control失败保留已发布base，不回收已构造正文。
// 47EF04：control字段先写，47EF42发布control；4987F0：先发布control再写字段。
void ConstructAnimationFormatReferenceAt47EF04(YtlReferenceSlotRuntime& out, AnimationFormatNativeBody44h* body);
void ConstructAnimationFormatReferenceAt4987F0(YtlReferenceSlotRuntime& out,
    AnimationFormatNativeBody44h* body, std::uint8_t owning);
AnimationFormatNativeBody44h* ResolveAnimationFormatReference(const YtlReferenceSlotRuntime& slot) noexcept;

// 正文外必传的PIC执行借用；没有默认成功实现，不保存输入或第二份Surface/codec owner。
// PIC30实现按447A7B/447D4E各自取BlockConvert，借用现有IArchiveCodecManagerSource及
// GetBlockConvertAt445220 / DecodeBlockConvertNativeAt445300，不能跨回调缓存manager。
class IAnimationFormatNativePictures
{
public:
    // 入口在445D2F读取frameCount之前；负责该WORD、旧空帧int2及完整逐帧尾段。
    virtual void LoadLegacyFramesAt445D2F(AnimationFormatNativeBody44h& body,
        const std::uint8_t*& cursor, std::uint8_t readChannels) = 0;
    virtual std::uint8_t LoadPicture00At446BD0(AnimationFormatNativeBody44h& body,
        const std::uint8_t*& cursor) = 0;
    virtual std::uint8_t LoadPicture01At4465C0(AnimationFormatNativeBody44h& body,
        const std::uint8_t*& cursor) = 0;
    virtual std::uint8_t LoadPicture10Or11At446FB0(AnimationFormatNativeBody44h& body,
        const std::uint8_t*& cursor, std::uint8_t is11) = 0;
    virtual std::uint8_t LoadPicture20At447500(AnimationFormatNativeBody44h& body,
        const std::uint8_t*& cursor) = 0;
    virtual std::uint8_t LoadPicture30At4479D0(AnimationFormatNativeBody44h& body,
        const std::uint8_t*& cursor) = 0;
protected:
    ~IAnimationFormatNativePictures() = default;
};

// 原无span上界；调用者保证原访问定义域。游标按每次原读取/写入点推进，不预解析整包。
std::int32_t LoadAnimationFormatNativeAt448010(AnimationFormatNativeBody44h& body,
    const YtlReferenceSlotRuntime& input, IAnimationFormatNativePictures& pictures);
void LoadLegacyAnimationCommandsNativeAt445B90(AnimationFormatNativeBody44h& body,
    const std::uint8_t*& cursor, std::uint8_t readChannels, IAnimationFormatNativePictures& pictures);
std::uint8_t LoadNewAnimationCommandsNativeAt4460D0(AnimationFormatNativeBody44h& body,
    const std::uint8_t*& cursor, float version, IAnimationFormatNativePictures& pictures);
std::uint8_t LoadAnimationPictureNativeAt447F60(AnimationFormatNativeBody44h& body,
    const std::uint8_t*& cursor, float version, IAnimationFormatNativePictures& pictures);

// 原int vector的扩容插入叶，value可以借用旧vector；读取发生在分配之后。
std::int32_t* InsertAnimationCommandArgumentAt4447B0(EnvironmentVectorStorage<std::int32_t>& values,
    std::int32_t* position, const std::int32_t* value);
void SetAnimationCommandPairAt448670(SpriteCommandNativeBody10h& command,
    std::int32_t first, std::int32_t second);
}
