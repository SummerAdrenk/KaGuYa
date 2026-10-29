#pragma once

#include <Runtime/GraphicsTextRuntime.h>
#include <Runtime/Message2Runtime.h>
#include <Runtime/TextDisplaySceneRuntime.h>
#include <Runtime/ByteArrayReferenceRuntime.h>
#include <Runtime/MessageFormatSelectionRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 4AD590 的输出引用槽；+00 的旧数据归属由原调用者提供，本入口只负责释放。
struct MessageTextSelection final
{
    std::shared_ptr<void> storageAt00;
    RuntimeCStringArrayReference voicesAt0C;
    MessageTextCommandReference textAt18;
};

class MessageTextSelectionRuntime final
{
public:
    // CMessage+28 的已读 byte 表由消息根对象持有，不能复制成场景私有快照。
    MessageTextSelectionRuntime(IMessageTextSource& source,
        const ByteArrayReferenceRuntime& readFlagsAt28) noexcept;

    [[nodiscard]] std::int32_t ReadTextAt4AD590(
        std::int32_t index, MessageTextSelection& output);
    [[nodiscard]] std::int32_t ReadTextAt4AD590(
        std::int32_t index, MessageFormatSelectionTarget output);
    [[nodiscard]] std::int32_t ReadTextRawAt4AD590(
        std::int32_t index, MessageFormatSelectionRawTarget output);

private:
    IMessageTextSource* source_;
    const ByteArrayReferenceRuntime* readFlags_;
};

// 418700：CMessage+1C 的选项已读 byte 表，和正文 +28 不共用。
class MessageChoiceSelectionRuntime final
{
public:
    MessageChoiceSelectionRuntime(IMessageChoiceSource& source,
        const ByteArrayReferenceRuntime& readFlagsAt1C) noexcept;
    [[nodiscard]] std::int32_t ReadChoiceAt418700(std::int32_t index,
        MessageTextSelection& output, std::int32_t unusedArgument);
    [[nodiscard]] std::int32_t ReadChoiceRawAt418700(std::int32_t index,
        MessageFormatSelectionRawTarget output, std::int32_t unusedArgument);

private:
    IMessageChoiceSource& source_;
    const ByteArrayReferenceRuntime& readFlags_;
};

// 4A2325..4A2388：params >= 4 的 TextDisplay 消息分支。旧 params 的 417C40
// 是另一条 reader，不把此类伪装成两代通用解析器。
class TextDisplayMessageTextRuntime final
{
public:
    TextDisplayMessageTextRuntime(IGraphicsTextApi& graphics,
        MessageTextSelectionRuntime& messages) noexcept;

    [[nodiscard]] bool CreateFontFromContextSample18(std::int32_t contextIndex);
    [[nodiscard]] TextDisplayTextInitializationResult InitializeFromMessageIndex(
        std::int32_t textStringIndex);

private:
    IGraphicsTextApi* graphics_;
    MessageTextSelectionRuntime* messages_;
};
}
