#pragma once

#include <Runtime/ScrVmParamsGenerationBinding.h>

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
enum class ScrVmValueError : std::uint8_t
{
    IndexMinusOne,
    IndexOutOfRange,
};

class ScrVmValueException final : public std::out_of_range
{
public:
    ScrVmValueException(ScrVmValueError error, std::int32_t index, const char* message);

    [[nodiscard]] ScrVmValueError Error() const noexcept;
    [[nodiscard]] std::int32_t Index() const noexcept;

private:
    ScrVmValueError error_;
    std::int32_t index_;
};

// 对应样本 18 运行时对象 +0x30 的固定值区：0x800 字节，共 512 个 DWORD。
class ScrVmValueTable final
{
public:
    static constexpr std::size_t ValueCount = ScrVmParamsGeneration::ValueCount;

    ScrVmValueTable();
    explicit ScrVmValueTable(
        ScrVmParamsGenerationBinding generationSlot);

    ScrVmValueTable(const ScrVmValueTable&) = delete;
    ScrVmValueTable& operator=(const ScrVmValueTable&) = delete;

    [[nodiscard]] std::int32_t Read(std::uint16_t index) const;
    [[nodiscard]] std::int32_t ReadSigned(std::int32_t index) const;
    // 借用与 ReadSigned 相同的当前 CParams；不缓存可被 Replace 替换的 generation。
    [[nodiscard]] const ScrVmParamsGeneration& CurrentParamsGeneration() const;
    [[nodiscard]] ScrVmParamsGeneration& CurrentParamsGeneration();
    // 仅比较槽身份，不读取当前正文或触发延迟创建。
    [[nodiscard]] bool UsesParamsGenerationSlot(
        const ScrVmParamsGenerationBinding& generationSlot) const noexcept;
    void SetMessageFormatIndexAt60(std::int32_t index);
    // text/menu 与 SAVE、归档、恢复服务共用 CSaveParams2+08，不持有指令私有副本。
    void SetInstructionIndexAtSaveParams08(std::int32_t index);
    void AppendCurrentMessageToHistoryAt3C();
    void Clear();
    void Write(std::uint16_t index, std::int32_t value);
    void WriteSigned(std::int32_t index, std::int32_t value);

private:
    // root+1C0借用不得跨payload生存期；稳定view每次取Current，不缓存正文或伪造shared所有权。
    ScrVmParamsGenerationBinding generationSlot_;
};
}
