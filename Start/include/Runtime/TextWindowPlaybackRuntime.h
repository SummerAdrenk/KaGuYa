#pragma once
#include <Platform/InputViews.h>

#include <Runtime/GuiInputStateRuntime.h>
#include <Runtime/TextWindowLayerRuntime.h>

namespace KaGuYa::Start::Runtime
{
class SaveDataEnvironmentArchive;
class UnSaveDataRuntime;
class ClickInfoArchive;

class ITextWindowPlaybackServices
{
public:
    virtual ~ITextWindowPlaybackServices() = default;
    // 必须使用同一 CTextWin；false 在 Tick 的 catch 范围内转为 int(1)。
    [[nodiscard]] virtual bool PreparePlaybackAt4C2A40() = 0;
    // 必须先取得实际根对象，再清其 +1B0 byte。
    virtual void ClearRootUpdateEnabledAt1B0After413BE0() = 0;
};

// 借用当前生产对象，不复制帧快照；依赖的寿命必须长于本对象。
// 不拥有 +64，不重新布局，也不从 46B230 的空虚槽触发。
class TextWindowPlaybackRuntime final
{
public:
    TextWindowPlaybackRuntime(TextWindowLayerRuntime& window,
        IGraphicsTextApi& text, IGraphicsTextGlyphApi& glyphs,
        ITextWindowTextServices& layout, const SaveDataEnvironmentArchive& save,
        const UnSaveDataRuntime& unsave, const ClickInfoArchive& click,
        Platform::MouseView mouse,
        Platform::KeyboardView keyboard,
        ITextWindowPlaybackServices& services) noexcept;

    // 1=推进/等待，0=本段完成，-1=捕获 C++ 异常。
    // 参数仅屏蔽 4B0550，不屏蔽滚轮或按住快进的检查。
    [[nodiscard]] std::int32_t TickAt4BB530(bool suppressAdvanceInput);

private:
    [[nodiscard]] bool ReadAdvanceInputAt4B0550() const;
    [[nodiscard]] bool ReadSkipAt4AA540() const;
    [[nodiscard]] std::int32_t FinishAt4BB697();

    TextWindowLayerRuntime& window_;
    IGraphicsTextApi& text_;
    IGraphicsTextGlyphApi& glyphs_;
    ITextWindowTextServices& layout_;
    const SaveDataEnvironmentArchive& save_;
    const UnSaveDataRuntime& unsave_;
    const ClickInfoArchive& click_;
    Platform::MouseView mouse_;
    GameGuiInputStateRuntime input_;
    ITextWindowPlaybackServices& services_;
};
}
