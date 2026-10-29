#pragma once

#include <Runtime/ParamsRuntimeProfile.h>
#include <Runtime/ParamsInputBytesBorrow.h>

namespace KaGuYa::Start::Runtime
{
// 41ADE0/41DFD0/41F8A0 直接访问 buffer，不使用 CSerialize 的标量游标。
// Bounded输入检查实际读取范围；native只在读取点重取begin，不预检end/capacity。
class ParamsRuntimeReadCursor final
{
public:
    explicit ParamsRuntimeReadCursor(std::span<const std::byte> bytes, std::size_t offset = 0);
    template<std::size_t Extent>
    explicit ParamsRuntimeReadCursor(std::span<std::byte, Extent> bytes, std::size_t offset = 0)
        : ParamsRuntimeReadCursor(std::span<const std::byte>(bytes), offset)
        { writableSpan_ = true; }
    // 实际SerializeBuffer尚为现代vector；借用owner而非缓存data，分配回调后仍晚取输入。
    explicit ParamsRuntimeReadCursor(const std::vector<std::byte>& bytes, std::size_t offset = 0);
    explicit ParamsRuntimeReadCursor(std::vector<std::byte>& bytes, std::size_t offset = 0);
    explicit ParamsRuntimeReadCursor(ParamsInputBytesBorrow bytes, std::size_t offset = 0);
    // 419EE0/41A2C2显式缓存源，跨后续manager回调不再重取begin。
    struct CapturedSource final
    {
        const std::byte* data;
        std::optional<std::size_t> availableBytes;
    };
    [[nodiscard]] CapturedSource CaptureSourceAt(std::size_t offset) const;
    [[nodiscard]] std::size_t Offset() const noexcept { return offset_; }
    [[nodiscard]] std::size_t Remaining() const;
    void Seek(std::size_t offset);
    [[nodiscard]] std::span<const std::byte> Slice(std::size_t offset, std::size_t length) const;
    [[nodiscard]] std::uint8_t U8();
    [[nodiscard]] std::uint16_t U16();
    [[nodiscard]] std::uint32_t U32();
    [[nodiscard]] std::vector<std::byte> Bytes(std::size_t length);
    // 旧reader逐字节重取输入载体，原位XOR不提交游标；只读现代视图明确拒绝。
    void XorByteAt(std::size_t offset, std::uint8_t key);
    // 4207F0：U8长度，非空才取manager；XOR先于构造，正常释放后才提交消费量。
    void Text8At4207F0(RuntimeCString& destination, const std::uint8_t& key, bool decrypt,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    void Text8At4207F0(RuntimeCString* destination, const std::uint8_t& key, bool decrypt,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    void Text16(ParamsText& destination);
    // 41B33E/41B48B 的安装项直接写入 CString，不经 ParamsText 值副本。
    void Text16(RuntimeCString& destination);
    // 4209F1 的零长度分支允许空目标；非空文本到实际写入边界才拒绝空指针。
    void Text16(RuntimeCString* destination);
    // 41D294：嵌套列表内联读取；每次解址重读byte Ref，正常/异常均经40CF00释放。
    void NestedText16At41D294(RuntimeCString& destination);
    // 420045/42017F：SettingTag内联S缓存control/bias、逐操作重读base，尾声固定control。
    void SettingText16At420045(RuntimeCString& destination);
    // 419EB0 成功返回消费量后仅加游标；尾声释放可改输入，不额外重验缓冲。
    void CommitOffsetAfter419EB0(std::uint32_t absoluteOffset) noexcept { offset_ = absoluteOffset; }
    [[nodiscard]] ParamsText Text16();
    void TextBytes(ParamsText& destination, std::size_t length);

private:
    std::span<const std::byte> bytes_;
    std::optional<ParamsInputBytesBorrow> byteOwner_;
    std::size_t offset_ = 0;
    bool writableSpan_ = false;
};
}
