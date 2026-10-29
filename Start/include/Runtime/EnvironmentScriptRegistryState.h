#pragma once

#include <Runtime/NormalStartupScriptServices.h>
#include <Runtime/ProgramRuntimeServices.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentRegistryRuntime;

// 4D53D0及4CFB30的叶访问；只借唯一registry，每个原调用点重新捕获，不持正文副本。
class EnvironmentScriptRegistryState final : public INormalStartupRegistryState,
    public IProgramRegistrySlot1, public IProgramRegistrySlot2,
    public IProgramRegistrySlot7, public IProgramCommand15Nullsub
{
public:
    explicit EnvironmentScriptRegistryState(EnvironmentRegistryRuntime& registry) noexcept;
    EnvironmentScriptRegistryState(const EnvironmentScriptRegistryState&) = delete;
    EnvironmentScriptRegistryState& operator=(const EnvironmentScriptRegistryState&) = delete;
    [[nodiscard]] ProgramRuntimeRegistry& ProgramRegistry() noexcept { return program_; }

    void WriteSlot2DwordAt04(std::uint32_t value) override;
    void WriteSlot2ByteAt0C(std::uint8_t value) override;
    void WriteSlot2DwordAt18(std::uint32_t value) override;
    void WriteSlot2DwordAt14(std::uint32_t value) override;
    void WriteSlot2DwordAt1C(std::uint32_t value) override;
    void WriteSlot2DwordAt20(std::uint32_t value) override;
    void WriteSlot2ByteAt08(std::uint8_t value) override;
    void WriteSlot2ByteAt09(std::uint8_t value) override;
    void WriteSlot2ByteAt28(std::uint8_t value) override;
    void WriteSlot2ByteAt29(std::uint8_t value) override;
    std::uint16_t ReadSlot7WordAt0C() const override;
    const char* ReadSlot1NameAt58() const override;
    const char* ReadSlot1NameAt5C() const override;
    void CallSlot4Nullsub3() override;

    std::uint8_t ReadByteAt84() const override;
    std::uint8_t ReadByteAt6F() const override;
    void WriteChildVslot84Byte(std::int32_t index, bool value) override;
    bool ReadChildVslot84Byte(std::int32_t index) const override;
    void WriteDwordAt14(std::int32_t value) override;
    void WriteWordAt0C(std::uint32_t index, std::uint16_t value) override;
    std::uint16_t ReadWordAt0C(std::uint32_t index) const override;
    void CallNullsub2() override;

private:
    class Toggle final : public IProgramRegistryToggleSlot
    {
    public:
        Toggle(EnvironmentRegistryRuntime& registry, bool scene) noexcept
            : registry_(registry), scene_(scene) {}
        void CallVslot18(std::int32_t value) override;
        void ResolveForNullsub() override;
    private:
        EnvironmentRegistryRuntime& registry_;
        bool scene_;
    };
    EnvironmentRegistryRuntime& registry_;
    Toggle cg_, scene_;
    ProgramRuntimeRegistry program_;
};
}
