#pragma once

#include <Runtime/ScrScriptRuntime.h>
#include <Runtime/ScrVmScriptCommandState.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class INormalStartupScriptTransactionServices
{
public:
    virtual ~INormalStartupScriptTransactionServices() = default;

    // 0x481720：仅更换根对象 +0x1C0 的 CParams generation，保留 CScript 已解析正文。
    virtual void ReplaceParamsGeneration() = 0;

    virtual void WriteRegistrySlot2DwordAt04(std::uint32_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt0C(std::uint8_t value) = 0;
    virtual void WriteRegistrySlot2DwordAt18(std::uint32_t value) = 0;
    virtual void WriteRegistrySlot2DwordAt14(std::uint32_t value) = 0;
    virtual void WriteRegistrySlot2DwordAt1C(std::uint32_t value) = 0;
    virtual void WriteRegistrySlot2DwordAt20(std::uint32_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt08(std::uint8_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt09(std::uint8_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt28(std::uint8_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt29(std::uint8_t value) = 0;

    [[nodiscard]] virtual std::int32_t ResolveScriptId(
        const char* scriptNameCp932) const = 0;
    [[nodiscard]] virtual ScrScriptSwitchResult LoadScript(
        std::int32_t scriptId) = 0;

    virtual void CopyRegistryPrologueFlag() = 0;
    virtual void SynchronizeRootOwnersAtVslot18() = 0;

    // 先取人名，再由Set内部一次捕获Graphics并检查模块；不能在事务层预查询。
    [[nodiscard]] virtual const char* ReadRegistrySlot1NameAt58() const = 0;
    [[nodiscard]] virtual const char* ReadRegistrySlot1NameAt5C() const = 0;
    virtual void SetGraphicsTextFirstName(const char* nameCp932) = 0;
    virtual void SetGraphicsTextSecondName(const char* nameCp932) = 0;

    virtual void MirrorParamsDwordAt3CTo40() = 0;
    virtual void CallRegistrySlot4Nullsub3() = 0;
};

// 0x4D53D0 的 a3 == -1 分支。失败与异常均保留此前副作用，不提供回滚。
class NormalStartupScriptTransaction final
{
public:
    NormalStartupScriptTransaction(
        ScrVmScriptCommandState& scriptCommandState,
        INormalStartupScriptTransactionServices& services) noexcept;

    [[nodiscard]] ScrScriptSwitchResult Execute(const char* scriptNameCp932);

private:
    ScrVmScriptCommandState* scriptCommandState_ = nullptr;
    INormalStartupScriptTransactionServices* services_ = nullptr;
};

class IStartupScriptTransactionServices : public INormalStartupScriptTransactionServices
{
public:
    virtual void WriteReplayFile(std::int32_t scriptId) = 0;
    virtual void WriteReplayPosition(std::int32_t entry) = 0;
};

// 样本 18 的 5B5CD0：スタート（ロード）.scr；用于回放调度，不是选中段的脚本名。
inline constexpr char Sample18ReplayStartupScriptCp932[] =
    "\x83\x58\x83\x5E\x81\x5B\x83\x67\x81\x69\x83\x8D\x81\x5B\x83\x68\x81\x6A.scr";

// 4D53D0 完整分派：entry==-1 进入既有 Normal，其余值（包括其他负数）进入 Replay。
class StartupScriptTransaction final
{
public:
    StartupScriptTransaction(ScrVmScriptCommandState& command,
        IStartupScriptTransactionServices& services) noexcept;
    [[nodiscard]] ScrScriptSwitchResult Execute(const char* scriptNameCp932, std::int32_t entry);

private:
    ScrVmScriptCommandState* command_;
    IStartupScriptTransactionServices* services_;
};
}
