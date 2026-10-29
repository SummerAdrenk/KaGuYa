#include <Runtime/FreeScrollRuntime.h>

#include <Runtime/ParamsRuntimeProfile.h>
#include <Runtime/ParamsRuntimeRoot.h>

#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const ParamsGameSystem* RootStartupConfigGameSystemSource::CaptureGameSystemAt418480() const
{
    return params_.GetOrCreateAt418480().GameSystem().get();
}

FreeScrollRuntime::FreeScrollRuntime(FreeScrollDependencies dependencies)
    : dependencies_(dependencies), input_(dependencies.mouse, dependencies.keyboard)
{
    const SIZE size = dependencies_.screenSize.ReadScreenSizeAt47CC00();
    state_.viewportAt1C = {0, 0, size.cx, size.cy};
}

FreeScrollRuntime::~FreeScrollRuntime() { ReleaseEntriesForward(); }

const FreeScrollState& FreeScrollRuntime::State() const noexcept { return state_; }
std::size_t FreeScrollRuntime::ScrollCount() const noexcept { return entries_.size(); }
FreeScrollEntryView FreeScrollRuntime::ScrollAt(const std::size_t index) const
{
    const Entry& entry = *entries_.at(index);
    return {&entry.pattern, entry.unionOrigin, entry.main.get(), entry.processed.get()};
}

LayerWindowRuntime& FreeScrollRuntime::WindowZero() const
{
    auto* window = dynamic_cast<LayerWindowRuntime*>(dependencies_.manager.LayerAt418C10(0));
    if (!window) throw std::logic_error("FreeScroll requires manager slot zero LayerWindow");
    return *window;
}

PatternLayerRuntime* FreeScrollRuntime::PatternAt(const std::size_t index) const
{
    auto* layer = WindowZero().PatternLayerAt(index);
    if (!layer) return nullptr;
    auto* pattern = dynamic_cast<PatternLayerRuntime*>(layer);
    if (!pattern) throw std::logic_error("FreeScroll requires a production Pattern layer");
    return pattern;
}

Platform::CursorView FreeScrollRuntime::Cursor() const
{
    if (!dependencies_.cursor) throw std::logic_error("FreeScroll requires a cursor owner");
    return *dependencies_.cursor;
}

void FreeScrollRuntime::ReleaseEntriesForward() noexcept
{
    for (auto& entry : entries_) entry.reset();
    entries_.clear();
}

bool FreeScrollRuntime::BeginAt4ADF70()
{
    state_.axesAt0C = 0;
    bool missingFirst = true;
    // 逐项读取配置数量，不以 min 截断，也不在产生当前项副作用前预检后续槽。
    for (std::uint32_t i = 0; ; ++i)
    {
        const auto* const game = dependencies_.gameSystem.CaptureGameSystemAt418480();
        if (!game) throw std::logic_error("FreeScroll Begin GameSystem body is null");
        const auto count = static_cast<std::uint32_t>(game->startupConfigBytes.size());
        if (std::bit_cast<std::int32_t>(i) >= std::bit_cast<std::int32_t>(count)) break;
        auto* pattern = PatternAt(i);
        if (!pattern) throw std::logic_error("FreeScroll Begin encountered a null Pattern");
        if (!pattern->Enabled()) continue;
        if (i == 0) missingFirst = false;
        entries_.push_back(std::make_unique<Entry>(*pattern));
    }
    if (missingFirst) return false;

    const auto requireFirst = [this]() -> PatternLayerRuntime& {
        auto* pattern = PatternAt(0);
        if (!pattern) throw std::logic_error("FreeScroll Pattern zero disappeared");
        return *pattern;
    };
    const auto& first = requireFirst();
    state_.xAt14 = Sub(0, first.X());
    state_.yAt18 = Sub(0, first.Y());
    const auto bounds = requireFirst().ScreenSize();
    const RECT extent = LocalUnionAt4ADDE0(requireFirst());
    if (Sub(extent.right, extent.left) > bounds.width) state_.axesAt0C |= 1u;
    if (Sub(extent.bottom, extent.top) > bounds.height) state_.axesAt0C |= 2u;
    state_.draggingAt44 = 0;
    state_.limitsAt2C = {};
    if (state_.axesAt0C == 0) return true;
    state_.limitsAt2C = {extent.left, extent.top,
        Sub(extent.right, bounds.width), Sub(extent.bottom, bounds.height)};
    for (std::size_t i = 0; i < entries_.size(); ++i) CreateCanvasAt4AE574(*entries_[i]);
    return true;
}

void FreeScrollRuntime::UpdateMouse()
{
    if (!state_.draggingAt44)
    {
        if (input_.ReadMousePressAtVslot14())
        {
            state_.lastPointerAt48 = input_.ReadPointerAtVslot08();
            state_.draggingAt44 = 1;
        }
        return;
    }
    if (input_.ReadMouseReleaseAtVslot18())
        state_.draggingAt44 = 0;
    else
    {
        const POINT point = input_.ReadPointerAtVslot08();
        RECT client{};
        dependencies_.windowApi.ReadClientRectangle(dependencies_.window, client);
        if (!dependencies_.windowApi.Contains(client, point)) state_.draggingAt44 = 0;
    }
    if (!state_.draggingAt44) return;

    const POINT point = input_.ReadPointerAtVslot08();
    const auto move = [](std::int32_t& value, const LONG current, const LONG previous,
                         const LONG minimum, const LONG maximum) {
        const auto difference = Sub(current, previous);
        const auto distance = difference < 0 ? Sub(0, difference) : difference;
        if (current > previous)
        {
            value = Sub(value, distance);
            if (value < minimum) value = minimum;
        }
        else if (current < previous)
        {
            value = Add(value, distance);
            if (value > maximum) value = maximum;
        }
    };
    move(state_.xAt14, point.x, state_.lastPointerAt48.x,
        state_.limitsAt2C.left, state_.limitsAt2C.right);
    move(state_.yAt18, point.y, state_.lastPointerAt48.y,
        state_.limitsAt2C.top, state_.limitsAt2C.bottom);
    state_.lastPointerAt48 = point;
}

void FreeScrollRuntime::UpdateKeyboard()
{
    if (input_.ReadPressedAtVslot04(1) || input_.ReadPressedAtVslot04(2) ||
        input_.ReadPressedAtVslot04(3) || input_.ReadPressedAtVslot04(4))
    {
        state_.repeatCountdownAt40 = 0;
        state_.repeatWaitAt3C = 10;
        state_.draggingAt44 = 0;
    }
    std::int32_t direction = -1;
    if (input_.ReadHeldAtVslot00(1)) direction = 0;
    else if (input_.ReadHeldAtVslot00(2)) direction = 1;
    else if (input_.ReadHeldAtVslot00(3)) direction = 2;
    else if (input_.ReadHeldAtVslot00(4)) direction = 3;
    else state_.repeatCountdownAt40 = 1;

    if (!state_.repeatCountdownAt40)
        throw std::logic_error("FreeScroll +40 has not been initialized");
    if (*state_.repeatCountdownAt40 != 0)
    {
        state_.repeatCountdownAt40 = Sub(*state_.repeatCountdownAt40, 1);
        return;
    }
    switch (direction)
    {
    case 0:
        state_.yAt18 = Sub(state_.yAt18, state_.stepAt10);
        if (state_.yAt18 < state_.limitsAt2C.top) state_.yAt18 = state_.limitsAt2C.top;
        break;
    case 1:
        state_.yAt18 = Add(state_.yAt18, state_.stepAt10);
        if (state_.yAt18 > state_.limitsAt2C.bottom) state_.yAt18 = state_.limitsAt2C.bottom;
        break;
    case 2:
        state_.xAt14 = Sub(state_.xAt14, state_.stepAt10);
        if (state_.xAt14 < state_.limitsAt2C.left) state_.xAt14 = state_.limitsAt2C.left;
        break;
    case 3:
        state_.xAt14 = Add(state_.xAt14, state_.stepAt10);
        if (state_.xAt14 > state_.limitsAt2C.right) state_.xAt14 = state_.limitsAt2C.right;
        break;
    default:
        break;
    }
    // 原读取 +3C 晚于坐标写入；未知值失败也不能回滚已发生的移动。
    if (!state_.repeatWaitAt3C) throw std::logic_error("FreeScroll +3C has not been initialized");
    state_.repeatCountdownAt40 = *state_.repeatWaitAt3C;
    state_.repeatWaitAt3C = 0;
}

void FreeScrollRuntime::UpdateWheel()
{
    const bool vertical = (state_.axesAt0C & 2u) != 0;
    if (!vertical && (state_.axesAt0C & 1u) == 0) return;
    auto& value = vertical ? state_.yAt18 : state_.xAt14;
    const auto wheel = input_.ReadWheelAtVslot0C();
    if (wheel < 0) value = Add(value, state_.stepAt10);
    else if (wheel > 0) value = Sub(value, state_.stepAt10);
    if (wheel != 0) state_.draggingAt44 = 0;
    const LONG minimum = vertical ? state_.limitsAt2C.top : state_.limitsAt2C.left;
    const LONG maximum = vertical ? state_.limitsAt2C.bottom : state_.limitsAt2C.right;
    if (value < minimum) value = minimum;
    if (value > maximum) value = maximum;
}

bool FreeScrollRuntime::UpdateAt4BDD00()
{
    if (state_.axesAt0C == 0) return false;
    const auto oldX = state_.xAt14;
    const auto oldY = state_.yAt18;
    static_cast<void>(Cursor().LoadAndApply(
        MAKEINTRESOURCEA(state_.draggingAt44 ? 0xD0 : 0xCF), false, false));
    UpdateMouse();
    if (oldX == state_.xAt14 && oldY == state_.yAt18) UpdateKeyboard();
    if (oldX == state_.xAt14 && oldY == state_.yAt18) UpdateWheel();
    RenderAt4AF200(oldX != state_.xAt14 || oldY != state_.yAt18);
    return true;
}

void FreeScrollRuntime::RestorePatternsAt4C4330()
{
    WindowZero().RestorePatternsAt4C4330(dependencies_.drawing);
}

void FreeScrollRuntime::EndAt4AC970()
{
    for (std::uint32_t i = 0; ; ++i)
    {
        const auto* const game = dependencies_.gameSystem.CaptureGameSystemAt418480();
        if (!game) throw std::logic_error("FreeScroll End GameSystem body is null");
        const auto count = static_cast<std::uint32_t>(game->startupConfigBytes.size());
        if (std::bit_cast<std::int32_t>(i) >= std::bit_cast<std::int32_t>(count)) break;
        auto* pattern = PatternAt(i);
        if (!pattern) continue;
        const auto& main = pattern->MainSurfaceAt18();
        AddDirty(pattern->DirtyRects(), {0, 0, main.Width(), main.Height()});
    }
    RestorePatternsAt4C4330();
    if (state_.axesAt0C != 0)
        AddDirty(dependencies_.manager.RegisteredLayers().DirtyRects(), state_.viewportAt1C);
    ReleaseEntriesForward();
    state_.axesAt0C = 0;
    static_cast<void>(Cursor().RestoreSystemArrow());
}
}
