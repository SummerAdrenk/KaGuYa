#pragma once

#include <Runtime/Message2Runtime.h>

#include <optional>

namespace KaGuYa::Start::Runtime
{
// 417C40 的输出指向调用者已有三引用；不另外拥有或复制 TextWin 的 +70/+7C/+88。
struct MessageFormatSelectionTarget final
{
    MessageTextCommandReference& name;
    RuntimeCStringArrayReference& voices;
    MessageTextCommandReference& body;
};

// 原生E8h的三槽已存在，不用reinterpret现代拥有包装，也不构造临时selection再交换。
struct MessageFormatSelectionRawTarget final
{
    YtlReferenceSlotRuntime& name;
    YtlReferenceSlotRuntime& voices;
    YtlReferenceSlotRuntime& body;
};

class IMessageFormatReadState
{
public:
    virtual ~IMessageFormatReadState() = default;
    // CMessage+10 的 byte 表，不是 +28 的正文已读，也不规范化非零 byte。
    [[nodiscard]] virtual std::optional<std::uint8_t> ReadFormatFlag(std::int32_t index) const = 0;
    virtual void SetFormatRead(std::int32_t index) = 0;
    virtual void PersistUnreadFlags() = 0;
};

class MessageFormatSelectionRuntime final
{
public:
    MessageFormatSelectionRuntime(IMessageFormatSource& source, IMessageFormatReadState& readState) noexcept;
    // 返回原已读 byte 或 -1；失败可能已经替换引用/置位，不是事务回滚接口。
    [[nodiscard]] std::int32_t ReadFormatAt417C40(std::int32_t index,
        MessageFormatSelectionTarget output, std::uint32_t variant, bool markRead);
    [[nodiscard]] std::int32_t ReadFormatRawAt417C40(std::int32_t index,
        MessageFormatSelectionRawTarget output, std::uint32_t variant, bool markRead);

private:
    IMessageFormatSource& source_;
    IMessageFormatReadState& readState_;
};

// 4C5850 显式清组：name -> body -> voices；不同于 4100C0 的析构逆序。
void ClearMessageFormatAt4C5850(MessageFormatSelectionTarget output) noexcept;
// 原入口没有noexcept；正常释放异常外传，前面的已清槽不回滚。
void ClearMessageFormatRawAt4C5850(MessageFormatSelectionRawTarget output);
// 410360：旧control释放后逐槽读源，三槽发布结束才增持；self赋值无操作。
void AssignMessageReferenceAt410360(YtlReferenceSlotRuntime& target,
    const YtlReferenceSlotRuntime& source);
}
