#pragma once

#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
// 45A5D0、43D3D0、494280/466760：原始manager槽Clone/GetNil可抛，成功后按4145E0拼接。
// GetNil返回前不写输出、不清理；返回后失败按40CD40展开清理当前输出槽。
// std::string/char*仅为显式现代输入；生产CString保留原manager和实时字段读取。
class RuntimeCStringConcatenation final
{
public:
    RuntimeCStringConcatenation(RuntimeStringBorrow prefix, const char* suffix,
        std::uint32_t suffixLength,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    // 466760：GetNil成功后才测suffix的C字符串长度，null表示长度0。
    RuntimeCStringConcatenation(RuntimeStringBorrow prefix, const char* suffix,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    ~RuntimeCStringConcatenation() noexcept(false);
    RuntimeCStringConcatenation(const RuntimeCStringConcatenation&) = delete;
    RuntimeCStringConcatenation& operator=(const RuntimeCStringConcatenation&) = delete;

    [[nodiscard]] const RuntimeCString& Value() const noexcept { return storage_.value; }
    [[nodiscard]] const char* GetString() const noexcept { return storage_.value.GetString(); }
private:
    void Initialize(RuntimeStringBorrow prefix, const char* suffix,
        const std::uint32_t* explicitSuffixLength, IRuntimeStringManagerSource& strings);
    union Storage
    {
        Storage() noexcept {}
        ~Storage() noexcept {}
        RuntimeCString value;
    } storage_;
};
static_assert(sizeof(RuntimeCStringConcatenation) == 4);
}
