#include <Runtime/WaitMarkServicesRuntime.h>
#include <Runtime/CountDownWindowResources.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>

#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
WaitMarkStateSourceRuntime::WaitMarkStateSourceRuntime(Platform::KeyboardView keyboard,
    const UnSaveDataRuntime& unsave) noexcept : keyboard_(keyboard), unsave_(&unsave) {}
WaitMarkStateSourceRuntime::WaitMarkStateSourceRuntime(const Platform::IInputRuntime& input,
    const EnvironmentStartupRuntime& environment) noexcept : input_(&input), environment_(&environment) {}
Platform::KeyboardView WaitMarkStateSourceRuntime::Keyboard() const
{
    // 4873CC逐次取输入+18；不能在45E450构造时绑定尚不存在的Keyboard。
    const auto keyboard = input_ ? input_->KeyboardOrNull() : keyboard_;
    if (!keyboard) throw std::logic_error("WaitMark requires the current keyboard slot");
    return *keyboard;
}
const UnSaveDataRuntime& WaitMarkStateSourceRuntime::UnSaved() const
{
    if (!environment_) return *unsave_;
    const auto* body = static_cast<UnSaveDataRuntime*>(environment_->Archives().CaptureArchiveAt4180C0(2));
    if (!body)
        throw std::logic_error("WaitMark requires the current UnSaveData slot");
    return *body;
}
bool WaitMarkStateSourceRuntime::AutomaticMarkSelected() const
{
    // 47B056..47B082：Program环境slot2，DWORD+18的低byte bit1，未查许可bit0。
    return (UnSaved().FlagsAt18() & 2u) != 0;
}
bool WaitMarkStateSourceRuntime::AnyRuntimeTripletSecondByteSet() const
{
    // 4873F2..48740C：+5C/+60是action6的绑定区，每项3byte；+10选当前双缓冲。
    const auto& keyboard = Keyboard();
    if (keyboard.ActionCapacity() <= 6)
        throw std::out_of_range("WaitMark keyboard does not contain action6 storage");
    for (std::size_t i = 0; i < keyboard.BindingCount(6); ++i)
        if (keyboard.CurrentState(6, i) != 0) return true;
    return false;
}
bool WaitMarkStateSourceRuntime::SlotTwoField20BitOneSet() const
{
    // 旧接口Field20表示十进制20，即0x14；不是UnSave+0x20，也不是DrawManager slot2。
    return (UnSaved().FlagsAt14() & 2u) != 0;
}
WaitMarkResourceLoaderRuntime::WaitMarkResourceLoaderRuntime(CountDownWindowResources& resources) noexcept
    : resources_(resources) {}
bool WaitMarkResourceLoaderRuntime::Load(SpriteRuntime& sprite, const std::string& name, bool playFromStart)
    { return resources_.LoadSpriteAt4814F0(sprite, name, playFromStart); }

namespace
{
std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
    { return std::bit_cast<std::int32_t>(std::uint32_t(a) + std::uint32_t(b)); }
const RuntimeGraphicsSurface& Frame(const SpriteRuntime& sprite, std::size_t index)
{
    if (index < sprite.PrimarySurfaceCount()) return sprite.PrimarySurfaceAt(index);
    index -= sprite.PrimarySurfaceCount();
    for (std::size_t group = 0; group < sprite.AuxiliarySurfaceGroupCount(); ++group)
    {
        const auto count = sprite.AuxiliarySurfaceCount(group);
        if (index < count) return sprite.AuxiliarySurfaceAt(group, index);
        index -= count;
    }
    throw std::out_of_range("WaitMark 443580 frame not present");
}
}
WaitMarkRendererRuntime::WaitMarkRendererRuntime(IGraphicsWindowSurfaceApi& graphics) noexcept : graphics_(graphics) {}
std::int32_t WaitMarkRendererRuntime::DrawToScreen(const SpriteRuntime& sprite, const RECT& dirty)
    { return Render(sprite, &dirty, nullptr); }
std::int32_t WaitMarkRendererRuntime::DrawToSurface(const SpriteRuntime& sprite,
    GraphicsSurfaceDestination destination, const RECT* clip) { return Render(sprite, clip, &destination); }
std::int32_t WaitMarkRendererRuntime::Render(const SpriteRuntime& sprite, const RECT* clip,
    const GraphicsSurfaceDestination* destination)
{
    if (sprite.LogicalName().empty()) return 1; // 4768DF / 46B39F。
    const auto submit = [&](const RuntimeGraphicsSurface& frame, std::int32_t x, std::int32_t y)
    {
        if (destination)
        {
            const auto targetHandle = destination->Handle();
            if (targetHandle && frame.Handle())
                static_cast<void>(graphics_.SurfaceBlitRaw(targetHandle, frame.Handle(), x, y,
                    nullptr, nullptr, clip, 0));
        }
        else if (frame.Handle())
            static_cast<void>(graphics_.SurfaceDrawRaw(frame.Handle(), x, y, nullptr, nullptr, clip, 0));
    };
    switch (sprite.LoadMode())
    {
    case SpriteLoadMode::StaticWithoutAlpha:
    case SpriteLoadMode::StaticWithAlpha:
        submit(Frame(sprite, 0), sprite.X(), sprite.Y());
        break;
    case SpriteLoadMode::Animation:
        // 原每轮重读映射数量/元素；Graphics回调后不可保留跨轮的vector迭代器。
        for (std::size_t i = 0; i < sprite.CurrentFrameMappings().size(); ++i)
        {
            const auto mapping = sprite.CurrentFrameMappings()[i];
            if (mapping < 0) continue;
            const auto& frame = Frame(sprite, static_cast<std::size_t>(mapping));
            submit(frame, Add(sprite.X(), frame.OriginX()), Add(sprite.Y(), frame.OriginY()));
        }
        break;
    default: return 3; // 476999 / 46B45D，非通用失败回退。
    }
    return 0; // 476990 / 46B454，SurfaceDraw/Blt AL不参与返回。
}
}
