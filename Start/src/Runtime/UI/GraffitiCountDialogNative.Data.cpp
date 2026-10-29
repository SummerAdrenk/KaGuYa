#include "GraffitiCountDialogNative.Internal.h"
#include <Runtime/GraphicsSurfaceRuntime.h>

namespace KaGuYa::Start::Runtime
{
using namespace GraffitiCountDialogNativeDetail;
namespace
{
// 原PE资源名的SJIS字节，不经源码编码/UTF-8转换。
#define GRAFFITI_NAME "vrm:\\\x93\xC1\x95\xCA\x83\x81\x83\x6A\x83\x85\x81\x5B\x8C\xF8\x89\xCA_"
constexpr const char* backgrounds[]{
    GRAFFITI_NAME "\x83\x78\x81\x5B\x83\x58.alp",
    GRAFFITI_NAME "\x90\x65\x96\xA7\x93\x78.alp",
    GRAFFITI_NAME "\x88\xCB\x91\xB6\x93\x78.alp",
    GRAFFITI_NAME "\x83\x5F\x83\x43\x83\x47\x83\x62\x83\x67\x8C\xF8\x89\xCA\x94\xAD\x90\xB6.alp"};
constexpr const char* digits[]{
    GRAFFITI_NAME "0.alp", GRAFFITI_NAME "1.alp", GRAFFITI_NAME "2.alp",
    GRAFFITI_NAME "3.alp", GRAFFITI_NAME "4.alp", GRAFFITI_NAME "5.alp",
    GRAFFITI_NAME "6.alp", GRAFFITI_NAME "7.alp", GRAFFITI_NAME "8.alp",
    GRAFFITI_NAME "9.alp", GRAFFITI_NAME "\x81\x7C.alp"};
constexpr char trainingName[] = GRAFFITI_NAME "\x83\x67\x83\x8C\x81\x5B\x83\x6A\x83\x93\x83\x4F"
    "LV\x83\x41\x83\x62\x83\x76.alp";
constexpr GuiStaticDescriptorNative24h training{1, {trainingName, nullptr, nullptr,
    trainingName, trainingName, nullptr, nullptr}, nullptr}; // 原609694。
#undef GRAFFITI_NAME
const Slot& At(const GuiControlSurfaceReferences& vector, std::int32_t index) noexcept
{
    // 原不读end/count；负值和越界不能被钳位或替换成0号资源。
    const auto address = reinterpret_cast<std::uintptr_t>(Read(vector.begin)) + static_cast<std::uint32_t>(index) * 12u;
    return *reinterpret_cast<const Slot*>(address);
}
void ClearCanvas(Common& body)
{
    auto* canvas = Resolve<GraphicsSurfaceNativeBody>(body.canvasAt6C);
    const auto handle = Read(canvas->handleAt04);
    if (!handle) return;
    const auto color = Read(canvas->clearColorAt20); // 4143C8/D3在4182F0之前固定。
    auto& api = RequireGuiControlGraphicsSource().CompositionAt4182F0();
    api.ClearSurface(handle, color, nullptr);
}
void Blit(Common& body, GraphicsSurfaceNativeBody* source, POINT position)
{
    auto* canvas = Resolve<GraphicsSurfaceNativeBody>(body.canvasAt6C);
    const auto targetHandle = Read(canvas->handleAt04);
    if (!targetHandle) return;
    const auto sourceHandle = Read(source->handleAt04);
    if (!sourceHandle) return;
    auto& api = RequireGuiControlGraphicsSource().CompositionAt4182F0();
    static_cast<void>(api.BlitSurface(targetHandle, sourceHandle, position.x, position.y, nullptr, nullptr, nullptr, 0));
}
POINT CanvasOrigin(Common& body) noexcept
{
    auto* canvas = Resolve<GraphicsSurfaceNativeBody>(body.canvasAt6C);
    return {Read(canvas->originXAt10), Read(canvas->originYAt14)};
}
void BlitBackground(Common& body, std::int32_t index, POINT origin)
{
    const auto& slot = At(body.backgroundsAt78, index);
    auto* source = Resolve<GraphicsSurfaceNativeBody>(slot);
    const auto x = Read(source->originXAt10), y = Read(source->originYAt14);
    source = Resolve<GraphicsSurfaceNativeBody>(slot);
    Blit(body, source, {Sub(x, origin.x), Sub(y, origin.y)});
}
void BlitDigit(Common& body, std::int32_t digit, POINT absolute, POINT origin)
{
    auto* source = Resolve<GraphicsSurfaceNativeBody>(At(body.digitsAt84, digit));
    Blit(body, source, {Sub(absolute.x, origin.x), Sub(absolute.y, origin.y)});
}
}
namespace GraffitiCountDialogNativeDetail
{
const GuiStaticDescriptorNative24h& WeightTrainingDescriptor() noexcept { return training; }
const char* BackgroundName(bool weight, std::uint32_t index) noexcept
{ return backgrounds[weight && index == 1 ? 3 : index]; }
const char* DigitName(std::uint32_t index) noexcept { return digits[index]; }
}
void RefreshGraffitiLikableAt528770(Likable& body, std::int32_t value)
{
    auto& common = body.common;
    ClearCanvas(common);
    const auto origin = CanvasOrigin(common);
    BlitBackground(common, 0, origin);
    auto* selected = Resolve<GraphicsSurfaceNativeBody>(At(common.backgroundsAt78, Read(body.categoryAt90)));
    Blit(common, selected, {Sub(592, origin.x), Sub(298, origin.y)});
    const auto hundreds = value / 100;
    if (hundreds) BlitDigit(common, hundreds, {551,349}, origin);
    const auto remainder = Sub(value, hundreds * 100);
    const auto tens = remainder / 10;
    if (hundreds || tens) BlitDigit(common, tens, {587,349}, origin);
    BlitDigit(common, Sub(remainder, tens * 10), {623,349}, origin);
}
void RefreshGraffitiWeightAt528A80(Weight& body, std::int32_t value)
{
    auto& common = body.common;
    ClearCanvas(common);
    const auto origin = CanvasOrigin(common);
    BlitBackground(common, 0, origin);
    BlitBackground(common, 1, origin);
    // 减号使用自身原点，不是固定digit坐标。
    if (value)
    {
        const auto& slot = At(common.digitsAt84, 10);
        auto* minus = Resolve<GraphicsSurfaceNativeBody>(slot);
        const POINT point{Sub(Read(minus->originXAt10), origin.x), Sub(Read(minus->originYAt14), origin.y)};
        Blit(common, Resolve<GraphicsSurfaceNativeBody>(slot), point);
    }
    const auto tens = value / 10;
    // 528CA0先个位，再528D0D十位；即便0也绘制两位。
    BlitDigit(common, Sub(value, tens * 10), {816,349}, origin);
    BlitDigit(common, tens, {770,349}, origin);
}
void SetGraffitiWeightWidgetsAt5398B0(Weight& body, std::int32_t mode)
{
    for (std::int32_t event = 0; event != 2; ++event)
    {
        Slot selected;
        auto& dialog = body.common.dialog;
        Read(dialog.vptrAt00)->findChildAt14(&dialog, &selected, event);
        RefUnwind lifetime{selected};
        auto* child = Resolve<GuiControlNativeBody50h>(selected);
        Read(child->vptrAt00)->writeEnabledAt4C(child, mode == event + 1);
        lifetime.armed = false;
        FinishCurrentWithoutClearing(selected);
    }
}
}
