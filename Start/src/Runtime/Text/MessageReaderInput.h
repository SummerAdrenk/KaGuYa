#pragma once

#include <Runtime/MessageReaderNative.h>

namespace KaGuYa::Start::Runtime
{
// 两种显式入口共用解析算法：原记录虚槽接任意IStreamFile，现代入口接指定文件API。
// 这些指针仅活在调用栈，绝不写入原reader/stream正文或control。
class MessageReaderInput final
{
public:
    explicit MessageReaderInput(IFileStreamRuntime* stream) noexcept : native_(stream) {}
    MessageReaderInput(FileStreamRuntime& stream, IStartFileApi& api) noexcept
        : native_(&stream), injected_(&stream), api_(&api) {}

    IFileStreamRuntime* Native() const noexcept { return native_; }
    bool Read(void* destination, DWORD size)
    {
        return (injected_ ? injected_->ReadExactWithApi(destination, size, *api_) :
            native_->ReadExactAtVslot10(destination, size)) != 0;
    }
    void RequireRead(void* destination, DWORD size, int error = 2)
    {
        if (!Read(destination, size)) throw error;
    }
    std::uint64_t Tell()
    {
        return injected_ ? injected_->TellWithApi(*api_) : native_->TellAtVslot18();
    }
    std::uint32_t TellLow() { return static_cast<std::uint32_t>(Tell()); }
    void Seek(std::uint64_t offset, DWORD method = FILE_BEGIN)
    {
        // 原懒查询/扫描不检查普通定位失败，但不捕获API抛出的异常。
        if (injected_) static_cast<void>(injected_->SeekWithApi(offset, method, *api_));
        else static_cast<void>(native_->SeekAtVslot14(offset, method));
    }
private:
    IFileStreamRuntime* native_;
    FileStreamRuntime* injected_ = nullptr;
    IStartFileApi* api_ = nullptr;
};

class OwnedMessageReaderInput final
{
public:
    OwnedMessageReaderInput(const MessageReaderCallContext& context, const char* path, int openError)
        : stream_(context.files), input_(stream_.Native(), context.files)
    {
        SECURITY_ATTRIBUTES attributes{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
        if (!stream_.OpenReaderWithApi({path, GENERIC_READ, 0, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL, &attributes}, context.transaction)) throw openError;
    }
    MessageReaderInput& Input() noexcept { return input_; }
private:
    ScopedFileStreamRuntime stream_;
    MessageReaderInput input_;
};

template<class Reference>
Reference CopyMessageReferenceForReturn(Reference& source) noexcept
{
    // 47B299/47B9D9：正常尾声只释放已捕获control；EH路径仍使用Ref自身析构。
    auto* const captured = source.RawSlot().controlAt08;
    Reference output(source);
    if (captured && --captured->referenceCountAt04 == 0)
    {
        if (captured->owningAt08) captured->DestroyPayloadAtVslot00();
        captured->DeleteControlAtVslot04(1);
    }
    source.RawSlot().controlAt08 = nullptr;
    return output;
}
}
