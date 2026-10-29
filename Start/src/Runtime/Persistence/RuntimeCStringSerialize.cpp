#include <Runtime/RuntimeCStringSerialize.h>
#include <Runtime/SerializeBuffer.h>
#include <bit>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
void SerializeRuntimeCStringAt465ED0(SerializeBuffer& serializer, RuntimeCString& value)
{
    auto& bytes = serializer.MutableBytes();
    if (serializer.IsWriting())
    {
        // 465EE8固定chars；长度加1按x86有符号值判断，逐byte追加保留失败前缀。
        const auto* text = reinterpret_cast<const std::byte*>(value.GetString());
        const auto count = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(value.GetLength()) + 1u);
        for (std::int32_t index = 0; index < count; ++index) bytes.push_back(text[index]);
        return;
    }
    const auto cursor = serializer.Cursor();
    if (cursor >= bytes.size())
        throw std::out_of_range("CSerialize CString read exceeds the input buffer");
    const auto* text = reinterpret_cast<const char*>(bytes.data() + cursor);
    const auto* end = static_cast<const char*>(std::memchr(text, 0, bytes.size() - cursor));
    // 原strlen越界不具备可移植行为；与既有标量适配器一样，显式拒绝截断输入。
    if (!end) throw std::out_of_range("CSerialize CString has no terminator in the input buffer");
    const auto length = static_cast<std::uint32_t>(end - text);
    SetRuntimeCStringAt437680(value, text, std::bit_cast<std::int32_t>(length));
    // 465F7D在Set成功后累加当前游标，不把管理器回调前的cursor写回。
    serializer.MutableCursor() += length + 1u;
}
}
