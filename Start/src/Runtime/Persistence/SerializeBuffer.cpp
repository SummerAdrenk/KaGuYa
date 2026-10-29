#include <Runtime/SerializeBuffer.h>

#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const IEnvironmentKeyByteSource& SerializeBuffer::EnvironmentKeys() const
{
    if (!keys_) throw std::logic_error("SecretSC serialization requires an explicit environment key source");
    return *keys_;
}

bool SerializeBuffer::IsWriting() const noexcept
{
    return writing_;
}

const std::vector<std::byte>& SerializeBuffer::Bytes() const noexcept
{
    return bytes_;
}

std::vector<std::byte>& SerializeBuffer::MutableBytes() noexcept
{
    return bytes_;
}

std::uint32_t SerializeBuffer::Cursor() const noexcept
{
    return cursor_;
}

std::uint32_t& SerializeBuffer::MutableCursor() noexcept
{
    return cursor_;
}

void SerializeBuffer::ResetForWrite() noexcept
{
    // Start.exe:46D150。仅清空逻辑长度，不要求释放已分配容量。
    bytes_.clear();
    cursor_ = 0;
    writing_ = true;
}

void SerializeBuffer::SetWriting(const bool writing) noexcept
{
    // Start.exe:48D060。切换到写模式时清空正文；切换到读模式时保留正文。
    writing_ = writing;
    cursor_ = 0;
    if (writing_)
    {
        bytes_.clear();
    }
}

void SerializeBuffer::CopyStateFrom(const SerializeBuffer& source)
{
    // Start.exe:4659F0 同时复制正文、模式和游标。
    if (this == &source)
    {
        return;
    }
    bytes_ = source.bytes_;
    writing_ = source.writing_;
    cursor_ = source.cursor_;
}

void SerializeBuffer::SerializeState(SerializeBuffer& value)
{
    // Start.exe:465E80 按 buffer、mode、cursor 顺序序列化另一个 CSerialize。
    std::uint32_t byteCount = static_cast<std::uint32_t>(value.bytes_.size());
    SerializeSlot58Dword(byteCount);
    if (!writing_)
    {
        value.bytes_.resize(byteCount);
    }
    for (std::uint32_t index = 0; index < byteCount; ++index)
    {
        std::uint8_t byte = writing_ ? std::to_integer<std::uint8_t>(value.bytes_[index]) : 0;
        SerializeSlot50Byte(byte);
        if (!writing_)
        {
            value.bytes_[index] = static_cast<std::byte>(byte);
        }
    }

    std::uint8_t mode = value.writing_ ? 1 : 0;
    SerializeSlot54Byte(mode);
    value.writing_ = mode != 0;
    SerializeSlot58Dword(value.cursor_);
}

void SerializeBuffer::SerializeRaw(void* const value, const std::size_t byteCount)
{
    if (writing_)
    {
        const auto* const first = static_cast<const std::byte*>(value);
        bytes_.insert(bytes_.end(), first, first + byteCount);
        return;
    }

    // 原 CSerialize 对截断输入会越界读取。现代安全边界在本次写目标/推进游标前明确失败，
    // 不补零、不回退，也不回滚此前成功读取的字段。
    if (cursor_ > bytes_.size() || byteCount > bytes_.size() - cursor_)
        throw std::out_of_range("CSerialize scalar read exceeds the input buffer");
    std::memcpy(value, bytes_.data() + cursor_, byteCount);
    cursor_ += static_cast<std::uint32_t>(byteCount);
}

void SerializeBuffer::SerializeSlot50Byte(std::uint8_t& value)
{
    SerializeRaw(&value, sizeof(value));
}

void SerializeBuffer::SerializeSlot54Byte(std::uint8_t& value)
{
    SerializeRaw(&value, sizeof(value));
}

void SerializeBuffer::SerializeSlot40Word(std::uint16_t& value)
{
    SerializeRaw(&value, sizeof(value));
}

void SerializeBuffer::SerializeSlot48Word(std::uint16_t& value)
{
    SerializeRaw(&value, sizeof(value));
}

void SerializeBuffer::SerializeSlot38Dword(std::uint32_t& value)
{
    SerializeRaw(&value, sizeof(value));
}

void SerializeBuffer::SerializeSlot3CDword(std::uint32_t& value)
{
    SerializeRaw(&value, sizeof(value));
}

void SerializeBuffer::SerializeSlot44Dword(std::uint32_t& value)
{
    SerializeRaw(&value, sizeof(value));
}

void SerializeBuffer::SerializeSlot58Dword(std::uint32_t& value)
{
    SerializeRaw(&value, sizeof(value));
}

void SerializeBuffer::SerializeSlot4CString(std::string& value)
{
    if (writing_)
    {
        const std::size_t byteCount = value.size() + 1;
        bytes_.insert(bytes_.end(), reinterpret_cast<const std::byte*>(value.c_str()),
            reinterpret_cast<const std::byte*>(value.c_str()) + byteCount);
        return;
    }

    // 原 strlen 没有正文上界；缺少 NUL 不应依赖堆后方碰巧出现的字节。
    if (cursor_ >= bytes_.size())
        throw std::out_of_range("CSerialize string read exceeds the input buffer");
    const char* const text = reinterpret_cast<const char*>(bytes_.data() + cursor_);
    const auto* end = static_cast<const char*>(std::memchr(text, 0, bytes_.size() - cursor_));
    if (!end)
        throw std::out_of_range("CSerialize string has no terminator in the input buffer");
    const std::size_t byteCount = static_cast<std::size_t>(end - text);
    value.assign(text, byteCount);
    cursor_ += static_cast<std::uint32_t>(byteCount + 1);
}

void SerializeBuffer::SerializeSlot2CByteVector(std::vector<std::uint8_t>& value)
{
    std::uint32_t count = static_cast<std::uint32_t>(value.size());
    SerializeSlot58Dword(count);
    if (!writing_)
    {
        value.resize(count);
    }
    for (std::uint8_t& item : value)
    {
        SerializeSlot50Byte(item);
    }
}

void SerializeBuffer::SerializeSlot24WordVector(std::vector<std::uint16_t>& value)
{
    std::uint32_t count = static_cast<std::uint32_t>(value.size());
    SerializeSlot58Dword(count);
    if (!writing_)
    {
        value.resize(count);
    }
    for (std::uint16_t& item : value)
    {
        SerializeSlot40Word(item);
    }
}

void SerializeBuffer::SerializeSlot34DwordVector(std::vector<std::uint32_t>& value)
{
    std::uint32_t count = static_cast<std::uint32_t>(value.size());
    SerializeSlot58Dword(count);
    if (!writing_)
    {
        value.resize(count);
    }
    for (std::uint32_t& item : value)
    {
        SerializeSlot58Dword(item);
    }
}

void SerializeBuffer::SerializeSlot1CDwordVector(std::vector<std::uint32_t>& value)
{
    std::uint32_t count = static_cast<std::uint32_t>(value.size());
    SerializeSlot58Dword(count);
    if (!writing_)
    {
        value.resize(count);
    }
    for (std::uint32_t& item : value)
    {
        SerializeSlot38Dword(item);
    }
}

void SerializeBuffer::SerializeSlot20DwordVector(std::vector<std::uint32_t>& value)
{
    std::uint32_t count = static_cast<std::uint32_t>(value.size());
    SerializeSlot58Dword(count);
    if (!writing_)
    {
        value.resize(count);
    }
    for (std::uint32_t& item : value)
    {
        SerializeSlot3CDword(item);
    }
}

void SerializeBuffer::SerializeSlot28StringVector(std::vector<std::string>& value)
{
    std::uint32_t count = static_cast<std::uint32_t>(value.size());
    SerializeSlot58Dword(count);
    if (!writing_)
    {
        value.resize(count);
    }
    for (std::string& item : value)
    {
        SerializeSlot4CString(item);
    }
}

void SerializeBuffer::SerializeSlot30BitVector(std::vector<bool>& value)
{
    std::uint32_t count = static_cast<std::uint32_t>(value.size());
    SerializeSlot58Dword(count);
    if (!writing_)
    {
        value.assign(count, false);
    }
    for (std::uint32_t index = 0; index < count; ++index)
    {
        std::uint8_t item = writing_ && value[index] ? 1 : 0;
        SerializeSlot54Byte(item);
        if (!writing_)
        {
            value[index] = item != 0;
        }
    }
}
}
