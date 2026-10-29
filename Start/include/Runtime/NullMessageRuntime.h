#pragma once

#include <Runtime/Message2Runtime.h>

namespace KaGuYa::Start::Runtime
{
// 现代source适配器；原10h Null正文仍持有+0C共享nil与两个XOR虚槽。
class NullMessageRuntime final : public IMessageRuntime
{
public:
    NullMessageRuntime() = default;
    ~NullMessageRuntime() noexcept override = default;

    [[nodiscard]] bool Load(const char* path) override;
    // 名称沿用共享契约，实际 Null 虚槽是 47DDF0 的 fldz，不读取未初始化的 +04。
    [[nodiscard]] float ReadVersionAt47DDE0() const override;
    [[nodiscard]] std::int32_t GetFormatCount() const noexcept override;
    [[nodiscard]] Message2FormatRef GetFormat(std::int32_t index) override;
    [[nodiscard]] std::int32_t GetChoiceCount() const noexcept override;
    [[nodiscard]] MessageTextCommandReference GetChoice(std::int32_t index) override;
    [[nodiscard]] std::int32_t GetMessageCount() const noexcept override;
    [[nodiscard]] MessageVoiceAndTextRef GetMessage(std::int32_t index) override;
    NativeNullMessageRuntime& Native() noexcept { return native_; }
    const NativeNullMessageRuntime& Native() const noexcept { return native_; }
private:
    NativeNullMessageRuntime native_;
};
}
