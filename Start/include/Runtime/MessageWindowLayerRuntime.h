#pragma once

#include <Runtime/DrawManagerLayerRuntime.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class IMessageWindowColorConfig;
struct MessageWindowColors;
class MessageWindowConfigRuntime;
class IMessageWindowConstructionServices;
struct MessageWindowRepeatState final
{
    std::int32_t columns = 1;       // variableWindow +2C
    std::int32_t maximumColumns = 0; // +30
    std::int32_t rows = 1;          // +34
    std::int32_t maximumRows = 0;   // +38
};

// normalWindow / variableWindow 的现代边界，不是原 DLL 的对象布局。
// 具体实现位于 MessageWindowPaneRuntime；接口没有默认成功路径。
class IMessageWindowPane
{
public:
    virtual ~IMessageWindowPane() = default;
    [[nodiscard]] virtual std::int32_t TypeAtVslot00() const = 0;
    [[nodiscard]] virtual RECT BoundsAtVslot04() const = 0;
    [[nodiscard]] virtual SIZE BaseTileSizeAt47DFE0() const = 0;
    virtual void RefreshColorsAtVslot08() = 0;
    virtual void DrawAtVslot0C(const RECT& dirty) = 0;
    virtual void CompositeAtVslot10(GraphicsSurfaceDestination destination, const RECT* clip) = 0;

    // 46B470 在检查类型前写 +34/+2C。用独立状态保留顺序，不能照搬对
    // 0x2C 字节 normalWindow 尾外地址的写入；普通面板不得消费这些计数。
    MessageWindowRepeatState repeat;
};

struct MessageWindowStyleRuntime final
{
    std::unique_ptr<IMessageWindowPane> body;
    std::unique_ptr<IMessageWindowPane> name;
    RECT nameBaseline{}; // messWindow +24；45F040 仅在可变姓名框分支复制
    // 465480：先释放 messWindow 自身的 config，再释放 name/body。
    std::shared_ptr<const IMessageWindowColorConfig> config;
};

class IMessageWindowStyleFactory
{
public:
    virtual ~IMessageWindowStyleFactory() = default;
    // 459AB0:45A274，参数是 601408 的描述符索引；返回空引用会明确失败。
    [[nodiscard]] virtual std::unique_ptr<MessageWindowStyleRuntime> CreateStyle(
        std::uint32_t descriptorIndex) = 0;
};

class IMessageWindowLayerState
{
public:
    virtual ~IMessageWindowLayerState() = default;
    [[nodiscard]] virtual std::int32_t ActiveScriptField60() const = 0;
    [[nodiscard]] virtual bool MessageWindowAllowedAt419290() const = 0;
    [[nodiscard]] virtual std::int32_t EvaluateMessWinTypeAt4C73A0() = 0;
    [[nodiscard]] virtual bool TextWindowHasNameAt70() const = 0;
    [[nodiscard]] virtual RECT TextWindowNameRectAt40() const = 0;
};

// CMesWin 459AB0 的配置/样式所有权与虚槽状态；不是原对象二进制布局。
// 生产使用 construction 服务入口，根 TextWin 与八槽 owner 仍须另行装配。
class MessageWindowLayerRuntime final : public FrameLayerItem, public ILayerPrepareTarget, public ILayerResetTarget
{
public:
    [[nodiscard]] std::uint8_t SetRequestedEnabledAtVslot10(
        std::uint8_t requested, std::int32_t secondArgument) override
        { return SetRequestedEnabledAt487030(requested, secondArgument); }
    static constexpr std::uint32_t Sample18StyleCount = 2;
    MessageWindowLayerRuntime(LayerList* parent, IMessageWindowConstructionServices& construction,
        IMessageWindowLayerState& state);
    // 显式后半段入口：仅 45A228 之后，不声称具有配置分配失败语义。
    MessageWindowLayerRuntime(LayerList* parent, IMessageWindowStyleFactory& styles,
        IMessageWindowLayerState& state);
    ~MessageWindowLayerRuntime() override;

    [[nodiscard]] RECT BoundsAt479DB0() const noexcept;
    [[nodiscard]] std::int32_t SelectedStyleAt30() const;
    [[nodiscard]] bool RememberedNameAt34() const;
    [[nodiscard]] bool SelectionInitialized() const noexcept;
    [[nodiscard]] const MessageWindowConfigRuntime* ConfigType1At38() const noexcept;
    [[nodiscard]] const MessageWindowConfigRuntime* ConfigType2At44() const noexcept;
    [[nodiscard]] std::size_t StyleCount() const noexcept;
    [[nodiscard]] MessageWindowColors ReadSelectedStyleColorsAt414BD0() const;
    [[nodiscard]] std::uint8_t SetRequestedEnabledAt487030(
        std::uint8_t requested, std::int32_t unusedSecondArgument);
    void ResetAt47ED90() noexcept;
    void ResetOwnedLayerAtVslot18(IGraphicsWindowSurfaceApi&) override { ResetAt47ED90(); }
    void RefreshStyleAt5325F0(std::int32_t selector);

    void Draw(const RECT& dirty) override;
    void CompositeToSurface(GraphicsSurfaceDestination destination, const RECT* clip) override;
    void AdvanceOwnedFrame() override;

private:
    [[nodiscard]] MessageWindowStyleRuntime* SelectedStyle() const;
    void AppendStyle(std::unique_ptr<MessageWindowStyleRuntime> style);
    void ClearAfterConstructionFailure(const char* phase);
    void PrepareNameAt46B470(MessageWindowStyleRuntime& style);
    void InvalidateBounds();
    void ReleaseStyles() noexcept;

    RECT bounds_{};
    std::vector<std::unique_ptr<MessageWindowStyleRuntime>> styles_;
    // 45A228 以前没有原初始化写入。不能把失败路径伪装成正常的 -1/false。
    std::optional<std::int32_t> selected_;
    std::optional<bool> rememberedName_;
    std::shared_ptr<MessageWindowConfigRuntime> type1_, type2_;
    IMessageWindowLayerState& state_;
};
}
