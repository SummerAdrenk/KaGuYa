#pragma once

#include <Runtime/ScriptNativeBody.h>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class ScrVmScriptCommandState;
[[nodiscard]] bool operator==(const ScrVmScriptCommandState&,
    const ScrVmScriptCommandState&) noexcept;

// 生产只借CCh原位字段；默认构造及复制构造是独立夹具/快照，不拥有原Ref。
// 赋值只导入已发布command字段，不重绑receiver，也不修改属于runtime的+25。
class ScrVmScriptCommandState final
{
public:
    static constexpr unsigned FlagsAt04Mask = 1u << 0;
    static constexpr unsigned ReloadByteAt08Mask = 1u << 1;
    static constexpr unsigned ReloadDwordAt08Mask = 0x1Eu;
    static constexpr unsigned StateByteAt24Mask = 1u << 5;
    static constexpr unsigned SaveGateAt25Mask = 1u << 6;
    static constexpr unsigned StateByteAt26Mask = 1u << 7;
    static constexpr unsigned StateByteAt27Mask = 1u << 8;
    static constexpr unsigned StateByteAt28Mask = 1u << 9;
    static constexpr unsigned CommandFieldsMask = 0x3BFu;

    ScrVmScriptCommandState() noexcept;
    ScrVmScriptCommandState(ScriptNativeBody& body, ScriptNativePublication& publication) noexcept;
    ScrVmScriptCommandState(const ScrVmScriptCommandState& source) noexcept;
    ScrVmScriptCommandState& operator=(const ScrVmScriptCommandState& source) noexcept;

    [[nodiscard]] ScriptNativeBody* BorrowedBody() const noexcept { return body_; }
    [[nodiscard]] ScriptNativePublication& Publication() noexcept { return publication_; }
    [[nodiscard]] const ScriptNativePublication& Publication() const noexcept { return publication_; }

    [[nodiscard]] std::uint32_t ReadFlagsAt04() const;
    void WriteFlagsAt04(std::uint32_t value) noexcept;
    [[nodiscard]] bool IsFlagsAt04Published() const noexcept { return Published(FlagsAt04Mask); }

    [[nodiscard]] std::uint8_t ReadReloadByteAt08() const;
    void WriteReloadByteAt08(std::uint8_t value) noexcept;
    [[nodiscard]] bool IsReloadByteAt08Published() const noexcept { return Published(ReloadByteAt08Mask); }

    // 完整DWORD的显式输入与原已确认的byte写口分开；后者不发布或读取高三byte。
    [[nodiscard]] std::int32_t ReadReloadDwordAt08() const;
    void WriteReloadDwordAt08(std::int32_t value) noexcept;
    [[nodiscard]] bool IsReloadDwordAt08Published() const noexcept { return Published(ReloadDwordAt08Mask); }

    [[nodiscard]] std::uint8_t ReadStateByteAt24() const
    { return ReadTail(0, StateByteAt24Mask, "CScript+24 read before publication"); }
    void WriteStateByteAt24(std::uint8_t value) noexcept { WriteTail(0, StateByteAt24Mask, value); }
    [[nodiscard]] bool IsStateByteAt24Published() const noexcept { return Published(StateByteAt24Mask); }
    [[nodiscard]] std::uint8_t ReadStateByteAt26() const
    { return ReadTail(2, StateByteAt26Mask, "CScript+26 read before publication"); }
    void WriteStateByteAt26(std::uint8_t value) noexcept { WriteTail(2, StateByteAt26Mask, value); }
    [[nodiscard]] bool IsStateByteAt26Published() const noexcept { return Published(StateByteAt26Mask); }
    [[nodiscard]] std::uint8_t ReadStateByteAt27() const
    { return ReadTail(3, StateByteAt27Mask, "CScript+27 read before publication"); }
    void WriteStateByteAt27(std::uint8_t value) noexcept { WriteTail(3, StateByteAt27Mask, value); }
    [[nodiscard]] bool IsStateByteAt27Published() const noexcept { return Published(StateByteAt27Mask); }
    [[nodiscard]] std::uint8_t ReadStateByteAt28() const
    { return ReadTail(4, StateByteAt28Mask, "CScript+28 read before publication"); }
    void WriteStateByteAt28(std::uint8_t value) noexcept { WriteTail(4, StateByteAt28Mask, value); }
    [[nodiscard]] bool IsStateByteAt28Published() const noexcept { return Published(StateByteAt28Mask); }

    friend bool operator==(const ScrVmScriptCommandState&,
        const ScrVmScriptCommandState&) noexcept;

private:
    [[nodiscard]] bool Published(unsigned mask) const noexcept
    { return (publication_.commandMask & mask) == mask; }
    void RequirePublication(unsigned mask, const char* message) const;
    [[nodiscard]] std::uint8_t ReadTail(unsigned index, unsigned mask, const char* message) const;
    void WriteTail(unsigned index, unsigned mask, std::uint8_t value) noexcept;

    // 仅独立夹具/快照使用这些字段；生产引用直接指向传入正文，没有同步副本。
    ScriptCommandPrefix fixturePrefix_;
    ScriptCommandTail fixtureTail_;
    ScriptNativePublication fixturePublication_;
    ScriptNativeBody* body_ = nullptr;
    ScriptCommandPrefix& prefix_;
    ScriptCommandTail& tail_;
    ScriptNativePublication& publication_;
};
}
