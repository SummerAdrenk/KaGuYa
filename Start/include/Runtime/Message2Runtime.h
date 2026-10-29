#pragma once

#include <Runtime/PersistenceFileAccess.h>
#include <Runtime/MessageVoiceAndText.h>
#include <Runtime/MessageFormat.h>
#include <Runtime/MessageReaderVectors.h>
#include <Runtime/MessageReaderNative.h>

#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class IMessageTextSource
{
public:
    virtual ~IMessageTextSource() = default;
    virtual MessageVoiceAndTextRef GetMessage(std::int32_t index) = 0;
};

class IMessageFormatSource
{
public:
    virtual ~IMessageFormatSource() = default;
    virtual Message2FormatRef GetFormat(std::int32_t index) = 0;
};

class IMessageChoiceSource
{
public:
    virtual ~IMessageChoiceSource() = default;
    virtual MessageTextCommandReference GetChoice(std::int32_t index) = 0;
};

class IMessageReaderVersion
{
public:
    virtual ~IMessageReaderVersion() = default;
    [[nodiscard]] virtual float ReadVersionAt47DDE0() const = 0;
};

// CMessage 持有的原 IMessage 业务契约；姓名只由 Message2 的内部索引查询使用。
class IMessageRuntime : public IMessageTextSource, public IMessageFormatSource,
    public IMessageChoiceSource, public IMessageReaderVersion
{
public:
    [[nodiscard]] virtual bool Load(const char* path) = 0;
    [[nodiscard]] virtual std::int32_t GetFormatCount() const noexcept = 0;
    [[nodiscard]] virtual std::int32_t GetChoiceCount() const noexcept = 0;
    [[nodiscard]] virtual std::int32_t GetMessageCount() const noexcept = 0;
};

// 现代source适配器：文件服务只保存在本层，实际读写均由原生90h reader执行。
class Message2Runtime final : public IMessageRuntime
{
public:
    explicit Message2Runtime(
        IStartFileApi& api, const FileTransactionContext* transactionContext = nullptr);

    [[nodiscard]] bool Load(const char* path) override;
    [[nodiscard]] float ReadVersionAt47DDE0() const override;
    [[nodiscard]] MessageVoiceAndTextRef GetMessage(std::int32_t index) override;
    [[nodiscard]] MessageTextCommandReference GetName(std::int32_t index);
    [[nodiscard]] MessageTextCommandReference GetChoice(std::int32_t index) override;
    [[nodiscard]] Message2FormatRef GetFormat(std::int32_t index) override;

    [[nodiscard]] std::int32_t GetNameCount() const noexcept;
    [[nodiscard]] std::int32_t GetChoiceCount() const noexcept override;
    [[nodiscard]] std::int32_t GetMessageCount() const noexcept override;
    [[nodiscard]] std::int32_t GetFormatCount() const noexcept override;
    NativeMessage2Runtime& Native() noexcept { return native_; }
    const NativeMessage2Runtime& Native() const noexcept { return native_; }

private:
    MessageReaderCallContext context_;
    NativeMessage2Runtime native_;
};
}
