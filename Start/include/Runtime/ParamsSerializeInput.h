#pragma once

#include <Runtime/SerializeBuffer.h>
#include <Runtime/ParamsInputBytesBorrow.h>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// 执行器的借用接口，方法名标原调用点，不冒充CSerialize完整虚表布局。
// 5B22F4+14 ->479C80仅lea eax,[ecx+4]; ret，没有计数/offset输出参数。
class IParamsSerializeInput
{
public:
    [[nodiscard]] virtual std::uint8_t ReadModeAt04() = 0;
    [[nodiscard]] virtual ParamsInputBytesBorrow CaptureInputAt14() = 0;
protected:
    ~IParamsSerializeInput() = default;
};

class ParamsSerializeBufferInput final : public IParamsSerializeInput
{
public:
    explicit ParamsSerializeBufferInput(SerializeBuffer& input) noexcept : input_(input) {}
    [[nodiscard]] std::uint8_t ReadModeAt04() override
        { return static_cast<std::uint8_t>(input_.IsWriting()); }
    [[nodiscard]] ParamsInputBytesBorrow CaptureInputAt14() override
        { return ParamsInputBytesBorrow::Bounded(input_.MutableBytes()); }
private:
    SerializeBuffer& input_;
};

class ParamsNativeSerialize18Input final : public IParamsSerializeInput
{
public:
    explicit ParamsNativeSerialize18Input(void* native18) noexcept : input_(native18) {}
    [[nodiscard]] std::uint8_t ReadModeAt04() override;
    [[nodiscard]] ParamsInputBytesBorrow CaptureInputAt14() override;
private:
    void* input_;
};
}
