#pragma once

#include <Runtime/Message2Runtime.h>

namespace KaGuYa::Start::Runtime
{
// 原 GameScript::Message（5B2610），v2/v3 将姓名、语音和正文内联在 format 记录中。
class LegacyMessageRuntime final : public IMessageRuntime
{
public:
    explicit LegacyMessageRuntime(
        IStartFileApi& files, const FileTransactionContext* transaction = nullptr);

    [[nodiscard]] bool Load(const char* path) override;
    [[nodiscard]] float ReadVersionAt47DDE0() const override;
    [[nodiscard]] Message2FormatRef GetFormat(std::int32_t index) override;
    [[nodiscard]] MessageTextCommandReference GetChoice(std::int32_t index) override;
    [[nodiscard]] MessageVoiceAndTextRef GetMessage(std::int32_t index) override;
    [[nodiscard]] std::int32_t GetFormatCount() const noexcept override;
    [[nodiscard]] std::int32_t GetChoiceCount() const noexcept override;
    [[nodiscard]] std::int32_t GetMessageCount() const noexcept override;
    NativeLegacyMessageRuntime& Native() noexcept { return native_; }
    const NativeLegacyMessageRuntime& Native() const noexcept { return native_; }

private:
    MessageReaderCallContext context_;
    NativeLegacyMessageRuntime native_;
};
}
