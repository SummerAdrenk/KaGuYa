#pragma once

#include <Runtime/ScrResourceNameSource.h>
#include <Runtime/ScrVmCore.h>
#include <Runtime/ScrVmValueTable.h>
#include <Runtime/ScrVmScriptStorage.h>

#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class IScriptFileCommandSource;
class ScrVmUpdateCommandException final : public std::runtime_error
{
public:
    explicit ScrVmUpdateCommandException(std::string detail);

    [[nodiscard]] std::string_view Detail() const noexcept;

private:
    std::string detail_;
};

// 0x4CB030 与 0x4D6C10 属于图层运行时，不属于 VM；此边界保留两次调用的原顺序。
class IScrVmUpdateRuntimeBoundary
{
public:
    virtual ~IScrVmUpdateRuntimeBoundary() = default;
    virtual void ApplyResultLayerList(std::int32_t listIndex) = 0;
    virtual void SelectLayerEffect(std::int32_t selector) = 0;
    [[nodiscard]] virtual bool ReadActiveResultPair(
        std::size_t index, ScrVmUpdatePair& destination) const = 0;
};

// 4D2A85..4D2A9A在资源模式已发布后才取当前root/CParams，不能缓存旧参数表。
class IScrVmUpdateValueSource
{
public:
    virtual ~IScrVmUpdateValueSource() = default;
    [[nodiscard]] virtual std::int32_t ReadUpdateValueAt4C73A0(std::int32_t index) const = 0;
};

class IScrVmUpdateCommandErrorSink
{
public:
    virtual ~IScrVmUpdateCommandErrorSink() = default;
    virtual void ReportUpdateFailure(std::string_view detail) = 0;
};

// 对应 opcode 6 / 0x4D2800。成功固定返回 3，捕获的命令错误返回 2。
class ScrUpdateInstructionHandler final : public IScrVmInstructionHandler
{
public:
    ScrUpdateInstructionHandler(
        IScrVmUpdateValueSource& values,
        const IScrResourceNameSource& resourceNames,
        IScrVmUpdateRuntimeBoundary& updateRuntime,
        ScrVmScriptStorage& storage,
        IScrVmUpdateCommandErrorSink& errorSink) noexcept;

    // 正式VM注册必须同时提供原PC取源和MessageBox边界；五参数入口只供显式decoded调用。
    ScrUpdateInstructionHandler(IScrVmUpdateValueSource& values,
        const IScrResourceNameSource& resourceNames, IScrVmUpdateRuntimeBoundary& updateRuntime,
        ScrVmScriptStorage& storage, IScrVmUpdateCommandErrorSink& errorSink,
        IScriptFileCommandSource& pcSource, IScrVmCommandErrorSink& nativeErrors) noexcept;

    [[nodiscard]] std::int32_t ExecuteRuntimeEntry(
        ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;

    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    IScrVmUpdateValueSource& values_;
    const IScrResourceNameSource& resourceNames_;
    IScrVmUpdateRuntimeBoundary& updateRuntime_;
    ScrVmScriptStorage& storage_;
    IScrVmUpdateCommandErrorSink& errorSink_;
    IScriptFileCommandSource* pcSource_ = nullptr;
    IScrVmCommandErrorSink* nativeErrors_ = nullptr;
};
}
