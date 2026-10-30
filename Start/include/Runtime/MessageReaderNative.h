#pragma once

#include <Runtime/ByteArrayReferenceRuntime.h>
#include <Runtime/FileStreamRuntime.h>
#include <Runtime/MessageReaderVectors.h>

namespace KaGuYa::Start::Runtime
{
// 仅属于现代调用栈；原reader正文和control中没有文件服务/事务上下文。
struct MessageReaderCallContext final
{
    IStartFileApi& files;
    const FileTransactionContext* transaction = nullptr;
};

class NativeMessageReader
{
public:
    explicit NativeMessageReader(IRuntimeStringManagerSource& strings);
    NativeMessageReader(const NativeMessageReader&) = delete;
    NativeMessageReader& operator=(const NativeMessageReader&) = delete;

    // 5B25B8及三个具体表的公共十槽；不得在前面或末尾添加虚析构。
    virtual void DecodeBufferAt46DFD0(std::uint8_t* buffer, std::int32_t length);
    virtual void DecodeReferenceAt46DF80(const ByteArrayReferenceRuntime& bytes);
    virtual float ReadVersionAt47DDE0() const;
    virtual bool Load(const char* path) = 0;
    virtual std::int32_t GetFormatCount() const noexcept = 0;
    virtual Message2FormatRef GetFormat(std::int32_t index) = 0;
    virtual std::int32_t GetChoiceCount() const noexcept = 0;
    virtual MessageTextCommandReference GetChoice(std::int32_t index) = 0;
    virtual std::int32_t GetMessageCount() const noexcept = 0;
    virtual MessageVoiceAndTextRef GetMessage(std::int32_t index) = 0;

    RuntimeCString& Path() noexcept { return pathAt0C.value; }
    const RuntimeCString& Path() const noexcept { return pathAt0C.value; }
    void DestroyPathAt40CD40();

    // 45A71A/45A8D1/45EDB5均不初始化+04..+0B。
    float versionAt04;
    std::uint8_t encryptedAt08;
    std::uint8_t keyAt09;
    union PathStorage
    {
        PathStorage() noexcept {}
        ~PathStorage() noexcept {}
        RuntimeCString value;
    } pathAt0C;
protected:
    // 三个具体销毁入口显式释放路径；此处既不释放，也不添加删除虚槽。
    ~NativeMessageReader() noexcept = default;
};
static_assert(sizeof(NativeMessageReader) == 0x10);
static_assert(offsetof(NativeMessageReader, versionAt04) == 0x04);
static_assert(offsetof(NativeMessageReader, encryptedAt08) == 0x08);
static_assert(offsetof(NativeMessageReader, keyAt09) == 0x09);
static_assert(offsetof(NativeMessageReader, pathAt0C) == 0x0C);

template<class Cache>
struct NativeMessage2Group final
{
    // 用户定义的空构造防止value-init抹掉mode/padding/total；两vector各自清三指针。
    NativeMessage2Group() noexcept {}
    std::uint8_t mode;
    MessageOffsetVector offsets;
    Cache cache;
    std::uint32_t total;
};
using NativeMessage2StringGroup = NativeMessage2Group<MessageTextCommandReferenceVector>;
using NativeMessage2BodyGroup = NativeMessage2Group<MessageReferenceVector>;
using NativeMessage2FormatGroup = NativeMessage2Group<MessageRawFormatReferenceVector>;
static_assert(sizeof(NativeMessage2StringGroup) == 0x20);
static_assert(sizeof(NativeMessage2BodyGroup) == 0x20);
static_assert(sizeof(NativeMessage2FormatGroup) == 0x20);
static_assert(offsetof(NativeMessage2StringGroup, offsets) == 4);
static_assert(offsetof(NativeMessage2StringGroup, cache) == 0x10);
static_assert(offsetof(NativeMessage2StringGroup, total) == 0x1C);

struct NativeMessage2Groups final
{
    NativeMessage2Groups() noexcept {}
    NativeMessage2StringGroup names;
    NativeMessage2StringGroup choices;
    NativeMessage2BodyGroup bodies;
    NativeMessage2FormatGroup formats;
};
static_assert(sizeof(NativeMessage2Groups) == 0x80);
static_assert(offsetof(NativeMessage2Groups, choices) == 0x20);
static_assert(offsetof(NativeMessage2Groups, bodies) == 0x40);
static_assert(offsetof(NativeMessage2Groups, formats) == 0x60);

class MessageReaderInput;

// 具体Message2的读取/姓名槽必须保留在原虚表中，不能因final删除virtual。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunnecessary-virtual-specifier"
#endif
class NativeMessage2Runtime final : public NativeMessageReader
{
public:
    explicit NativeMessage2Runtime(IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    ~NativeMessage2Runtime() noexcept;
    bool Load(const char* path) override;
    std::int32_t GetFormatCount() const noexcept override;
    Message2FormatRef GetFormat(std::int32_t index) override;
    std::int32_t GetChoiceCount() const noexcept override;
    MessageTextCommandReference GetChoice(std::int32_t index) override;
    std::int32_t GetMessageCount() const noexcept override;
    MessageVoiceAndTextRef GetMessage(std::int32_t index) override;
    virtual MessageTextCommandReference ReadStringAt47D570(IFileStreamRuntime* stream, bool skip);
    virtual std::int32_t GetNameCount() const noexcept;
    virtual MessageTextCommandReference GetName(std::int32_t index);
    virtual MessageVoiceAndTextRef ReadMessageAt47B370(IFileStreamRuntime* stream, bool skip);
    virtual MessageRawFormatRef ReadFormatAt47AB80(IFileStreamRuntime* stream, bool skip);

    bool LoadWithContext(const char* path, const MessageReaderCallContext& context);
    Message2FormatRef GetFormatWithContext(std::int32_t index, const MessageReaderCallContext& context);
    MessageTextCommandReference GetChoiceWithContext(std::int32_t index, const MessageReaderCallContext& context);
    MessageVoiceAndTextRef GetMessageWithContext(std::int32_t index, const MessageReaderCallContext& context);
    MessageTextCommandReference GetNameWithContext(std::int32_t index, const MessageReaderCallContext& context);
    void ClearAt46D340();

    // 命名union禁止路径释放回调之后再次自动析构已清理的vector。
    union GroupsStorage
    {
        GroupsStorage() noexcept : value() {}
        ~GroupsStorage() noexcept {}
        NativeMessage2Groups value;
    } groupsAt10;
    NativeMessage2Groups& Groups() noexcept { return groupsAt10.value; }
    const NativeMessage2Groups& Groups() const noexcept { return groupsAt10.value; }
private:
    MessageTextCommandReference ReadString(MessageReaderInput& input, bool skip);
    MessageVoiceAndTextRef ReadMessage(MessageReaderInput& input, bool skip);
    MessageRawFormatRef ReadFormat(MessageReaderInput& input, bool skip);
    MessageTextCommandReference GetStringWithContext(std::int32_t index, bool name,
        const MessageReaderCallContext& context);
};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
static_assert(sizeof(NativeMessage2Runtime) == 0x90);
static_assert(offsetof(NativeMessage2Runtime, groupsAt10) == 0x10);

class NativeLegacyMessageRuntime final : public NativeMessageReader
{
public:
    explicit NativeLegacyMessageRuntime(IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    ~NativeLegacyMessageRuntime() noexcept;
    bool Load(const char* path) override;
    std::int32_t GetFormatCount() const noexcept override;
    Message2FormatRef GetFormat(std::int32_t index) override;
    std::int32_t GetChoiceCount() const noexcept override;
    MessageTextCommandReference GetChoice(std::int32_t index) override;
    std::int32_t GetMessageCount() const noexcept override;
    MessageVoiceAndTextRef GetMessage(std::int32_t index) override;
    bool LoadWithContext(const char* path, const MessageReaderCallContext& context);
    Message2FormatRef GetFormatWithContext(std::int32_t index, const MessageReaderCallContext& context);
    union OffsetStorage
    {
        OffsetStorage() noexcept : value() {}
        ~OffsetStorage() noexcept {}
        MessageOffsetVector value;
    } offsetsAt10;
};
static_assert(sizeof(NativeLegacyMessageRuntime) == 0x1C);
static_assert(offsetof(NativeLegacyMessageRuntime, offsetsAt10) == 0x10);

class NativeNullMessageRuntime final : public NativeMessageReader
{
public:
    explicit NativeNullMessageRuntime(IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    ~NativeNullMessageRuntime() noexcept(false);
    float ReadVersionAt47DDE0() const override;
    bool Load(const char* path) override;
    std::int32_t GetFormatCount() const noexcept override;
    Message2FormatRef GetFormat(std::int32_t index) override;
    std::int32_t GetChoiceCount() const noexcept override;
    MessageTextCommandReference GetChoice(std::int32_t index) override;
    std::int32_t GetMessageCount() const noexcept override;
    MessageVoiceAndTextRef GetMessage(std::int32_t index) override;
};
static_assert(sizeof(NativeNullMessageRuntime) == 0x10);

// 45A5D0/4519E0的raw正文分配边界。构造抛出时原EH不回收此raw块。
NativeMessage2Runtime* AllocateNativeMessage2Runtime(
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
NativeLegacyMessageRuntime* AllocateNativeLegacyMessageRuntime(
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
NativeNullMessageRuntime* AllocateNativeNullMessageRuntime(
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
