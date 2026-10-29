#pragma once

#include <Runtime/Message2Runtime.h>
#include <Runtime/MessageReaderReference.h>

namespace KaGuYa::Start::Runtime
{
// 借用稳定Ref槽而非缓存正文地址；跨owner寿命的调用者须先增持Ref，再绑定有效文件服务。
// 此对象不进入原reader正文/control，不延长reader、文件服务或事务的寿命。
class MessageReaderRuntimeAdapter final : public IMessageRuntime
{
public:
    MessageReaderRuntimeAdapter(const MessageReaderReference& reader,
        MessageReaderCallContext context) noexcept : reader_(reader), context_(context) {}
    bool Load(const char* path) override;
    float ReadVersionAt47DDE0() const override;
    std::int32_t GetFormatCount() const noexcept override;
    Message2FormatRef GetFormat(std::int32_t index) override;
    std::int32_t GetChoiceCount() const noexcept override;
    MessageTextCommandReference GetChoice(std::int32_t index) override;
    std::int32_t GetMessageCount() const noexcept override;
    MessageVoiceAndTextRef GetMessage(std::int32_t index) override;
private:
    const MessageReaderReference& reader_;
    MessageReaderCallContext context_;
};
}
