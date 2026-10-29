#include <Runtime/MessageReaderNative.h>
#include "MessageReaderInput.h"

#include <algorithm>
#include <bit>
#include <cstring>
#include <mbstring.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Bytes = std::span<const std::uint8_t>;

void RequireRange(const Bytes& bytes, const std::size_t position, const std::size_t count)
{
    // 安全差异：原 PE 的 43D230/410420 只做指针运算，不检查内部边界。
    // 将原版非法内存访问改为 C++ 异常，由原入口的 catch 边界返回空引用或 false。
    if (position > bytes.size() || count > bytes.size() - position)
        throw std::out_of_range("Message2 payload bounds");
}

ByteArrayReferenceRuntime ReadPayload(MessageReaderInput& stream, const bool shortLength)
{
    std::int32_t size;
    if (shortLength)
    {
        // 47D59C使用stream参数槽的低WORD接长度；短读不能预先填零。
        auto slot = reinterpret_cast<std::uintptr_t>(stream.Native());
        stream.RequireRead(&slot, 2);
        size = static_cast<std::uint16_t>(slot);
    }
    else stream.RequireRead(&size, 4); // 47B39D：未初始化的局部DWORD。
    if (size < 0)
        throw std::out_of_range("Message2 negative payload size");
    ByteArrayReferenceRuntime bytes;
    // 47D5D5/47B3D4：独立分配byte正文与control，保留原未写尾部及失败部分发布。
    bytes.AllocateAt412700(static_cast<std::uint32_t>(size));
    stream.RequireRead(bytes.Data(), static_cast<DWORD>(size));
    return bytes;
}

const char* CheckedTextSource(
    const Bytes& bytes, const std::size_t position, const std::int32_t length)
{
    if (length < 0)
        throw std::invalid_argument("Message2 negative text length");
    RequireRange(bytes, position, static_cast<std::size_t>(length));
    std::size_t end = position;
    if (length == 0)
    {
        // 45EE00：Size==0 走 strlen(Src)；零长度姓名的 Src 则为空指针。
        if (!bytes.empty())
        {
            while (end < bytes.size() && bytes[end] != 0)
                ++end;
            RequireRange(bytes, end, 1);
        }
    }
    // 正长度必须交给45EE00原样复制；这里只拒绝越界，不提前截短内嵌NUL。
    return bytes.empty() ? nullptr : reinterpret_cast<const char*>(bytes.data() + position);
}

RuntimeCString ReadVoice(const Bytes& bytes, const std::size_t position)
{
    auto* manager = DefaultRuntimeStringManagerSource().CaptureAt417360();
    if (!manager) throw ATL::CAtlException(E_FAIL);
    RuntimeCString result(manager);
    for (auto cursor = position; ; cursor += 2)
    {
        RequireRange(bytes, cursor, 2);
        const auto value = static_cast<wchar_t>(bytes[cursor] | (bytes[cursor + 1] << 8));
        if (value == 0)
            break;
    }
    const auto* wide = reinterpret_cast<const wchar_t*>(bytes.data() + position);
    // 4102D0：保留 CP_THREAD_ACP、输入长度 -1、所需长度减一的输出容量，
    // 并按原版不检查第二次转换的返回值；CString 随后自行补上 NUL。
    const int size = WideCharToMultiByte(CP_THREAD_ACP, 0, wide, -1,
        nullptr, 0, nullptr, nullptr) - 1;
    if (size <= 0)
        return result;
    // 41030E..410337：分配 CString 后直接转换，最后按第一次测得的长度提交。
    // 第二次失败不能清零、重试或按 strlen 缩短，原路径仍保留已写入字节。
    char* destination = result.GetBuffer(size);
    static_cast<void>(WideCharToMultiByte(CP_THREAD_ACP, 0, wide, -1,
        destination, size, nullptr, nullptr));
    result.ReleaseBufferSetLength(size);
    return result;
}

MessageVoiceAndTextRef ParseMessage(const Bytes& bytes)
{
    RequireRange(bytes, 0, 4);
    const auto bits = static_cast<std::uint32_t>(bytes[0]) |
        (static_cast<std::uint32_t>(bytes[1]) << 8) |
        (static_cast<std::uint32_t>(bytes[2]) << 16) |
        (static_cast<std::uint32_t>(bytes[3]) << 24);
    const auto textLength = std::bit_cast<std::int32_t>(bits);
    auto result = MakeMessageVoiceAndText();
    result->text.ReplaceOwnedAt451A80(AllocateMessageTextCommand(
        CheckedTextSource(bytes, 4, textLength), textLength));
    auto position = 4u + static_cast<std::size_t>(textLength);
    RequireRange(bytes, position, 1);
    const auto voiceCount = bytes[position++];
    if (voiceCount != 0)
    {
        result->voices.RebuildAt412870(voiceCount);
        for (std::uint32_t index = 0; index < voiceCount; ++index)
        {
            const auto voice = ReadVoice(bytes, position);
            // 47B59E..47B5A4 按转换后的窄字符串字节数推进，不是按 wcslen 推进。
            position += 2 * static_cast<std::size_t>(voice.GetLength()) + 2;
            // 47B5F8 与47B63F分别走清空/共享赋值，空串不得换用临时串的manager。
            if (voice.IsEmpty()) result->voices.At(index).Empty();
            else result->voices.At(index) = voice;
        }
    }
    // 47B370 不要求最终游标恰好消费整个负载。
    // 47B679..47B6AE 先增持返回槽，再释放局部记录，不能把此步变成移动清空。
    return MessageVoiceAndTextRef(result);
}

template<class Group>
std::int32_t GroupCount(const Group& group) noexcept
{
    return static_cast<std::int32_t>(group.mode ? group.cache.size() : group.offsets.size());
}
}

NativeMessage2Runtime::NativeMessage2Runtime(IRuntimeStringManagerSource& strings)
    : NativeMessageReader(strings)
{
    // 45A7F1..45A7FD：只写四个mode byte，累计量和三个padding byte均保持未写。
    Groups().formats.mode = 0;
    Groups().bodies.mode = 0;
    Groups().choices.mode = 0;
    Groups().names.mode = 0;
}

NativeMessage2Runtime::~NativeMessage2Runtime() noexcept
{
    // 464B00：按组逆序，各组cache先于offset；路径最后才取源，不能预先Clear。
    Groups().~NativeMessage2Groups();
    DestroyPathAt40CD40();
}

void NativeMessage2Runtime::ClearAt46D340()
{
    encryptedAt08 = 1;
    keyAt09 = 0xFF;
    Path().SetString("", 0);
    auto clear = [](auto& group) noexcept {
        group.offsets.clear();
        group.cache.clear();
    };
    clear(Groups().names);
    clear(Groups().choices);
    clear(Groups().bodies);
    clear(Groups().formats);
}

MessageTextCommandReference NativeMessage2Runtime::ReadString(MessageReaderInput& stream, bool skip)
{
    auto bytes = ReadPayload(stream, true);
    if (skip) return {};
    DecodeReferenceAt46DF80(bytes);
    const auto length = static_cast<std::int32_t>(bytes.Size());
    return MakeMessageTextCommand(CheckedTextSource(bytes.Bytes(), 0, length), length);
}

MessageVoiceAndTextRef NativeMessage2Runtime::ReadMessage(MessageReaderInput& stream, bool skip)
{
    auto bytes = ReadPayload(stream, false);
    if (skip) return {};
    DecodeReferenceAt46DF80(bytes);
    return ParseMessage(bytes.Bytes());
}

MessageRawFormatRef NativeMessage2Runtime::ReadFormat(MessageReaderInput& stream, bool skip)
{
    MessageRawFormatRef result;
    if (!skip) ConstructMessageRawFormatAt47AB80(result.RawSlot());
    // 47AC96：先发布正文/control；name和所有message编号复用同一未初始化局部DWORD。
    std::int32_t value;
    stream.RequireRead(&value, 4);
    if (!skip) result->nameId = value;
    std::uint8_t count;
    stream.RequireRead(&count, 1);
    for (unsigned int i = 0; i < count; ++i)
    {
        stream.RequireRead(&value, 4);
        if (!skip) result->messages.push_back(value);
    }
    return result;
}

MessageTextCommandReference NativeMessage2Runtime::ReadStringAt47D570(IFileStreamRuntime* stream, bool skip)
{
    MessageReaderInput input(stream);
    return ReadString(input, skip);
}
MessageVoiceAndTextRef NativeMessage2Runtime::ReadMessageAt47B370(IFileStreamRuntime* stream, bool skip)
{
    MessageReaderInput input(stream);
    return ReadMessage(input, skip);
}
MessageRawFormatRef NativeMessage2Runtime::ReadFormatAt47AB80(IFileStreamRuntime* stream, bool skip)
{
    MessageReaderInput input(stream);
    return ReadFormat(input, skip);
}

bool NativeMessage2Runtime::Load(const char* path)
{
    Win32StartFileApi files;
    return LoadWithContext(path, {files});
}

bool NativeMessage2Runtime::LoadWithContext(const char* path, const MessageReaderCallContext& context)
{
    try
    {
        ClearAt46D340();
        OwnedMessageReaderInput opened(context, path, 1);
        auto& stream = opened.Input();
        {
            // 48449F..48454B：签名单独分配17-byte Ref、清零、读16字节，比较后即释放。
            ByteArrayReferenceRuntime magic;
            magic.AllocateAt412700(17);
            std::memset(magic.Data(), 0, magic.Size());
            stream.RequireRead(magic.Data(), 16, 2);
            if (_mbscmp(reinterpret_cast<const unsigned char*>("[SCR-MESSAGE]ver"), magic.Data()) != 0)
                throw 3;
        }
        std::uint8_t marker;
        stream.RequireRead(&marker, 1, 4);
        if (marker != '4') throw -1;
        std::uint16_t suffix;
        stream.RequireRead(&suffix, 2, 4); // 原版不校验'.0'。
        versionAt04 = 4.0f;
        stream.RequireRead(&marker, 1, 4); // 484596复用刚刚读到'4'的局部byte。
        encryptedAt08 = marker != 0;
        stream.RequireRead(&keyAt09, 1, 5); // 直接写字段，不经预清零临时量。

        std::int32_t count;
        auto loadGroup = [&](auto& group, auto readRecord, std::int32_t& countSlot) {
            stream.RequireRead(&countSlot, 4, 4);
            group.total = 0; // count成功后才初始化本组total，失败不撤回前面已提交的组。
            for (std::int32_t i = 0; i < countSlot; ++i)
            {
                const auto before = stream.TellLow();
                if (group.mode)
                {
                    const auto value = readRecord(false);
                    group.cache.push_back(value);
                }
                else
                {
                    group.offsets.push_back(before);
                    static_cast<void>(readRecord(true));
                }
                // 原码释放记录临时Ref之后再Tell，低DWORD相减并自然回绕。
                group.total += stream.TellLow() - before;
            }
        };
        loadGroup(Groups().names, [&](bool skip) { return ReadString(stream, skip); }, count);
        loadGroup(Groups().choices, [&](bool skip) { return ReadString(stream, skip); }, count);
        loadGroup(Groups().bodies, [&](bool skip) { return ReadMessage(stream, skip); }, count);
        std::int32_t formatCount; // 4847D8使用另一局部槽，不继承第三组count的未写尾部。
        loadGroup(Groups().formats, [&](bool skip) { return ReadFormat(stream, skip); }, formatCount);
        Path().SetString(path, path ? static_cast<int>(std::strlen(path)) : 0);
        return true; // 484965..484985：读完四组即成功，保留未消费的文件尾部。
    }
    catch (...) // 5E63F4 -> 5E63E0 -> 5E63D0 -> 484988，原版并非只捕获 int。
    {
        ClearAt46D340();
        return false;
    }
}

MessageVoiceAndTextRef NativeMessage2Runtime::GetMessage(std::int32_t index)
{
    Win32StartFileApi files;
    return GetMessageWithContext(index, {files});
}

MessageVoiceAndTextRef NativeMessage2Runtime::GetMessageWithContext(
    const std::int32_t index, const MessageReaderCallContext& context)
{
    try
    {
        if (index < 0 || index >= GetMessageCount()) throw 1;
        MessageVoiceAndTextRef result;
        if (Groups().bodies.mode) result = Groups().bodies.cache[static_cast<std::size_t>(index)];
        else
        {
            OwnedMessageReaderInput opened(context, Path().GetString(), 2);
            auto& stream = opened.Input();
            stream.Seek(Groups().bodies.offsets[static_cast<std::size_t>(index)]);
            const auto loaded = ReadMessage(stream, false);
            result = loaded;
        }
        return CopyMessageReferenceForReturn(result);
    }
    catch (...) // 47B2D2/47B2D8：原版将输出引用的三个字段全部清零。
    {
        return nullptr;
    }
}

MessageTextCommandReference NativeMessage2Runtime::GetStringWithContext(
    std::int32_t index, bool name, const MessageReaderCallContext& context)
{
    try
    {
        if (index < 0 || index >= (name ? GetNameCount() : GetChoiceCount())) throw 1;
        auto& group = name ? Groups().names : Groups().choices;
        MessageTextCommandReference result;
        // 47B901确实读format.mode；不能修成看似对称的name.mode。
        const bool cached = name ? Groups().formats.mode != 0 : group.mode != 0;
        if (cached) result = group.cache[static_cast<std::size_t>(index)];
        else
        {
            OwnedMessageReaderInput opened(context, Path().GetString(), 2);
            auto& stream = opened.Input();
            stream.Seek(group.offsets[static_cast<std::size_t>(index)]);
            const auto loaded = ReadString(stream, false);
            result = loaded;
        }
        return CopyMessageReferenceForReturn(result);
    }
    catch (...) // 47B8A0 与 47CF00 同样在失败时返回空引用。
    {
        return nullptr;
    }
}

MessageTextCommandReference NativeMessage2Runtime::GetName(std::int32_t index)
{
    Win32StartFileApi files;
    return GetNameWithContext(index, {files});
}
MessageTextCommandReference NativeMessage2Runtime::GetChoice(std::int32_t index)
{
    Win32StartFileApi files;
    return GetChoiceWithContext(index, {files});
}
MessageTextCommandReference NativeMessage2Runtime::GetNameWithContext(
    std::int32_t index, const MessageReaderCallContext& context)
{
    return GetStringWithContext(index, true, context);
}
MessageTextCommandReference NativeMessage2Runtime::GetChoiceWithContext(
    std::int32_t index, const MessageReaderCallContext& context)
{
    return GetStringWithContext(index, false, context);
}
Message2FormatRef NativeMessage2Runtime::GetFormat(std::int32_t index)
{
    Win32StartFileApi files;
    return GetFormatWithContext(index, {files});
}
Message2FormatRef NativeMessage2Runtime::GetFormatWithContext(
    const std::int32_t index, const MessageReaderCallContext& context)
{
    try
    {
        if (index < 0 || index >= GetFormatCount()) throw 1;
        MessageRawFormatRef disk;
        if (Groups().formats.mode) disk = Groups().formats.cache[static_cast<std::size_t>(index)];
        else
        {
            OwnedMessageReaderInput opened(context, Path().GetString(), 2);
            auto& stream = opened.Input();
            stream.Seek(Groups().formats.offsets[static_cast<std::size_t>(index)]);
            const auto loaded = ReadFormat(stream, false);
            disk = loaded;
        } // 47A2B0 在嵌套查询前关闭文件；共享模式为零，不能重叠打开。
        Message2FormatRef output;
        YtlReferenceControlRuntime* capturedRawControl;
        {
            auto result = MakeMessage2Format();
            capturedRawControl = disk.RawSlot().controlAt08; // 47A2FE保存EDI，正常尾声不重取。
            if (disk->nameId >= 0)
            {
                const auto count = GetNameCount();
                if (disk->nameId >= count)
                    throw 2;
                const auto name = GetNameWithContext(disk->nameId, context);
                result->name = name;
            }
            // 47A3A6只固定迭代指针；47A3C8每轮重新取end，不把原范围复制成编号快照。
            for (auto* cursor = disk->messages.begin(); cursor != disk->messages.end(); ++cursor)
            {
                if (*cursor < 0) throw 3;
                const auto count = GetMessageCount();
                const auto messageIndex = *cursor;
                if (messageIndex >= count) throw 3;
                // 47A190 即使收到嵌套 +24 的 catch 返回的空引用，也仍追加该项。
                const auto message = GetMessageWithContext(messageIndex, context);
                result->messages.push_back(message);
            }
            // 47A475先增持返回槽，再按47A48A重读控制块释放局部resolved。
            output = result;
        }
        disk.ReleaseCapturedOnReturn(capturedRawControl); // 47A4C2仅正常尾声使用捕获的控制块。
        return output;
    }
    catch (...) // 原版捕获入口：5E62D8 -> 5E62C8 -> 5E62B8 -> 47A4F7。
    {
        return nullptr;
    }
}

std::int32_t NativeMessage2Runtime::GetNameCount() const noexcept
{
    return GroupCount(Groups().names);
}

std::int32_t NativeMessage2Runtime::GetChoiceCount() const noexcept
{
    return GroupCount(Groups().choices);
}

std::int32_t NativeMessage2Runtime::GetMessageCount() const noexcept
{
    return GroupCount(Groups().bodies);
}

std::int32_t NativeMessage2Runtime::GetFormatCount() const noexcept
{
    return GroupCount(Groups().formats);
}
}
