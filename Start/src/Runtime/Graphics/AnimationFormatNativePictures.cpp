#include <Runtime/AnimationFormatNativePictures.h>

#include <Runtime/ArchiveCodecManagerRuntime.h>
#include <Runtime/ArchiveHuffmanRuntime.h>
#include <Runtime/ArchiveLzssRuntime.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/BlockConvertNative.h>
#include <Runtime/BmrDecoderRuntime.h>
#include <Runtime/ByteArrayReferenceRuntime.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/SpriteNativeStorage.h>

#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = AnimationFormatNativeBody44h;
using Services = AnimationFormatNativePictureServices;
using Byte = std::uint8_t;
using Cursor = const Byte**;

std::int32_t& CanvasX(Body& body) noexcept { return body.xAt08; }
std::int32_t& CanvasY(Body& body) noexcept { return body.yAt0C; }
std::int32_t& CanvasWidth(Body& body) noexcept { return body.widthAt10; }
std::int32_t& CanvasHeight(Body& body) noexcept { return body.heightAt14; }
YtlReferenceSlotRuntime& Surfaces(Body& body) noexcept
{
    return body.surfacesAt38;
}

template<class T>
T Read(const void* address) noexcept
{
    T value;
    std::memcpy(&value, address, sizeof(value));
    return value;
}
template<class T>
T* Add(T* pointer, std::uint32_t bytes) noexcept
{
    return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(pointer) + bytes);
}
void Advance(Cursor cursor, std::uint32_t count) noexcept { *cursor = Add(*cursor, count); }
template<class T>
T Take(Cursor cursor) noexcept
{
    const auto value = Read<T>(*cursor);
    Advance(cursor, static_cast<std::uint32_t>(sizeof(T)));
    return value;
}
std::int32_t Signed(std::uint32_t value) noexcept { return std::bit_cast<std::int32_t>(value); }
std::uint32_t Product(std::int32_t width, std::int32_t height, std::int32_t channels) noexcept
{
    return static_cast<std::uint32_t>(width) * static_cast<std::uint32_t>(height) *
        static_cast<std::uint32_t>(channels);
}
std::int32_t Sum(std::int32_t first, std::int32_t second) noexcept
{
    return Signed(static_cast<std::uint32_t>(first) + static_cast<std::uint32_t>(second));
}
std::uint32_t Stride(YtlReferenceControlRuntime* control) noexcept
{
    return control ? static_cast<EnvironmentObjectReferenceControl*>(control)->strideAt0C : 0u;
}
std::uint32_t Count(YtlReferenceControlRuntime* control) noexcept
{
    return control ? static_cast<EnvironmentObjectReferenceControl*>(control)->countAt10 : 0u;
}
Byte* ResolveBytes(std::uintptr_t base, std::uint32_t bias,
    YtlReferenceControlRuntime* control) noexcept
{
    return base ? reinterpret_cast<Byte*>(base + bias * Stride(control)) : nullptr;
}
GraphicsSurfaceNativeBody* Surface(Body& body, std::uint32_t index) noexcept
{
    const auto& slot = Surfaces(body);
    if (!slot.baseAt00) return nullptr;
    return reinterpret_cast<GraphicsSurfaceNativeBody*>(slot.baseAt00 +
        (slot.biasAt04 + index) * Stride(slot.controlAt08));
}
void ReadCanvas(Body& body, Cursor cursor) noexcept
{
    // 原先写字段再推进游标；不要改成先Take再赋值，cursor可能别名正文。
    CanvasX(body) = Read<std::int32_t>(*cursor);
    Advance(cursor, 4);
    CanvasY(body) = Read<std::int32_t>(*cursor);
    Advance(cursor, 4);
    CanvasWidth(body) = Read<std::int32_t>(*cursor);
    Advance(cursor, 4);
    CanvasHeight(body) = Read<std::int32_t>(*cursor);
    Advance(cursor, 4);
}
void ReplaceSurfaces(Body& body, std::uint16_t count, Services& services)
{
    ReplaceSpriteSurfaceArrayAt444FE0(Surfaces(body), count, services.surfaces, services.strings);
}

FARPROC ResolveInline(GraphicsModuleNativeBody8h* module, const char* exportName,
    Services& services)
{
    union Name
    {
        Name() noexcept {}
        ~Name() noexcept {}
        RuntimeCString value;
    } name;
    // 内联417360/GetNil/410290：测长在GetNil之后；正常resolver前撤销名字EH资格。
    ConstructArchiveResourceNameAt40B530(&name.value, exportName, services.strings);
    const char* const characters = name.value.GetString();
    const auto function = services.win32.GetProcAddress(module->moduleAt04, characters);
    ReleaseRuntimeCStringHeader(reinterpret_cast<ATL::CStringData*>(
        const_cast<char*>(characters) - sizeof(ATL::CStringData)));
    return function;
}

std::int32_t CreateInline(GraphicsSurfaceNativeBody* receiver, std::int32_t width,
    std::int32_t height, std::int32_t alpha, Services& services)
{
    receiver->ResetAt432670(services.surfaces);
    auto* const root = services.graphics.CaptureAt4182F0();
    std::int32_t handle = 0;
    if (root->moduleAt00.baseAt00)
    {
        auto* const module = ResolveGraphicsModuleReference(root->moduleAt00);
        using Function = std::int32_t (__cdecl*)(std::int32_t, std::int32_t,
            std::int32_t, std::int32_t);
        const auto function = reinterpret_cast<Function>(ResolveInline(module, "SurfaceCreate", services));
        handle = function(width, height, alpha, 0);
    }
    receiver->handleAt04 = handle;
    if (!handle) throw 4;
    return handle;
}
void CommitCreated(GraphicsSurfaceNativeBody* receiver, std::int32_t width,
    std::int32_t height, std::uint8_t alpha) noexcept
{
    receiver->widthAt08 = width;
    receiver->heightAt0C = height;
    receiver->alphaAt18 = alpha;
}
void Origin(Body& body, std::uint32_t index, std::int32_t x, std::int32_t y) noexcept
{
    auto* const receiver = Surface(body, index);
    receiver->originXAt10 = x;
    receiver->originYAt14 = y;
}
void UploadCaptured(GraphicsRootNativeBody1Ch* root, std::int32_t handle,
    std::int32_t width, std::int32_t height, std::int32_t channels,
    const void* pixels, Services& services)
{
    if (!root->moduleAt00.baseAt00) return;
    auto* const module = ResolveGraphicsModuleReference(root->moduleAt00);
    using Function = void (__cdecl*)(std::int32_t, std::int32_t, std::int32_t,
        std::int32_t, const void*);
    const auto function = reinterpret_cast<Function>(ResolveInline(module, "SurfaceLoadANM", services));
    function(handle, width, height, channels, pixels);
}
void UploadBeforeCapture(std::int32_t handle, std::int32_t width,
    std::int32_t height, std::int32_t channels, const void* pixels, Services& services)
{
    // 4466BB/4470F9/447424/44793D的五参数都在4182F0前压栈。
    auto* const root = services.graphics.CaptureAt4182F0();
    UploadCaptured(root, handle, width, height, channels, pixels, services);
}

class Scratch final
{
public:
    Scratch() noexcept {}
    ~Scratch() noexcept { if (active_) bytes.~ByteArrayReferenceRuntime(); }
    void Begin() noexcept
    {
        ::new (static_cast<void*>(&bytes)) ByteArrayReferenceRuntime;
        active_ = true;
    }
    void Finish(YtlReferenceControlRuntime* control) noexcept
    {
        // 正常尾使用寄存器control，不在payload回调后改读局部Ref；异常则由析构读当前槽。
        active_ = false;
        if (control && --control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            control->DeleteControlAtVslot04(1);
        }
    }
    union { ByteArrayReferenceRuntime bytes; };
private:
    bool active_ = false;
};

// 原局部只写BYTE的参数槽，高三字节保持未写；不伪称原push把AL零扩展。
struct ByteArgument final
{
    alignas(std::uint32_t) Byte storage[4];
    void SetLow(Byte value) noexcept { storage[0] = value; }
    std::int32_t Bits() const noexcept { return Read<std::int32_t>(storage); }
};

ArchiveCodecInputView CodecInput(const Services& services, const Byte* address) noexcept
{
    // 生产沿原局部cursor走Raw；不读Ref/count，不建立或推断输入span。
    if (services.inputMode == ArchiveCodecInputMode::Raw) return ArchiveCodecInputView::Raw(address);
    const auto begin = reinterpret_cast<std::uintptr_t>(services.inputExtent.data());
    const auto count = services.inputExtent.size();
    const auto value = reinterpret_cast<std::uintptr_t>(address);
    // 仅显式stage保留原有界行为，范围外仍在codec实际读取处拒绝。
    const auto available = value >= begin && value - begin <= count ? count - (value - begin) : 0;
    return ArchiveCodecInputView::Bounded({address, available});
}
void HuffmanDelta(const Byte* input, Byte* destination, const Byte* previous,
    std::uint32_t count, Services& services)
{
    auto* const manager = services.codecs.managers.CaptureAt445910();
    auto* const decoder = GetHuffmanAt43DE24(*manager);
    decoder->DecodeAt43DE24(CodecInput(services, input), {destination, count},
        nullptr, nullptr, services.codecs.errors);
    for (std::uint32_t index = 0; index != count; ++index)
        destination[index] = static_cast<Byte>(destination[index] + previous[index]);
}

void RleDeltaAt447300(const Byte* input, Byte* destination, const Byte* previous,
    std::uint32_t count, std::uint32_t step) noexcept
{
    std::uint32_t sourceIndex = 0;
    for (std::uint32_t lane = 0; lane != step; ++lane)
    {
        // 原每lane先写首项，即使step超过N；packedSize不是解码截断边界。
        auto last = input[sourceIndex++];
        destination[lane] = static_cast<Byte>(previous[lane] + last);
        std::uint32_t position = lane + step;
        while (position < count)
        {
            const auto value = input[sourceIndex++];
            destination[position] = static_cast<Byte>(previous[position] + value);
            position += step;
            if (value == last)
            {
                std::uint32_t run = input[sourceIndex++];
                if (run & 0x80u)
                    run = ((run & 0x7Fu) << 8u) + 0x80u + input[sourceIndex++];
                for (std::uint32_t remaining = run; remaining != 0; --remaining)
                {
                    destination[position] = static_cast<Byte>(previous[position] + value);
                    position += step;
                }
                if (position >= count) break;
                last = input[sourceIndex++];
                destination[position] = static_cast<Byte>(previous[position] + last);
                position += step;
            }
            else last = value;
        }
    }
}
void LoadRawFrames(Body& body, Cursor cursor, std::uint16_t count,
    bool hasChannels, Services& services)
{
    for (std::uint32_t index = 0; index < count; ++index)
    {
        const auto x = Take<std::int32_t>(cursor);
        const auto y = Take<std::int32_t>(cursor);
        const auto width = Take<std::int32_t>(cursor);
        const auto height = Take<std::int32_t>(cursor);
        auto* const receiver = Surface(body, index);
        static_cast<void>(CreateInline(receiver, width, height, 1, services));
        CommitCreated(receiver, width, height, 1);
        Origin(body, index, x, y);
        const auto channels = hasChannels ? Take<std::int32_t>(cursor) : 4;
        const auto handle = Surface(body, index)->handleAt04;
        auto* const root = services.graphics.CaptureAt4182F0();
        // 446E8B：像素指针晚于正常Leave，不与446DEE的handle一起提前缓存。
        const auto* const pixels = *cursor;
        UploadCaptured(root, handle, width, height, channels, pixels, services);
        Advance(cursor, Product(width, height, channels));
    }
}
}

bool LoadAnimationPicturesNativeAt446BD0(Body& body, Cursor cursor, Services& services)
{
    const auto count = Take<std::uint16_t>(cursor);
    if (!count) return false;
    GraphicsSurfaceNativeScope surfaceScope(services.surfaces);
    ReplaceSurfaces(body, count, services);
    ReadCanvas(body, cursor);
    LoadRawFrames(body, cursor, count, true, services);
    return true;
}

bool LoadAnimationPicturesNativeAt4465C0(Body& body, Cursor cursor, Services& services)
{
    const auto count = Take<std::uint16_t>(cursor);
    if (!count) return false;
    GraphicsSurfaceNativeScope surfaceScope(services.surfaces);
    ReplaceSurfaces(body, count, services);
    ReadCanvas(body, cursor);
    const auto channels = Take<std::int32_t>(cursor);
    const auto& slot = Surfaces(body);
    const auto alphaSource = slot.baseAt00 ? Stride(slot.controlAt08) :
        static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(*cursor));
    const auto alpha = Signed((alphaSource & 0xFFFFFF00u) | (channels == 4 ? 1u : 0u));
    auto* const first = Surface(body, 0);
    const auto firstHeight = CanvasHeight(body);
    const auto firstWidth = CanvasWidth(body);
    if (!first->CreateAt415010(services.surfaces, firstWidth, firstHeight, alpha, 0)) throw 4;
    auto* const upload = Surface(body, 0);
    const auto* const pixels = *cursor;
    const auto uploadHeight = CanvasHeight(body);
    const auto uploadWidth = CanvasWidth(body);
    const auto handle = upload->handleAt04;
    UploadBeforeCapture(handle, uploadWidth, uploadHeight, channels, pixels, services);

    const auto imageHeight = CanvasHeight(body);
    const auto imageWidth = CanvasWidth(body);
    const Byte* previous = reinterpret_cast<const Byte*>(*cursor);
    const auto imageBytes = Product(imageWidth, imageHeight, channels);
    Advance(cursor, imageBytes);
    Scratch scratch;
    scratch.Begin();
    scratch.bytes.AllocateAt412700(imageBytes + imageBytes);
    auto* control = scratch.bytes.RawSlot().controlAt08;
    auto* destination = ResolveBytes(scratch.bytes.RawSlot().baseAt00,
        scratch.bytes.RawSlot().biasAt04, control);
    const auto rowBytes = static_cast<std::uint32_t>(CanvasWidth(body)) *
        static_cast<std::uint32_t>(channels);
    std::uint32_t half = 0;
    ByteArgument inlineAlpha;
    inlineAlpha.SetLow(static_cast<Byte>(alpha));
    for (std::uint32_t index = 1; index < count; ++index)
    {
        auto* const receiver = Surface(body, index);
        const auto height = CanvasHeight(body);
        const auto width = CanvasWidth(body);
        static_cast<void>(CreateInline(receiver, width, height, inlineAlpha.Bits(), services));
        CommitCreated(receiver, width, height, static_cast<Byte>(alpha));
        std::uint32_t rowOffset = 0;
        for (std::uint32_t row = 0; Signed(row) < CanvasHeight(body); ++row)
        {
            for (std::uint32_t column = 0; column < rowBytes; ++column)
            {
                const auto offset = rowOffset + column;
                const auto delta = *Add(reinterpret_cast<const Byte*>(*cursor), offset);
                *Add(destination, offset) = static_cast<Byte>(delta + *Add(previous, offset));
            }
            rowOffset += rowBytes;
        }
        const auto nextHandle = Surface(body, index)->handleAt04;
        auto* const root = services.graphics.CaptureAt4182F0();
        // 446A66/446A6C：这两个字段在Leave之后、CString回调之前读取。
        const auto nextHeight = CanvasHeight(body);
        const auto nextWidth = CanvasWidth(body);
        UploadCaptured(root, nextHandle, nextWidth, nextHeight, channels, destination, services);
        control = scratch.bytes.RawSlot().controlAt08;
        half ^= imageBytes;
        Advance(cursor, imageBytes);
        previous = destination;
        destination = ResolveBytes(scratch.bytes.RawSlot().baseAt00,
            scratch.bytes.RawSlot().biasAt04 + half, control);
    }
    scratch.Finish(control);
    return true;
}

bool LoadAnimationPicturesNativeAt446FB0(Body& body, Cursor cursor,
    std::int32_t compression, Services& services)
{
    const auto count = Take<std::uint16_t>(cursor);
    if (!count) return false;
    GraphicsSurfaceNativeScope surfaceScope(services.surfaces);
    ReplaceSurfaces(body, count, services);
    ReadCanvas(body, cursor);
    const auto x = Take<std::int32_t>(cursor);
    const auto y = Take<std::int32_t>(cursor);
    const auto width = Take<std::int32_t>(cursor);
    const auto height = Take<std::int32_t>(cursor);
    if (!Surface(body, 0)->CreateAt415010(services.surfaces, width, height, 1, 0)) throw 4;
    Origin(body, 0, x, y);
    const auto channels = Take<std::int32_t>(cursor);
    const auto* const pixels = *cursor;
    const auto handle = Surface(body, 0)->handleAt04;
    UploadBeforeCapture(handle, width, height, channels, pixels, services);
    const Byte* previous = reinterpret_cast<const Byte*>(*cursor);
    const auto imageBytes = Product(width, height, channels);
    Advance(cursor, imageBytes);
    Scratch scratch;
    scratch.Begin();
    scratch.bytes.AllocateAt412700(imageBytes + imageBytes);
    auto* destination = scratch.bytes.Data();
    std::uint32_t half = 0;
    if (compression != 0) Advance(cursor, 2);
    for (std::uint32_t index = 1; index < count; ++index)
    {
        auto* const receiver = Surface(body, index);
        static_cast<void>(CreateInline(receiver, width, height, 1, services));
        CommitCreated(receiver, width, height, 1);
        Origin(body, index, x, y);
        std::uint32_t packedSize;
        if (compression == 0)
        {
            const auto step = Take<Byte>(cursor);
            packedSize = Take<std::uint32_t>(cursor);
            RleDeltaAt447300(reinterpret_cast<const Byte*>(*cursor), destination, previous,
                imageBytes, step);
        }
        else if (compression == 1)
        {
            const auto* const input = reinterpret_cast<const Byte*>(*cursor);
            HuffmanDelta(input, destination, previous, imageBytes, services);
            // 4473FD：解码成功后重读当前游标处的长度，不沿用解码器先前的长度。
            packedSize = Take<std::uint32_t>(cursor);
        }
        else throw 4;
        const auto nextHandle = Surface(body, index)->handleAt04;
        UploadBeforeCapture(nextHandle, width, height, channels, destination, services);
        Advance(cursor, packedSize);
        previous = destination;
        half ^= imageBytes;
        destination = scratch.bytes.DataAt(half);
    }
    scratch.Finish(scratch.bytes.RawSlot().controlAt08);
    return true;
}

bool LoadAnimationPicturesNativeAt447500(Body& body, Cursor cursor, Services& services)
{
    const auto count = Take<std::uint16_t>(cursor);
    if (!count) return false;
    GraphicsSurfaceNativeScope surfaceScope(services.surfaces);
    ReplaceSurfaces(body, count, services);
    ReadCanvas(body, cursor);
    Advance(cursor, 4);
    const auto compression = Take<std::uint16_t>(cursor);
    Scratch scratch;
    scratch.Begin();
    YtlReferenceControlRuntime* control = nullptr;
    // 原var_40只有BMR命中才写；miss不补默认长度、不改成现代格式错误。
    std::uint32_t identifiedSize;
    ByteArgument alphaArgument;
    for (std::uint32_t index = 0; index < count; ++index)
    {
        const auto x = Take<std::int32_t>(cursor);
        const auto y = Take<std::int32_t>(cursor);
        const auto width = Take<std::int32_t>(cursor);
        const auto height = Take<std::int32_t>(cursor);
        const auto channels = Take<std::int32_t>(cursor);
        const auto alpha = static_cast<Byte>(channels == 4);
        alphaArgument.SetLow(alpha);
        auto* const receiver = Surface(body, index);
        static_cast<void>(CreateInline(receiver, width, height, alphaArgument.Bits(), services));
        receiver->heightAt0C = height;
        receiver->alphaAt18 = alpha;
        receiver->widthAt08 = width;
        // 4476F7/4476FA：Create回调完成后才读当前画布位置，再重新解析Surface写origin。
        const auto originY = Sum(CanvasY(body), y);
        const auto originX = Sum(CanvasX(body), x);
        Origin(body, index, originX, originY);
        const auto packedSize = Take<std::uint32_t>(cursor);
        std::uintptr_t base;
        if (compression == 3)
        {
            const auto* const input = reinterpret_cast<const Byte*>(*cursor);
            auto* const identifyingManager = services.codecs.managers.CaptureAt445910();
            auto* const identifying = GetBmrAt445110(*identifyingManager);
            const auto* const payload = identifying->IdentifyAt445A00(
                CodecInput(services, input), identifiedSize);
            base = scratch.bytes.RawSlot().baseAt00;
            if (!base || Count(control) != identifiedSize)
            {
                scratch.bytes.AllocateAt412700(identifiedSize);
                control = scratch.bytes.RawSlot().controlAt08;
                base = scratch.bytes.RawSlot().baseAt00;
            }
            auto* const destination = ResolveBytes(base, scratch.bytes.RawSlot().biasAt04, control);
            const auto extent = identifiedSize;
            // 4477A7是第二次独立manager/GetBmr；不沿用Identify的decoder身份。
            auto* const decodingManager = services.codecs.managers.CaptureAt445910();
            auto* const decoder = GetBmrAt445110(*decodingManager);
            decoder->DecodeAt43DCF0(CodecInput(services, payload), {destination, extent},
                services.codecs.managers, services.codecs.errors);
        }
        else if (compression == 4)
        {
            base = scratch.bytes.RawSlot().baseAt00;
            const auto outputSize = Read<std::uint32_t>(*cursor);
            if (!base || Count(control) != outputSize)
            {
                scratch.bytes.AllocateAt412700(outputSize);
                control = scratch.bytes.RawSlot().controlAt08;
                base = scratch.bytes.RawSlot().baseAt00;
            }
            auto* const destination = ResolveBytes(base, scratch.bytes.RawSlot().biasAt04, control);
            const auto extent = outputSize;
            auto* const manager = services.codecs.managers.CaptureAt445910();
            const bool cold = manager->lzssAt18.baseAt00 == 0;
            auto* const decoder = GetLzssAt4840B0(*manager);
            // 原冷工厂4478A3重取scratch.base，热路径沿用EDI；目标仍是getter前的EBX。
            if (cold) base = scratch.bytes.RawSlot().baseAt00;
            const auto* const input = Add(reinterpret_cast<const Byte*>(*cursor), 4);
            decoder->DecodeAt445750(CodecInput(services, input), {destination, extent});
        }
        else throw 4;

        // 4478FB重新取control，但base仍沿用上述EDI；正常尾EBX跨Upload保持。
        control = scratch.bytes.RawSlot().controlAt08;
        const auto* const pixels = ResolveBytes(base, scratch.bytes.RawSlot().biasAt04, control);
        const auto handle = Surface(body, index)->handleAt04;
        UploadBeforeCapture(handle, width, height, channels, pixels, services);
        Advance(cursor, packedSize);
    }
    scratch.Finish(control);
    return true;
}

bool LoadAnimationPicturesNativeAt4479D0(Body& body, Cursor cursor, Services& services)
{
    const auto count = Take<std::uint16_t>(cursor);
    if (!count) return false;
    GraphicsSurfaceNativeScope surfaceScope(services.surfaces);
    ReplaceSurfaces(body, count, services);
    ReadCanvas(body, cursor);
    const auto channels = Take<std::int32_t>(cursor);
    const auto blockSize = Take<Byte>(cursor);
    const auto blockHeight = CanvasHeight(body);
    const auto blockWidth = CanvasWidth(body);
    auto* const manager = services.codecs.managers.CaptureAt445910();
    auto* const converter = GetBlockConvertAt445220(*manager);
    converter->widthAt00 = blockWidth;
    converter->blockSizeAt0C = blockSize;
    converter->heightAt04 = blockHeight;
    converter->channelsAt08 = channels;

    // 447A9E..447AC3：getter后的当前cursor/尺寸；此时才建立scratch展开资格。
    const Byte* pixels = reinterpret_cast<const Byte*>(*cursor);
    const auto imageHeight = CanvasHeight(body);
    const auto imageWidth = CanvasWidth(body);
    const auto imageBytes = Product(imageWidth, imageHeight, channels);
    const auto alpha = static_cast<Byte>(channels == 4);
    Scratch scratch;
    scratch.Begin();
    const Byte* previous = nullptr;
    std::uint32_t half = imageBytes;
    for (std::uint32_t index = 0; index < count; ++index)
    {
        auto* const receiver = Surface(body, index);
        const auto height = CanvasHeight(body);
        const auto width = CanvasWidth(body);
        // var_34原先完整写过零扩展blockSize，随后只改AL，故本处高24位有明确零值。
        static_cast<void>(CreateInline(receiver, width, height, alpha, services));
        CommitCreated(receiver, width, height, alpha);
        if (index == 0)
        {
            // 447BED先推进，上传用447A9E缓存的输入地址；Create若换cursor不能重取pixels。
            Advance(cursor, imageBytes);
        }
        else
        {
            if (!scratch.bytes.RawSlot().baseAt00)
                scratch.bytes.AllocateAt412700(imageBytes + imageBytes);
            auto* const destination = scratch.bytes.DataAt(half);
            pixels = destination;
            const auto encoded = Take<Byte>(cursor);
            if (encoded == 0)
            {
                for (std::uint32_t offset = 0; offset != imageBytes; ++offset)
                {
                    const auto delta = *Add(reinterpret_cast<const Byte*>(*cursor), offset);
                    *Add(destination, offset) = static_cast<Byte>(delta + *Add(previous, offset));
                }
                Advance(cursor, imageBytes);
            }
            else
            {
                const auto packedSize = Take<std::uint32_t>(cursor);
                const auto* const payload = reinterpret_cast<const Byte*>(*cursor);
                auto* const currentManager = services.codecs.managers.CaptureAt445910();
                auto* const currentConverter = GetBlockConvertAt445220(*currentManager);
                DecodeBlockConvertNativeAt445300(*currentConverter, previous, destination, payload);
                Advance(cursor, packedSize);
            }
        }
        const auto handle = Surface(body, index)->handleAt04;
        auto* const root = services.graphics.CaptureAt4182F0();
        // 447E2C/447E32：上传尺寸晚读于Leave，且在GetNil/resolver/Free前固定。
        const auto uploadHeight = CanvasHeight(body);
        const auto uploadWidth = CanvasWidth(body);
        UploadCaptured(root, handle, uploadWidth, uploadHeight, channels, pixels, services);
        previous = pixels;
        half ^= imageBytes;
    }
    scratch.Finish(scratch.bytes.RawSlot().controlAt08);
    return true;
}

bool LoadAnimationPicturesNativeAt447F60(Body& body, Cursor cursor,
    float version, Services& services)
{
    if (version == 0.0f) return LoadAnimationPicturesNativeAt446BD0(body, cursor, services);
    if (version == 0.1f) return LoadAnimationPicturesNativeAt4465C0(body, cursor, services);
    if (version == 1.0f) return LoadAnimationPicturesNativeAt446FB0(body, cursor, 0, services);
    if (version == 1.1f) return LoadAnimationPicturesNativeAt446FB0(body, cursor, 1, services);
    if (version == 2.0f) return LoadAnimationPicturesNativeAt447500(body, cursor, services);
    if (version == 3.0f) return LoadAnimationPicturesNativeAt4479D0(body, cursor, services);
    return false;
}

void LoadLegacyAnimationPicturesNativeAt445D2F(Body& body, Cursor cursor,
    std::uint8_t hasChannels, Services& services)
{
    const auto count = Take<std::uint16_t>(cursor);
    if (!count) throw 2;
    GraphicsSurfaceNativeScope surfaceScope(services.surfaces);
    ReplaceSurfaces(body, count, services);
    LoadRawFrames(body, cursor, count, hasChannels != 0, services);
}

AnimationFormatNativeRawInputScope::AnimationFormatNativeRawInputScope(ArchiveCodecInputMode* mode) noexcept
    : mode_(mode), previous_(mode ? *mode : ArchiveCodecInputMode::Bounded)
{
    if (mode_) *mode_ = ArchiveCodecInputMode::Raw;
}

AnimationFormatNativeRawInputScope::~AnimationFormatNativeRawInputScope() noexcept
{
    if (mode_) *mode_ = previous_;
}

AnimationFormatNativePictures::AnimationFormatNativePictures(Services& services) noexcept
    : services_(services)
{
}
void AnimationFormatNativePictures::LoadLegacyFramesAt445D2F(Body& body,
    const Byte*& cursor, std::uint8_t readChannels)
{
    LoadLegacyAnimationPicturesNativeAt445D2F(body, &cursor, readChannels, services_);
}
std::uint8_t AnimationFormatNativePictures::LoadPicture00At446BD0(Body& body, const Byte*& cursor)
{
    return LoadAnimationPicturesNativeAt446BD0(body, &cursor, services_);
}
std::uint8_t AnimationFormatNativePictures::LoadPicture01At4465C0(Body& body, const Byte*& cursor)
{
    return LoadAnimationPicturesNativeAt4465C0(body, &cursor, services_);
}
std::uint8_t AnimationFormatNativePictures::LoadPicture10Or11At446FB0(Body& body,
    const Byte*& cursor, std::uint8_t is11)
{
    return LoadAnimationPicturesNativeAt446FB0(body, &cursor, is11, services_);
}
std::uint8_t AnimationFormatNativePictures::LoadPicture20At447500(Body& body, const Byte*& cursor)
{
    return LoadAnimationPicturesNativeAt447500(body, &cursor, services_);
}
std::uint8_t AnimationFormatNativePictures::LoadPicture30At4479D0(Body& body, const Byte*& cursor)
{
    return LoadAnimationPicturesNativeAt4479D0(body, &cursor, services_);
}
}
