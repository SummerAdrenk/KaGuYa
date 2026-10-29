#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class IEnvironmentKeyByteSource;
class SerializeBuffer final
{
public:
    SerializeBuffer() = default;
    // 显式运行环境依赖，仅属于现代serializer适配器，不进入原生归档正文。
    explicit SerializeBuffer(const IEnvironmentKeyByteSource& keys) noexcept : keys_(&keys) {}
    [[nodiscard]] const IEnvironmentKeyByteSource& EnvironmentKeys() const;

    [[nodiscard]] bool IsWriting() const noexcept;
    [[nodiscard]] const std::vector<std::byte>& Bytes() const noexcept;
    [[nodiscard]] std::vector<std::byte>& MutableBytes() noexcept;
    [[nodiscard]] std::uint32_t Cursor() const noexcept;
    [[nodiscard]] std::uint32_t& MutableCursor() noexcept;

    void ResetForWrite() noexcept;
    void SetWriting(bool writing) noexcept;
    void CopyStateFrom(const SerializeBuffer& source);
    void SerializeState(SerializeBuffer& value);

    void SerializeSlot50Byte(std::uint8_t& value);
    void SerializeSlot54Byte(std::uint8_t& value);
    void SerializeSlot40Word(std::uint16_t& value);
    void SerializeSlot48Word(std::uint16_t& value);
    void SerializeSlot38Dword(std::uint32_t& value);
    void SerializeSlot3CDword(std::uint32_t& value);
    void SerializeSlot44Dword(std::uint32_t& value);
    void SerializeSlot58Dword(std::uint32_t& value);
    void SerializeSlot4CString(std::string& value);

    void SerializeSlot2CByteVector(std::vector<std::uint8_t>& value);
    void SerializeSlot24WordVector(std::vector<std::uint16_t>& value);
    void SerializeSlot34DwordVector(std::vector<std::uint32_t>& value);
    void SerializeSlot1CDwordVector(std::vector<std::uint32_t>& value);
    void SerializeSlot20DwordVector(std::vector<std::uint32_t>& value);
    void SerializeSlot28StringVector(std::vector<std::string>& value);
    void SerializeSlot30BitVector(std::vector<bool>& value);

private:
    void SerializeRaw(void* value, std::size_t byteCount);

    std::vector<std::byte> bytes_;
    bool writing_ = true;
    std::uint32_t cursor_ = 0;
    const IEnvironmentKeyByteSource* keys_ = nullptr;
};
}
