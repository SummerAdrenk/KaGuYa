#include <Runtime/MessageReaderNative.h>
#include "MessageReaderInput.h"
#include <Runtime/RuntimeCStringTemporary.h>

#include <array>
#include <cstring>
#include <mbstring.h>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
class LegacyPayload final
{
public:
    explicit LegacyPayload(std::span<const std::uint8_t> bytes) noexcept : bytes_(bytes) {}

    std::uint8_t Byte()
    {
        if (position_ == bytes_.size()) throw std::out_of_range("legacy message byte");
        return bytes_[position_++];
    }

    RuntimeCStringTemporary String()
    {
        const auto first = position_;
        while (position_ < bytes_.size() && bytes_[position_] != 0) ++position_;
        // 原 CString 从指针读到NUL；明确拒绝越界，不伪造缺失终止符。
        if (position_ == bytes_.size()) throw std::out_of_range("legacy message terminator");
        const auto length = position_++ - first;
        auto* manager = DefaultRuntimeStringManagerSource().CaptureAt417360();
        if (!manager) throw ATL::CAtlException(E_FAIL);
        // 直接返回纯右值，由 C++20 保证消除中间对象；不复制或移动临时字符串。
        return RuntimeCStringTemporary(reinterpret_cast<const char*>(bytes_.data() + first),
            static_cast<int>(length), manager);
    }

private:
    std::span<const std::uint8_t> bytes_;
    std::size_t position_ = 0;
};
}

NativeLegacyMessageRuntime::NativeLegacyMessageRuntime(IRuntimeStringManagerSource& strings)
    : NativeMessageReader(strings)
{
}
NativeLegacyMessageRuntime::~NativeLegacyMessageRuntime() noexcept
{
    offsetsAt10.value.~MessageOffsetVector();
    DestroyPathAt40CD40();
}
bool NativeLegacyMessageRuntime::Load(const char* path)
{
    Win32StartFileApi files;
    return LoadWithContext(path, {files});
}
bool NativeLegacyMessageRuntime::LoadWithContext(const char* path, const MessageReaderCallContext& context)
{
    try
    {
        // 484AEE..484B08 仅在入口清offset/path，不重置版本和XOR字段。
        offsetsAt10.value.clear();
        Path().SetString("", 0);
        OwnedMessageReaderInput opened(context, path, 1);
        auto& stream = opened.Input();
        {
            // 484B8F..484C3A：与Message2相同的独立Ref，不以栈数组抹掉分配/释放事件。
            ByteArrayReferenceRuntime magic;
            magic.AllocateAt412700(17);
            std::memset(magic.Data(), 0, magic.Size());
            if (!stream.Read(magic.Data(), 16)) throw 2;
            if (_mbscmp(reinterpret_cast<const unsigned char*>("[SCR-MESSAGE]ver"), magic.Data()) != 0) throw 3;
        }
        std::uint8_t marker;
        stream.RequireRead(&marker, 1, 4);
        versionAt04 = marker == 3 ? 3.0f : 2.0f;
        if (marker == 3) stream.RequireRead(&marker, 1, 4);
        encryptedAt08 = marker != 0;
        stream.RequireRead(&keyAt09, 1, 5);
        std::uint32_t length;
        while (stream.Read(&length, sizeof(length)))
        {
            offsetsAt10.value.push_back(stream.TellLow() - 4);
            // high DWORD=0，因此length是无符号相对位移，不符号扩展LONG。
            stream.Seek(length, FILE_CURRENT);
        }
        // 484D48 -> 410290：若文件服务接受空指针，最终路径仍按原分支清空。
        Path().SetString(path, path ? static_cast<int>(std::strlen(path)) : 0);
        return true;
    }
    catch (...) // 5E5F78 states0..2 ->484D6B，保留已扫描偏移和版本。
    {
        encryptedAt08 = 1;
        keyAt09 = 0xFF;
        Path().SetString("", 0);
        return false;
    }
}

Message2FormatRef NativeLegacyMessageRuntime::GetFormat(std::int32_t index)
{
    Win32StartFileApi files;
    return GetFormatWithContext(index, {files});
}
Message2FormatRef NativeLegacyMessageRuntime::GetFormatWithContext(
    const std::int32_t index, const MessageReaderCallContext& context)
{
    // 47A5B0..47A5FB 在原局部catch建立前分配结果，分配失败不能转成空引用。
    auto result = MakeMessage2Format();
    try
    {
        if (index < 0 || index >= GetFormatCount()) throw 1;
        OwnedMessageReaderInput opened(context, Path().GetString(), 2);
        auto& stream = opened.Input();
        stream.Seek(offsetsAt10.value[static_cast<std::size_t>(index)]);
        std::uint32_t length;
        if (!stream.Read(&length, sizeof(length))) throw 3;
        // 原signed负长度不分配，随后会以空指针读取巨量字节；明确拒绝该非法输入。
        if (length > 0x7FFFFFFFu) throw std::out_of_range("legacy message negative payload size");
        ByteArrayReferenceRuntime bytes;
        bytes.AllocateAt412700(length); // 47A6CD，同Message2使用原byte[] Ref，不清零。
        if (!stream.Read(bytes.Data(), length)) throw 4;
        DecodeReferenceAt46DF80(bytes);
        LegacyPayload payload(bytes.Bytes());
        {
            const auto name = payload.String();
            if (!name.IsEmpty()) result->name.ReplaceOwnedAt451A80(AllocateMessageTextCommand(name.GetString(), 0));
        } // 47A762：读取消息数量前释放姓名临时CString。
        const auto messageCount = payload.Byte();
        for (unsigned int i = 0; i < messageCount; ++i)
        {
            auto message = MakeMessageVoiceAndText();
            if (ReadVersionAt47DDE0() == 2.0f)
            {
                auto voice = payload.String();
                if (!voice.IsEmpty())
                {
                    message->voices.RebuildAt412870(1);
                    message->voices.At(0) = voice.Value();
                }
            }
            else if (ReadVersionAt47DDE0() == 3.0f)
            {
                const auto voiceCount = payload.Byte();
                if (voiceCount != 0)
                {
                    message->voices.RebuildAt412870(voiceCount);
                    for (std::uint32_t voiceIndex = 0; voiceIndex < voiceCount; ++voiceIndex)
                    {
                        const auto voice = payload.String();
                        // 47A8F7 / 47A911：空项仍占槽，但不共享临时 CString 的 nil。
                        if (voice.IsEmpty()) message->voices.At(voiceIndex).Empty();
                        else message->voices.At(voiceIndex) = voice.Value();
                    }
                }
            }
            else throw 5;
            // 空正文仍分配text对象；语音是原窄字符串，不走Message2的UTF-16转换。
            {
                const auto text = payload.String();
                message->text.ReplaceOwnedAt451A80(AllocateMessageTextCommand(text.GetString(), 0));
            } // 47A9BC..47A9D8：插入vector前释放正文临时CString。
            result->messages.push_back(message);
        }
        // 原函数不要求末尾恰好耗尽负载。
    }
    catch (...) // 5E5EB8 states1..12 ->47AAE3 清结果引用。
    {
        result.reset();
    }
    // 47AA95的返回槽复制发生在释放负载/关闭流之后，局部Format仍需单独释放。
    auto* const captured = result.RawSlot().controlAt08; // 47AA92的EBX。
    Message2FormatRef output(result);
    result.ReleaseCapturedOnReturn(captured);
    return output;
}

MessageTextCommandReference NativeLegacyMessageRuntime::GetChoice(std::int32_t) { return {}; }
MessageVoiceAndTextRef NativeLegacyMessageRuntime::GetMessage(std::int32_t) { return {}; }
std::int32_t NativeLegacyMessageRuntime::GetFormatCount() const noexcept
{
    return static_cast<std::int32_t>(offsetsAt10.value.size());
}
std::int32_t NativeLegacyMessageRuntime::GetChoiceCount() const noexcept { return 0; }
std::int32_t NativeLegacyMessageRuntime::GetMessageCount() const noexcept { return 0; }
}
