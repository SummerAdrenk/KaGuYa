#pragma once

#include <Runtime/EnvironmentRootRuntime.h>

#include <cstddef>
#include <cstdint>
#include <span>

namespace KaGuYa::Start::Runtime
{
class BmrDecoderRuntime;

// 6198CC指向的唯一30h正文；无vptr、分配器或重复的codec拥有状态。
struct ArchiveCodecManagerRuntime final
{
    ArchiveCodecManagerRuntime() noexcept;
    // 43DAC0的四个state均终止保护；失败不续清剩余槽。
    ~ArchiveCodecManagerRuntime() noexcept;
    ArchiveCodecManagerRuntime(const ArchiveCodecManagerRuntime&) = delete;
    ArchiveCodecManagerRuntime& operator=(const ArchiveCodecManagerRuntime&) = delete;

    YtlReferenceSlotRuntime huffmanAt00;
    YtlReferenceSlotRuntime bmrAt0C;
    YtlReferenceSlotRuntime lzssAt18;
    // 445220：真实BlockConvert10h，getter本身不初始化四个尺寸字段。
    YtlReferenceSlotRuntime blockConvertAt24;
};
static_assert(sizeof(ArchiveCodecManagerRuntime) == 0x30);
static_assert(offsetof(ArchiveCodecManagerRuntime, huffmanAt00) == 0x00);
static_assert(offsetof(ArchiveCodecManagerRuntime, bmrAt0C) == 0x0C);
static_assert(offsetof(ArchiveCodecManagerRuntime, lzssAt18) == 0x18);
static_assert(offsetof(ArchiveCodecManagerRuntime, blockConvertAt24) == 0x24);

// 现代来源接口在正文之外；返回借用，不额外增持codec或固定其身份。
class IArchiveCodecManagerSource
{
public:
    virtual ArchiveCodecManagerRuntime* CaptureAt445910() = 0;
protected:
    ~IArchiveCodecManagerSource() = default;
};

class ArchiveCodecManagerOwner final : public IArchiveCodecManagerSource
{
public:
    explicit ArchiveCodecManagerOwner(IEnvironmentRootLock& lock,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;
    ~ArchiveCodecManagerOwner() noexcept;
    ArchiveCodecManagerOwner(const ArchiveCodecManagerOwner&) = delete;
    ArchiveCodecManagerOwner& operator=(const ArchiveCodecManagerOwner&) = delete;

    [[nodiscard]] ArchiveCodecManagerRuntime* CaptureAt445910() override;
    // 即使发布槽为空也执行Enter/Leave；不得作为无需锁的幂等退出适配。
    void ReleaseAt5AA640() noexcept;
    [[nodiscard]] ArchiveCodecManagerRuntime* Published() const noexcept { return published_; }
    [[nodiscard]] ArchiveCodecManagerRuntime*& PublicationSlot() noexcept { return published_; }

private:
    void LeaveNoexcept() noexcept;
    IEnvironmentRootLock& lock_;
    IEnvironmentRootMemory& memory_;
    ArchiveCodecManagerRuntime* published_ = nullptr;
};

// 锁、退出注册与MTF表由CRT初始化，访问器不懒建；只有30h正文按原getter惰性申请。
[[nodiscard]] IArchiveCodecManagerSource& ProcessArchiveCodecManagerSource() noexcept;
[[nodiscard]] std::span<const std::uint8_t, 256> ProcessArchiveMtfDictionary() noexcept;

// 实现由BmrDecoderRuntime模块提供；这里只确定跨模块签名。
[[nodiscard]] BmrDecoderRuntime* GetBmrAt445110(ArchiveCodecManagerRuntime& manager);
// CLzss/Huffman的正文类型及inline getter由各自模块头声明。
}
