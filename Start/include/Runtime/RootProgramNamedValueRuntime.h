#pragma once
#include <Runtime/ScrVmParamsGeneration.h>

#include <Runtime/ScrVmProgramHandler.h>
#include <Runtime/ScriptNamedValueNative.h>

#include <Windows.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;
class RootRuntimeOwner;

class IProgramNamedValueRuntime
{
public:
    virtual ~IProgramNamedValueRuntime() = default;
    [[nodiscard]] virtual std::int32_t ReadNamedValue(ScrVmProgramNamedValue name) const = 0;
    virtual void WriteNamedValue(ScrVmProgramNamedValue name, std::int32_t value) = 0;
    virtual void WriteProgResultFromRegistrySlot1ByteAt6F() = 0;
};

// 稳定的进程owner、原生名称表、CString管理器与系统叶须长于本适配器。
// 每次操作仅借用本次捕获的正文，不保活旧代，也不支持借用期间并发释放正文。
class RootProgramNamedValueRuntime final : public IProgramNamedValueRuntime
{
public:
    RootProgramNamedValueRuntime(EnvironmentStartupRuntime& environment, RootRuntimeOwner& root,
        ScriptNamedValueNativeTables tables, decltype(&::MessageBoxA) messageBox,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;

    [[nodiscard]] std::int32_t ReadNamedValue(ScrVmProgramNamedValue name) const override;
    // 419A22与417B58分别采用相反的取源顺序，不能共用隐含顺序的getter。
    [[nodiscard]] std::int32_t ReadAfterRootAt4C73A0(const char* name) const;
    [[nodiscard]] std::int32_t ReadAfterSlot0At4C73A0(const char* name) const;
    void WriteNamedValue(ScrVmProgramNamedValue name, std::int32_t value) override;
    void WriteProgResultFromRegistrySlot1ByteAt6F() override;

private:
    struct NamedTarget final
    {
        ScrVmParamsGeneration& params;
        std::int32_t index;
    };

    [[nodiscard]] NamedTarget PrepareNamedValue(ScrVmProgramNamedValue name) const;
    [[nodiscard]] NamedTarget PrepareNamedValue(const char* name, bool slotFirst) const;
    [[nodiscard]] std::int32_t& RequireValue(ScrVmParamsGeneration& params,
        std::int32_t index, const char* errorText) const;

    EnvironmentStartupRuntime& environment_;
    RootRuntimeOwner& root_;
    ScriptNamedValueNativeTables tables_;
    decltype(&::MessageBoxA) messageBox_;
    IRuntimeStringManagerSource& strings_;
};
}
