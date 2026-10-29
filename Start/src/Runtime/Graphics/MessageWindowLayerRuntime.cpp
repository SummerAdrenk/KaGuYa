#include <Runtime/MessageWindowLayerRuntime.h>
#include <Runtime/MessageWindowConstructionRuntime.h>
#include <Logging/Logger.h>

#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
class NullStyleReference final : public std::logic_error
{
public:
    NullStyleReference() : std::logic_error("CMesWin style factory returned a null reference") {}
};

class NullConfigReference final : public std::logic_error
{
public:
    NullConfigReference() : std::logic_error("CMesWin construction returned a null config reference") {}
};

std::int32_t Add32(std::int32_t left, std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(left) +
        static_cast<std::uint32_t>(right));
}

std::int32_t Sub32(std::int32_t left, std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(left) -
        static_cast<std::uint32_t>(right));
}
}

MessageWindowLayerRuntime::MessageWindowLayerRuntime(LayerList* parent,
    IMessageWindowConstructionServices& construction, IMessageWindowLayerState& state)
    : FrameLayerItem(parent), state_(state)
{
    // 459AF4 注册在外层 try 之前；配置初始化不能搬到基类构造之前。
    const char* phase = "type1 allocation";
    try
    {
        type1_ = construction.AllocateConfig(MessageWindowConfigType::Type1);
        if (!type1_) throw NullConfigReference();
        phase = "type2 allocation";
        type2_ = construction.AllocateConfig(MessageWindowConfigType::Type2);
        if (!type2_) throw NullConfigReference();
        phase = "config lookup";
        construction.ApplyConfigOverrides(*type1_, *type2_);
        // 45A228..45A23C：仅走到此处才关闭图层、初始化 selector/name。
        ResetAt47ED90();
        bounds_ = {};
        phase = "style construction";
        for (std::uint32_t index = 0; index < Sample18StyleCount; ++index)
            AppendStyle(construction.CreateStyle(index, type1_, type2_));
    }
    catch (const NullConfigReference&)
    {
        // null 服务返回违反契约，不用原 catch 掩盖非法内存访问。
        type2_.reset();
        type1_.reset();
        ReleaseStyles();
        throw;
    }
    catch (const NullStyleReference&)
    {
        type2_.reset();
        type1_.reset();
        ReleaseStyles();
        throw;
    }
    catch (...)
    {
        ClearAfterConstructionFailure(phase);
    }
}

MessageWindowLayerRuntime::MessageWindowLayerRuntime(LayerList* parent,
    IMessageWindowStyleFactory& styles, IMessageWindowLayerState& state)
    : FrameLayerItem(parent), state_(state)
{
    ResetAt47ED90();
    bounds_ = {};
    try
    {
        // 45A242..45A429 固定遍历样本 18 的两个描述符，不按 params 槽位数猜测。
        for (std::uint32_t index = 0; index < Sample18StyleCount; ++index)
            AppendStyle(styles.CreateStyle(index));
    }
    catch (const NullStyleReference&)
    {
        // 原路径解引用 null 是非法内存访问，不伪造为 C++ 装载失败。
        ReleaseStyles();
        throw;
    }
    catch (...)
    {
        ClearAfterConstructionFailure("style construction (post-config entry)");
    }
}

MessageWindowLayerRuntime::~MessageWindowLayerRuntime()
{
    // 468D00：先外层 type2/type1，再正序 styles，最后基类注销。
    // pane/style 自己的引用可使 config 活过这两次外层释放。
    type2_.reset();
    type1_.reset();
    ReleaseStyles();
}

void MessageWindowLayerRuntime::AppendStyle(std::unique_ptr<MessageWindowStyleRuntime> style)
{
    if (!style) throw NullStyleReference();
    RECT combined{};
    if (style->body)
    {
        const RECT rect = style->body->BoundsAtVslot04();
        ::UnionRect(&combined, &combined, &rect);
    }
    if (style->name)
    {
        const RECT rect = style->name->BoundsAtVslot04();
        ::UnionRect(&combined, &combined, &rect);
    }
    // 45A39B..45A3F6：先合并总矩形，再扩容/追加；失败时由外层 catch 清空。
    ::UnionRect(&bounds_, &bounds_, &combined);
    styles_.push_back(std::move(style));
}

void MessageWindowLayerRuntime::ClearAfterConstructionFailure(const char* phase)
{
    // EH 5EAB08 state 4..27 -> 45A46C：保留配置/注册/当前标志，只有矩形和样式被清空。
    bounds_ = {};
    ReleaseStyles();
    KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Error,
        "CMesWin 45A46C: %s failed; retained configs/registration/state, cleared bounds and styles", phase);
}

void MessageWindowLayerRuntime::ReleaseStyles() noexcept
{
    // 468D00 -> 4622F0 正序释放已构造的样式引用，再由基类注销。
    for (auto& style : styles_) style.reset();
    styles_.clear();
}

void MessageWindowLayerRuntime::RefreshStyleAt5325F0(const std::int32_t selector)
{
    const auto refresh = [](MessageWindowStyleRuntime& style) {
        // 5326F0：body 再 name，空 pane 跳过，返回值不门控后续刷新。
        if (style.body) style.body->RefreshColorsAtVslot08();
        if (style.name) style.name->RefreshColorsAtVslot08();
    };
    if (selector < 0)
    {
        for (auto& style : styles_)
        {
            if (!style) throw std::logic_error("CMesWin refresh has null style");
            refresh(*style);
        }
    }
    else
    {
        if (static_cast<std::size_t>(selector) >= styles_.size()) return;
        auto& style = styles_[static_cast<std::size_t>(selector)];
        if (!style) throw std::logic_error("CMesWin refresh has null style");
        refresh(*style);
    }
    // 5326C6：合法 selector / 全样式刷新后，只有可见时提交整体 bounds。
    if (Enabled()) InvalidateBounds();
}

RECT MessageWindowLayerRuntime::BoundsAt479DB0() const noexcept { return bounds_; }
std::int32_t MessageWindowLayerRuntime::SelectedStyleAt30() const
{
    if (!selected_) throw std::logic_error("CMesWin +30 is uninitialized after early construction failure");
    return *selected_;
}
bool MessageWindowLayerRuntime::RememberedNameAt34() const
{
    if (!rememberedName_) throw std::logic_error("CMesWin +34 is uninitialized after early construction failure");
    return *rememberedName_;
}
bool MessageWindowLayerRuntime::SelectionInitialized() const noexcept
    { return selected_.has_value() && rememberedName_.has_value(); }
const MessageWindowConfigRuntime* MessageWindowLayerRuntime::ConfigType1At38() const noexcept
    { return type1_.get(); }
const MessageWindowConfigRuntime* MessageWindowLayerRuntime::ConfigType2At44() const noexcept
    { return type2_.get(); }
std::size_t MessageWindowLayerRuntime::StyleCount() const noexcept { return styles_.size(); }

MessageWindowColors MessageWindowLayerRuntime::ReadSelectedStyleColorsAt414BD0() const
{
    // 414C74..414CF5：非法 selector 改用第0样式，再读取样式自己持有的 config。
    // 不能直接读外层 type1/type2，否则会掩盖样式构造失败后的空引用。
    const auto selected = SelectedStyleAt30();
    const auto index = selected < 0 || static_cast<std::size_t>(selected) >= styles_.size()
        ? std::size_t{0} : static_cast<std::size_t>(selected);
    const auto& style = styles_.at(index);
    if (!style || !style->config) throw std::logic_error("CTextWin shadow requires a CMesWin style config");
    return style->config->Read();
}

MessageWindowStyleRuntime* MessageWindowLayerRuntime::SelectedStyle() const
{
    const auto selected = SelectedStyleAt30();
    if (selected < 0 || static_cast<std::size_t>(selected) >= styles_.size()) return nullptr;
    return styles_[static_cast<std::size_t>(selected)].get();
}

void MessageWindowLayerRuntime::InvalidateBounds()
{
    auto* const parent = RegisteredLayerList();
    if (!parent)
        throw std::logic_error("CMesWin invalidation requires the parent LayerList");
    parent->DirtyRects().Add(bounds_);
}

std::uint8_t MessageWindowLayerRuntime::SetRequestedEnabledAt487030(
    std::uint8_t requested, std::int32_t)
{
    // 即使调用者请求隐藏，原版也先读取脚本和 419290；-1 才短路第二项。
    if (state_.ActiveScriptField60() == -1 || !state_.MessageWindowAllowedAt419290())
        requested = 0;
    if (requested != 0)
    {
        const auto index = state_.EvaluateMessWinTypeAt4C73A0();
        if (SelectedStyleAt30() != index)
        {
            InvalidateBounds();
            selected_ = index;
            rememberedName_ = state_.TextWindowHasNameAt70();
        }
        else if (SelectedStyle())
        {
            InvalidateBounds();
        }
        if (Enabled()) return 0;
        SetEnabled(true);
        InvalidateBounds();
        return 1;
    }
    if (!Enabled()) return 0;
    SetEnabled(false);
    InvalidateBounds();
    return 1;
}

void MessageWindowLayerRuntime::ResetAt47ED90() noexcept
{
    SetEnabled(false);
    selected_ = -1;
    rememberedName_ = false;
}

void MessageWindowLayerRuntime::PrepareNameAt46B470(MessageWindowStyleRuntime& style)
{
    auto& pane = *style.name;
    pane.repeat.rows = 1;
    pane.repeat.columns = 1;
    if (pane.TypeAtVslot00() != 1) return;

    const SIZE tile = pane.BaseTileSizeAt47DFE0();
    const RECT baseline = style.nameBaseline;
    const RECT name = state_.TextWindowNameRectAt40();
    auto bottom = static_cast<std::int32_t>(baseline.bottom);
    const auto height = Sub32(name.bottom, name.top);
    // 46B5C0..46B631：先扩行后扩列，保留 ADD/SUB/INC 的低 32 位。
    while (Sub32(bottom, baseline.top) < height)
    {
        if (pane.repeat.rows >= pane.repeat.maximumRows) break;
        bottom = Add32(bottom, tile.cy);
        pane.repeat.rows = Add32(pane.repeat.rows, 1);
    }
    auto right = static_cast<std::int32_t>(baseline.right);
    const auto width = Sub32(name.right, name.left);
    while (Sub32(right, baseline.left) < width)
    {
        if (pane.repeat.columns >= pane.repeat.maximumColumns) break;
        right = Add32(right, tile.cx);
        pane.repeat.columns = Add32(pane.repeat.columns, 1);
    }
}

void MessageWindowLayerRuntime::Draw(const RECT& dirty)
{
    auto* const style = SelectedStyle();
    if (!style) return;
    if (style->body) style->body->DrawAtVslot0C(dirty);
    if (style->name && state_.TextWindowHasNameAt70())
    {
        PrepareNameAt46B470(*style);
        style->name->DrawAtVslot0C(dirty);
    }
}

void MessageWindowLayerRuntime::CompositeToSurface(
    GraphicsSurfaceDestination destination, const RECT* clip)
{
    auto* const style = SelectedStyle();
    if (!style) return;
    if (style->body) style->body->CompositeAtVslot10(destination, clip);
    if (style->name && state_.TextWindowHasNameAt70())
    {
        PrepareNameAt46B470(*style);
        style->name->CompositeAtVslot10(destination, clip);
    }
}

void MessageWindowLayerRuntime::AdvanceOwnedFrame()
{
    // 46B230 为单条 RETN，CMesWin 本身不推进动画或提交失效矩形。
}
}
