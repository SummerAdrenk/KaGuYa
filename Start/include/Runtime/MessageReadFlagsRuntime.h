#pragma once

#include <Runtime/MessageFormatSelectionRuntime.h>
#include <Runtime/LayerWindowScrVmMenuRuntime.h>
#include <Runtime/PersistenceFileAccess.h>
#include <Runtime/RuntimeStringBorrow.h>
#include <Runtime/ByteArrayReferenceRuntime.h>
#include <Runtime/MessageEnvironmentPathSource.h>

#include <array>
#include <optional>
#include <string>

namespace KaGuYa::Start::Runtime
{
class MessageReaderReference;
using MessageReadFlagsStorage = std::array<ByteArrayReferenceRuntime, 3>;
static_assert(sizeof(MessageReadFlagsStorage) == 0x24);
static_assert(alignof(MessageReadFlagsStorage) == 4);

// CMessage+10/+1C/+28 的原三槽；本服务对象及其外部依赖不进入native正文。
// 带storage的构造只借用，不增持/释放；旧构造显式拥有独立fixture存储。
class MessageReadFlagsRuntime final : public IMessageFormatReadState, public IMenuUnreadChoiceState
{
public:
    MessageReadFlagsRuntime(IStartFileApi& files, RuntimeStringBorrow saveDirectoryCp932) noexcept;
    MessageReadFlagsRuntime(IStartFileApi& files, const IMessageEnvironmentPathSource& paths) noexcept;
    MessageReadFlagsRuntime(MessageReadFlagsStorage& storage, IStartFileApi& files,
        RuntimeStringBorrow saveDirectoryCp932) noexcept;
    MessageReadFlagsRuntime(MessageReadFlagsStorage& storage, IStartFileApi& files,
        const IMessageEnvironmentPathSource& paths) noexcept;
    MessageReadFlagsRuntime(const MessageReadFlagsRuntime&) = delete;
    MessageReadFlagsRuntime& operator=(const MessageReadFlagsRuntime&) = delete;

    void LoadUnreadFlagsAt494280();
    void ReconcileReaderCountsAt45A9D6(const IMessageRuntime& message);
    void ReconcileCurrentReaderAt45A9D6(const MessageReaderReference& reader);
    void ReleaseArraysAt463250() noexcept;
    void PersistUnreadFlags() override;
    [[nodiscard]] std::optional<std::uint8_t> ReadFormatFlag(std::int32_t index) const override;
    void SetFormatRead(std::int32_t index) override;
    [[nodiscard]] std::optional<bool> ReadChoiceFlag(std::int32_t index) const override;
    void SetChoiceFlag(std::int32_t index) override;
    [[nodiscard]] std::int32_t MarkChoiceAt4C29D0(std::int32_t index, std::uint8_t mark);
    [[nodiscard]] std::span<const std::uint8_t> FormatFlagsAt10() const noexcept;
    [[nodiscard]] std::span<const std::uint8_t> ChoiceFlagsAt1C() const noexcept;
    [[nodiscard]] std::span<const std::uint8_t> BodyFlagsAt28() const noexcept;
    // 消费者借用稳定槽，不缓存随读取/重建失效的 span，也不增加数组引用计数。
    [[nodiscard]] ByteArrayReferenceRuntime& FormatReferenceAt10() noexcept { return groups_[0]; }
    [[nodiscard]] ByteArrayReferenceRuntime& ChoiceReferenceAt1C() noexcept { return groups_[1]; }
    [[nodiscard]] ByteArrayReferenceRuntime& BodyReferenceAt28() noexcept { return groups_[2]; }
    [[nodiscard]] const ByteArrayReferenceRuntime& FormatReferenceAt10() const noexcept { return groups_[0]; }
    [[nodiscard]] const ByteArrayReferenceRuntime& ChoiceReferenceAt1C() const noexcept { return groups_[1]; }
    [[nodiscard]] const ByteArrayReferenceRuntime& BodyReferenceAt28() const noexcept { return groups_[2]; }

private:
    [[nodiscard]] RuntimeStringBorrow CurrentSaveDirectory() const;
    IStartFileApi& files_;
    std::optional<RuntimeStringBorrow> saveDirectory_;
    const IMessageEnvironmentPathSource* paths_ = nullptr;
    // 借用入口保持disengaged；生产native正文只嵌MessageReadFlagsStorage，不嵌本类。
    std::optional<MessageReadFlagsStorage> standalone_;
    MessageReadFlagsStorage& groups_;
};
}
