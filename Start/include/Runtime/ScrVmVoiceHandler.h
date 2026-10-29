#pragma once

#include <Runtime/ScrVmCore.h>

#include <cstddef>
#include <cstdint>
#include <memory_resource>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class IScriptFileCommandSource;
class IScrVmCommandErrorSink;
class IScriptVoiceSource;
class IScrVmVoiceNameSource
{
public:
    virtual ~IScrVmVoiceNameSource() = default;
    // 4D3286..4D32E8：无对应索引返回null；合法空名仍是有效的CString数据指针。
    [[nodiscard]] virtual const char* FindVoiceNameAt4D3286(std::int32_t index) const = 0;
};

// 显式快照供离线调用者使用；正式根装配由ParamsVoiceNameRuntime逐项取得当前Pattern。
class ScrVmVoiceNameTable final : public IScrVmVoiceNameSource
{
public:
    explicit ScrVmVoiceNameTable(std::vector<std::string> names);

    [[nodiscard]] const std::string& NameAt(std::int32_t index) const noexcept;
    [[nodiscard]] std::size_t Size() const noexcept;
    [[nodiscard]] const char* FindVoiceNameAt4D3286(std::int32_t index) const noexcept override;

private:
    std::vector<std::string> names_;
};

class ScrVmVoiceCommandException final : public std::runtime_error
{
public:
    explicit ScrVmVoiceCommandException(std::string detail);
    [[nodiscard]] std::string_view Detail() const noexcept;

private:
    std::string detail_;
};

// 对应 0x42C4F0 的语音槽对象；offset 名保留尚未确认业务名的三个虚调用。
class IScrVmVoiceRuntimeBoundary
{
public:
    virtual ~IScrVmVoiceRuntimeBoundary() = default;

    [[nodiscard]] virtual bool HasPlaybackSlots() const = 0;
    [[nodiscard]] virtual bool IsBatchObjectReadyAtA4() const = 0;
    // 42C573..42C599重新取GameSystem+4C；与已经创建的物理槽容量不同。
    [[nodiscard]] virtual std::int32_t VoiceBatchCapacityAt42C599() const = 0;
    [[nodiscard]] virtual std::size_t VoiceSlotCapacity() const = 0;
    virtual void StopAndClearAllVoiceSlots() = 0;
    [[nodiscard]] virtual bool PrepareSlotAtA8(
        std::size_t slotIndex,
        std::string_view resourceName) = 0;
    [[nodiscard]] virtual std::int32_t ReadSlotValueAtAC(
        std::size_t slotIndex,
        std::string_view resourceName) = 0;
    [[nodiscard]] virtual std::int32_t ReadFallbackValueAt6C(
        std::size_t slotIndex,
        std::string_view resourceName) = 0;
    [[nodiscard]] virtual bool OpenSlot(
        std::size_t slotIndex,
        std::string_view resourceName,
        std::int32_t resolvedValue) = 0;
    virtual void PlayVoiceSlot(std::size_t slotIndex) = 0;
    virtual void StoreVoiceSlotResourceName(
        std::size_t slotIndex,
        std::string_view resourceName) = 0;
};

class IScrVmVoiceCommandErrorSink
{
public:
    virtual ~IScrVmVoiceCommandErrorSink() = default;
    virtual void ReportVoiceFailure(std::string_view detail) = 0;
};

// 对应 opcode 27 / 0x4D2FF0。
class ScrVoiceInstructionHandler final : public IScrVmInstructionHandler
{
public:
    ScrVoiceInstructionHandler(
        const IScrVmVoiceNameSource& voiceNames,
        IScrVmVoiceRuntimeBoundary& voiceRuntime,
        IScrVmVoiceCommandErrorSink& errorSink,
        std::pmr::memory_resource& temporaryMemory = *std::pmr::new_delete_resource()) noexcept;
    ScrVoiceInstructionHandler(const IScrVmVoiceNameSource& voiceNames,
        IScriptFileCommandSource& pcSource, IScrVmCommandErrorSink& nativeErrors,
        IScriptVoiceSource& voiceSource) noexcept;
    [[nodiscard]] std::int32_t ExecuteRuntimeEntry(ScrScriptRuntimeState& runtime,
        IEnvironmentRootMemory& memory) override;

    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    const IScrVmVoiceNameSource& voiceNames_;
    // 离线工具入口显式借用；4D2FF0按原调用点取得manager/错误来源。
    IScrVmVoiceRuntimeBoundary* voiceRuntime_ = nullptr;
    IScrVmVoiceCommandErrorSink* errorSink_ = nullptr;
    // 借用临时容器分配器，须长于handler；不覆盖原CString管理器和Ref控制块分配协议。
    std::pmr::memory_resource* temporaryMemory_ = nullptr;
    IScriptFileCommandSource* pcSource_ = nullptr;
    IScrVmCommandErrorSink* nativeErrors_ = nullptr;
    IScriptVoiceSource* voiceSource_ = nullptr;
};
}
