#pragma once

#include <Runtime/TextWindowNativeText.h>
#include <Runtime/GuiInputNative.h>

namespace KaGuYa::Start::Runtime
{
class ClickInfoArchive;

// 调用期借用的原叶边界；声音准备/输入不得绑定到现代TextWin镜像。
// 这些叶的完整进程来源另行装配，本接口不提供默认成功或固定返回值。
class ITextWindowNativePlaybackServices
{
public:
    virtual ~ITextWindowNativePlaybackServices() = default;
    virtual bool PreparePlaybackAt4C2A40(TextWindowLayerNativeBodyE8h&) = 0;
    virtual bool ReadAdvanceInputAt4B0550(ClickInfoArchive&) = 0;
    // 一次捕获的Mouse+2C/+34短路查询；避免正文外可变view被重入覆盖。
    virtual bool IsPacedMouseAt4BB5D1() = 0;
    virtual bool ReadSkipAt4AA540(TextWindowLayerNativeBodyE8h&) = 0;
    virtual void ClearRootUpdateEnabledAt1B0After413BE0() = 0;
};

class TextWindowNativePlayback final
{
public:
    TextWindowNativePlayback(TextWindowLayerNativeBodyE8h&, EnvironmentRootOwner&,
        IGraphicsTextApi&, TextWindowNativeText&, ITextWindowNativePlaybackServices&) noexcept;
    // 4BB5AF读取整个DWORD；1=推进/等待，0=完成，-1=本地C++异常。
    [[nodiscard]] std::int32_t TickAt4BB530(std::uint32_t suppressAdvanceInput);
private:
    std::int32_t FinishAt4BB697();
    TextWindowLayerNativeBodyE8h& body_;
    EnvironmentRootOwner& environment_;
    IGraphicsTextApi& text_;
    TextWindowNativeText& layout_;
    ITextWindowNativePlaybackServices& services_;
};
}
