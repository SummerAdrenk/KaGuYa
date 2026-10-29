#pragma once

#include <Runtime/ScrScriptRuntime.h>
#include <Runtime/ScrResourceNameSource.h>
#include <Runtime/ScrVmCore.h>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
class IScriptFileCommandSource;
class IScriptMusicSource;
enum class ScrVmMusicError : std::uint8_t
{
    MissingActiveResourceName,
};

class ScrVmMusicException final : public std::runtime_error
{
public:
    ScrVmMusicException(ScrVmMusicError error, const char* message);
    [[nodiscard]] ScrVmMusicError Error() const noexcept;

private:
    ScrVmMusicError error_;
};

// 4D5A31/4D5B12 写入时才取当前 Environment slot2+2C，不缓存跨环境代的字符串引用。
class IScrVmMusicNameWriter
{
public:
    virtual ~IScrVmMusicNameWriter() = default;
    virtual void WriteCurrentMusicNameAt2C(const char* name) = 0;
};

// 对应 0x4D59E0 消费的音乐管理器对象。接口顺序就是原函数的观察顺序。
class IScrVmMusicRuntimeBoundary
{
public:
    virtual ~IScrVmMusicRuntimeBoundary() = default;

    [[nodiscard]] virtual std::int32_t ReadPlaybackParameter() const = 0;
    [[nodiscard]] virtual bool HasPlaybackSlots() const = 0;
    [[nodiscard]] virtual bool IsCurrentSlotActive() const = 0;
    // 4D5B62 直接借用槽内 CString；合法空串参与比较，不复制或以空串代替缺槽。
    [[nodiscard]] virtual const char* CurrentSlotResourceName() const = 0;
    virtual void SwitchCurrentSlot(
        std::string_view resourceName,
        std::int32_t playbackParameter) = 0;
    virtual void StopMusicSlots() = 0;
};

// 对应 opcode 10 / 0x4CF9E0；指令正文只有一个有符号 int32 资源 ID。
class ScrMusicInstructionHandler final : public IScrVmInstructionHandler
{
public:
    ScrMusicInstructionHandler(
        const IScrResourceNameSource& resourceNames,
        IScrVmMusicNameWriter& musicName,
        IScrVmMusicRuntimeBoundary& musicRuntime) noexcept;
    ScrMusicInstructionHandler(const IScrResourceNameSource& resourceNames,
        IScrVmMusicNameWriter& musicName, IScriptFileCommandSource& pcSource,
        IScriptMusicSource& musicSource) noexcept;
    [[nodiscard]] std::int32_t ExecuteRuntimeEntry(ScrScriptRuntimeState& runtime,
        IEnvironmentRootMemory& memory) override;

    // 对应共享函数 0x4D59E0，供 text/menu/restore 等已确认调用点复用。
    void ApplyResourceId(std::int32_t resourceId);
    void ApplyNativeResourceId(std::int32_t resourceId);

    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    const IScrResourceNameSource& resourceNames_;
    IScrVmMusicNameWriter& musicName_;
    // 仅离线 Execute/ApplyResourceId 使用；原4CF9E0/4D59E0入口不固定manager。
    IScrVmMusicRuntimeBoundary* musicRuntime_ = nullptr;
    IScriptFileCommandSource* pcSource_ = nullptr;
    IScriptMusicSource* musicSource_ = nullptr;
};
}
