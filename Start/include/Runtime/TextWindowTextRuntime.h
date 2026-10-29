#pragma once

#include <Runtime/GraphicsTextRuntime.h>
#include <Runtime/Message2Runtime.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class LayerList;

// +70/+7C/+88 是三组独立引用；声明顺序使析构按 4100C0 的 body/voices/name 逆序。
// 空姓名引用与 bytes 为空的姓名对象不同，不能把 +70 简化成 bool。
struct TextWindowTextState final
{
    MessageTextCommandReference name;
    RuntimeCStringArrayReference voices;
    MessageTextCommandReference body;
    RECT contentRect{}; // +30
    RECT nameRect{}; // +40
    std::optional<std::int32_t> glyphCursorAt50;
    std::optional<std::uint8_t> playbackStartedAt54;
    // 4BB530 的逐字等待/全文完成后自动等待；构造并未初始化，不能默认给零。
    std::optional<std::int32_t> waitFramesAt58;
    std::optional<std::int32_t> autoWaitFramesAt5C;
    std::optional<std::uint8_t> skipAt60;
    std::optional<std::int32_t> formatIndexAt94;
    // 原构造未初始化这三个字节；由生产设置链显式赋值，服务不得默认补零。
    std::optional<std::uint8_t> shadowAt98;
    std::optional<std::uint8_t> renderAt99;
    std::optional<std::uint8_t> unreadAt9A;
    std::int32_t bodyOffsetX = 0; // +D8
    std::int32_t bodyOffsetY = 0; // +DC
    std::int32_t nameOffsetX = 0; // +E0
    std::int32_t nameOffsetY = 0; // +E4
};

struct TextWindowTextFontDescriptor final
{
    std::int32_t size = 0;
    std::int32_t weight = 0;
    std::int32_t characterSpacing = 0;
    std::int32_t lineSpacing = 0;
};

enum class TextWindowNameAlignment : std::int32_t
{
    Left = 0,
    CenterAtAnchor = 1,
    RightAtAnchor = 2,
    CenterInRegion = 3,
};

// 4172D0 返回的 64 字节记录的语义布局，不作为原二进制 ABI 使用。
struct TextWindowTextDescriptor final
{
    TextWindowTextFontDescriptor bodyFont; // +00..+0C
    std::int32_t bodyX = 0; // +10
    std::int32_t bodyY = 0; // +14
    TextWindowTextFontDescriptor nameFont; // +18..+24
    std::int32_t nameX = 0; // +28
    std::int32_t nameY = 0; // +2C
    std::int32_t nameAlignment = 0; // TextWindowNameAlignment；旧bool居中须显式转换为区域语义。
    std::int32_t nameNewlineMode = 0; // +34: 0 去 LF，1 保留，其他保留并上移
    std::uint8_t expandName = 0; // +38
    std::int32_t minimumHalfCharacters = 0; // +3C
};

std::span<const TextWindowTextDescriptor, 2> GameTextWindowNativeDescriptors() noexcept;

// 主线生产服务借用实际字体名、状态、save、颜色引用和 manager；核心不复制这些输入。
class ITextWindowTextServices
{
public:
    virtual ~ITextWindowTextServices() = default;
    // 每次重新求值 mess_win_type；越界返回 nullptr，不回退到第零项。
    [[nodiscard]] virtual const TextWindowTextDescriptor* ReadDescriptor() = 0;
    // 414BD0：完整创建字体。原调用者忽略 false，但不在此吞异常。
    [[nodiscard]] virtual bool PrepareFont(const TextWindowTextFontDescriptor& record,
        std::uint32_t color, const std::uint32_t* shadowOverride) = 0;
    [[nodiscard]] virtual std::uint32_t ReadBodyColor() = 0; // 418980
    [[nodiscard]] virtual const std::uint32_t* ReadNameColorOverride() = 0; // +B4
    [[nodiscard]] virtual const std::uint32_t* ReadNameShadowOverride() = 0; // +C0
};

// 所有对象均借用，必须长于本对象。仅绘制非空区间时要求 Surface 对象非空。
// 零 handle 仍交给 TextBlt；不能在构造 layout 时提前拒绝失败 owner 的空 Surface。
// 不处理 4C2740、readFlags、4BB530 或 +64，亦不自行替换 F040/F041 姓名占位符。
class TextWindowTextRuntime final
{
public:
    TextWindowTextRuntime(TextWindowTextState& state, RuntimeGraphicsSurface* surface,
        LayerList* parent, IGraphicsTextApi& text, IGraphicsTextGlyphApi& glyphs,
        ITextWindowTextServices& services) noexcept;

    // 42D5F0 的局部 catch(...) 返回 false，保留失败前已经写入的矩形和字形。
    [[nodiscard]] bool PrepareNameAt42D5F0();
    // 415150 无局部 catch；明确失败抛 int(1)，不回滚已完成的姓名布局。
    [[nodiscard]] bool PrepareTextAt415150();
    // 419090 无符号半开区间；不夹取长度、不修改 glyphCursorAt50。
    [[nodiscard]] bool BlitRangeAt419090(std::uint32_t begin, std::uint32_t end);

private:
    TextWindowTextState& state_;
    RuntimeGraphicsSurface* surface_;
    LayerList* parent_;
    IGraphicsTextApi& text_;
    IGraphicsTextGlyphApi& glyphs_;
    ITextWindowTextServices& services_;
};
}
