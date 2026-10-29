#pragma once

#include <Runtime/MessageFormatSelectionRuntime.h>
#include <Runtime/TextWindowLayerRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct ParamsGameSystem;
class SaveDataEnvironmentArchive;
class UnSaveDataRuntime;
class ScrVmValueTable;

class ITextWindowFormatErrorDisplay
{
public:
    virtual ~ITextWindowFormatErrorDisplay() = default;
    virtual void ShowMessageReadError() = 0;
};

class Win32TextWindowFormatErrorDisplay final : public ITextWindowFormatErrorDisplay
{
public:
    Win32TextWindowFormatErrorDisplay(const HWND& window, const std::string& titleCp932) noexcept;
    void ShowMessageReadError() override;
private:
    const HWND& window_;
    const std::string& title_;
};

// 4C2740 的生产调用链；所有依赖借用同一实际 owner，不保存第二组文本/已读状态。
class TextWindowFormatRuntime final
{
public:
    TextWindowFormatRuntime(TextWindowLayerRuntime& window, MessageFormatSelectionRuntime& messages,
        const ParamsGameSystem& params, const SaveDataEnvironmentArchive& save, UnSaveDataRuntime& unsave,
        IGraphicsTextApi& text, IGraphicsTextGlyphApi& glyphs, ITextWindowTextServices& layout,
        ITextWindowFormatErrorDisplay& errors) noexcept;
    [[nodiscard]] bool SetFormatAt4C2740(std::int32_t index, bool initialize);
    // 4C1650：读取可替换的当前 CParams，再选择/布局/补画全文；本地 catch 返回 false。
    [[nodiscard]] bool RebuildAt4C1650(const ScrVmValueTable& activeParams, bool refreshUnread);
    // DrawManager槽3在调用时取得，恢复服务不能误用构造时的另一窗口。
    [[nodiscard]] bool RebuildForWindowAt4C1650(TextWindowLayerRuntime& window,
        const ScrVmValueTable& activeParams, bool refreshUnread, ITextWindowTextServices& layout);

private:
    TextWindowLayerRuntime& window_;
    MessageFormatSelectionRuntime& messages_;
    const ParamsGameSystem& params_;
    const SaveDataEnvironmentArchive& save_;
    UnSaveDataRuntime& unsave_;
    IGraphicsTextApi& text_;
    IGraphicsTextGlyphApi& glyphs_;
    ITextWindowTextServices& layout_;
    ITextWindowFormatErrorDisplay& errors_;
};
}
