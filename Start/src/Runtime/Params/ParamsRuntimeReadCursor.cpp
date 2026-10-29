#include <Runtime/ParamsRuntimeReadCursor.h>
#include <Runtime/ByteArrayReferenceRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>

#include <Windows.h>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <limits>

namespace KaGuYa::Start::Runtime
{
namespace
{
void ReleaseConvertedTemporary(ByteArrayReferenceRuntime& temporary,
    YtlReferenceControlRuntime* capturedControl) noexcept
{
    // 420AAA的state1 -> 541E55；正常尾声固定ESI，不重读局部Ref的control。
    if (capturedControl && --capturedControl->referenceCountAt04 == 0)
    {
        if (capturedControl->owningAt08) capturedControl->DestroyPayloadAtVslot00();
        capturedControl->DeleteControlAtVslot04(1);
    }
    // 原局部槽此后不再清理；异常展开仍由ByteArray的40CF00析构负责。
    temporary.RawSlot().controlAt08 = nullptr;
}
}

ParamsRuntimeReadCursor::ParamsRuntimeReadCursor(
    const std::span<const std::byte> bytes, const std::size_t offset) : bytes_(bytes)
{
    Seek(offset);
}

ParamsRuntimeReadCursor::ParamsRuntimeReadCursor(
    const std::vector<std::byte>& bytes, const std::size_t offset)
    : ParamsRuntimeReadCursor(ParamsInputBytesBorrow::Bounded(bytes), offset)
{
}

ParamsRuntimeReadCursor::ParamsRuntimeReadCursor(
    std::vector<std::byte>& bytes, const std::size_t offset)
    : ParamsRuntimeReadCursor(ParamsInputBytesBorrow::Bounded(bytes), offset)
{
}

ParamsRuntimeReadCursor::ParamsRuntimeReadCursor(
    const ParamsInputBytesBorrow bytes, const std::size_t offset) : byteOwner_(bytes)
{
    Seek(offset);
}

ParamsRuntimeReadCursor::CapturedSource ParamsRuntimeReadCursor::CaptureSourceAt(const std::size_t offset) const
{
    const auto size = byteOwner_ ? byteOwner_->BoundedSize() : std::optional<std::size_t>(bytes_.size());
    if (size && offset > *size)
        throw ParamsRuntimeException(ParamsRuntimeError::TruncatedInput, offset,
            "params runtime read exceeds the supplied buffer (safe boundary)");
    const auto* begin = byteOwner_ ? byteOwner_->Begin() : bytes_.data();
    const auto* data = reinterpret_cast<const std::byte*>(reinterpret_cast<std::uintptr_t>(begin) + offset);
    return {data, size ? std::optional<std::size_t>(*size - offset) : std::nullopt};
}

std::size_t ParamsRuntimeReadCursor::Remaining() const
{
    if (byteOwner_ && !byteOwner_->IsBounded())
        throw std::logic_error("native Params input has no bounded remaining extent");
    const auto source = CaptureSourceAt(offset_);
    return *source.availableBytes;
}

void ParamsRuntimeReadCursor::Seek(const std::size_t offset)
{
    if (!byteOwner_ || byteOwner_->IsBounded()) static_cast<void>(Slice(offset, 0));
    offset_ = offset;
}

std::span<const std::byte> ParamsRuntimeReadCursor::Slice(
    const std::size_t offset, const std::size_t length) const
{
    const auto source = CaptureSourceAt(offset);
    if (source.availableBytes && length > *source.availableBytes)
        throw ParamsRuntimeException(ParamsRuntimeError::TruncatedInput, offset,
            "params runtime read exceeds the supplied buffer (safe boundary)");
    return {source.data, length};
}

std::uint8_t ParamsRuntimeReadCursor::U8()
{
    const auto data = Slice(offset_, 1);
    ++offset_;
    return std::to_integer<std::uint8_t>(data[0]);
}

std::uint16_t ParamsRuntimeReadCursor::U16()
{
    const auto data = Slice(offset_, 2);
    const auto value = static_cast<std::uint16_t>(std::to_integer<std::uint8_t>(data[0]) |
        (std::to_integer<std::uint8_t>(data[1]) << 8));
    offset_ += 2;
    return value;
}

std::uint32_t ParamsRuntimeReadCursor::U32()
{
    const auto data = Slice(offset_, 4);
    std::uint32_t value = 0;
    for (unsigned i = 0; i != 4; ++i)
        value |= static_cast<std::uint32_t>(std::to_integer<std::uint8_t>(data[i])) << (i * 8);
    offset_ += 4;
    return value;
}

std::vector<std::byte> ParamsRuntimeReadCursor::Bytes(const std::size_t length)
{
    const auto data = Slice(offset_, length);
    std::vector<std::byte> result(data.begin(), data.end());
    offset_ += length;
    return result;
}

void ParamsRuntimeReadCursor::XorByteAt(const std::size_t offset, const std::uint8_t key)
{
    if (!(byteOwner_ ? byteOwner_->IsWritable() : writableSpan_))
        throw std::logic_error("legacy params XOR requires writable input (safe boundary)");
    const auto byte = Slice(offset, 1);
    // 写权限来自显式mutable借用或原Serialize载体，不对const输入做复制兜底。
    *const_cast<std::byte*>(byte.data()) ^= static_cast<std::byte>(key);
}

void ParamsRuntimeReadCursor::Text8At4207F0(RuntimeCString& destination,
    const std::uint8_t& key, const bool decrypt, IRuntimeStringManagerSource& strings)
{
    Text8At4207F0(&destination, key, decrypt, strings);
}

void ParamsRuntimeReadCursor::Text8At4207F0(RuntimeCString* destination,
    const std::uint8_t& key, const bool decrypt, IRuntimeStringManagerSource& strings)
{
    const auto start = offset_;
    const auto length = std::to_integer<std::uint8_t>(Slice(start, 1)[0]);
    if (!length)
    {
        offset_ = start + 1;
        return;
    }
    if (decrypt)
        for (std::uint32_t index = 0; index < length; ++index)
            XorByteAt(start + 1u + index, key);
    // 420854..420864先缓存源，再取manager；分配回调后不得重取或提前复制源。
    const auto source = Slice(start + 1u, length);
    auto* const manager = strings.CaptureAt417360();
    struct Temporary final
    {
        union { RuntimeCString value; };
        bool active = false;
        Temporary() noexcept {}
        ~Temporary() noexcept { if (active) DestroyRuntimeCStringAt40CD40(&value); }
    } temporary;
    ConstructRuntimeCStringAt40B490(&temporary.value,
        reinterpret_cast<const char*>(source.data()), length, manager);
    temporary.active = true;
    if (!destination) _invalid_parameter_noinfo_noreturn();
    AssignRuntimeCStringAt410220(*destination, temporary.value);
    auto* const header = reinterpret_cast<ATL::CStringData*>(
        const_cast<char*>(temporary.value.GetString())) - 1;
    // 42087C..42089B先撤state0；正常Free异常不再次释放本临时串。
    temporary.active = false;
    ReleaseRuntimeCStringHeader(header);
    offset_ = start + length + 1u;
}

void ParamsRuntimeReadCursor::TextBytes(ParamsText& destination, const std::size_t length)
{
    // 4209B0/420B00：零字节不赋值；非零复制到补零宽字符串，嵌入 NUL 截断转换。
    if (length == 0) return;
    const auto data = Slice(offset_, length);
    if (length > static_cast<std::size_t>(std::numeric_limits<int>::max()) - 3u)
        throw std::length_error("params runtime text exceeds Win32 conversion range");
    std::wstring wide((length + 1u) / 2u + 1u, L'\0');
    std::memcpy(wide.data(), data.data(), length);
    wide.resize(std::char_traits<wchar_t>::length(wide.c_str()));

    // 4102D0 原调用的输出容量不包含 NUL，第二次返回值不参与分支。
    // 与 Message2Runtime 保持一致：失败后未写字节置零是安全差异，不复制未初始化内存。
    const int size = WideCharToMultiByte(CP_THREAD_ACP, 0, wide.c_str(), -1,
        nullptr, 0, nullptr, nullptr) - 1;
    std::string narrow;
    if (size > 0)
    {
        narrow.resize(static_cast<std::size_t>(size), '\0');
        static_cast<void>(WideCharToMultiByte(CP_THREAD_ACP, 0, wide.c_str(), -1,
            narrow.data(), size, nullptr, nullptr));
    }
    destination.utf16.assign(reinterpret_cast<const char16_t*>(wide.data()), wide.size());
    // 字段名为历史 cp932；运行时实际采用线程 ACP，LE 日区启动时才是 CP932。
    destination.cp932 = std::move(narrow);
    offset_ += length;
}

void ParamsRuntimeReadCursor::Text16(ParamsText& destination) { TextBytes(destination, U16()); }
void ParamsRuntimeReadCursor::Text16(RuntimeCString& destination)
{
    Text16(&destination);
}

void ParamsRuntimeReadCursor::Text16(RuntimeCString* destination)
{
    const auto start = offset_;
    const auto prefix = Slice(start, 2);
    const auto length = static_cast<std::uint32_t>(std::to_integer<std::uint8_t>(prefix[0])) |
        (static_cast<std::uint32_t>(std::to_integer<std::uint8_t>(prefix[1])) << 8u);
    if (!length)
    {
        offset_ = start + 2;
        return; // 420AE6只返回2，不清目标、不取manager、不分配临时数组。
    }

    ByteArrayReferenceRuntime temporary;
    temporary.AllocateAt412700(length + 2u);
    auto* const control = static_cast<EnvironmentObjectReferenceControl*>(temporary.RawSlot().controlAt08);
    const auto base = temporary.RawSlot().baseAt00;
    // 420A1F..420A49缓存control/base，count来自control，不从请求长度反推。
    const auto temporaryAddress = [&]() noexcept
    {
        const auto stride = control ? control->strideAt0C : 0u;
        return base ? reinterpret_cast<std::uint8_t*>(base + temporary.RawSlot().biasAt04 * stride) : nullptr;
    };
    std::memset(temporaryAddress(), 0, control ? control->countAt10 : 0u);
    // 420A7A在412700回调之后晚取输入；不缓存分配前的data，也不提前拷贝正文。
    const auto input = Slice(start + 2u, length);
    std::memmove(temporaryAddress(), input.data(), length);
    const auto* const wide = reinterpret_cast<const wchar_t*>(temporaryAddress());
    // 明确安全边界：原len+2在奇数len时未必容纳完整WCHAR零；不偷加一字节冒充原分配。
    if (!std::wmemchr(wide, L'\0', (length + 2u) / sizeof(wchar_t)))
        throw ParamsRuntimeException(ParamsRuntimeError::InvalidUtf16, start + 2u,
            "params runtime WCHAR text has no complete NUL within its byte allocation (safe boundary)");
    // 4209B0自身不访问目标；转换器先执行原Win32查询，再处理空目标失败边界。
    AssignRuntimeCStringWideAt4102D0(destination, wide);
    ReleaseConvertedTemporary(temporary, control);
    // 原函数成功返回length+2后调用者才加游标；转换/释放失败不能提交消费量。
    offset_ = start + length + 2u;
}
void ParamsRuntimeReadCursor::NestedText16At41D294(RuntimeCString& destination)
{
    const auto start = offset_;
    const auto prefix = Slice(start, 2);
    const auto length = static_cast<std::uint32_t>(std::to_integer<std::uint8_t>(prefix[0])) |
        (static_cast<std::uint32_t>(std::to_integer<std::uint8_t>(prefix[1])) << 8u);
    if (length)
    {
        ByteArrayReferenceRuntime temporary;
        temporary.AllocateAt412700(length + 2u);
        // 41D2DF..41D2F7先取当前count，再经43D230解址；不能缓存4209B0的control/base。
        const auto count = temporary.Size();
        std::memset(temporary.DataAt(0), 0, count);
        const auto input = Slice(start + 2u, length);
        std::memmove(temporary.DataAt(0), input.data(), length);
        const auto* const wide = reinterpret_cast<const wchar_t*>(temporary.DataAt(0));
        // 与普通S相同的明确安全边界：不读取len+2分配之外的WCHAR，也不扩大原分配。
        if (!std::wmemchr(wide, L'\0', (length + 2u) / sizeof(wchar_t)))
            throw ParamsRuntimeException(ParamsRuntimeError::InvalidUtf16, start + 2u,
                "params nested WCHAR text has no complete NUL within its byte allocation (safe boundary)");
        AssignRuntimeCStringWideAt4102D0(destination, wide);
        // 41D33B先撤state1B，再调用40CF00。析构自身noexcept且重读当前Ref，失败不可重试。
    }
    // 41D347：零长度保留destination；转换和临时Ref释放成功以后才提交消耗量。
    offset_ = start + length + 2u;
}

void ParamsRuntimeReadCursor::SettingText16At420045(RuntimeCString& destination)
{
    const auto start = offset_;
    const auto prefix = Slice(start, 2);
    const auto length = static_cast<std::uint32_t>(std::to_integer<std::uint8_t>(prefix[0])) |
        (static_cast<std::uint32_t>(std::to_integer<std::uint8_t>(prefix[1])) << 8u);
    if (length)
    {
        ByteArrayReferenceRuntime temporary;
        temporary.AllocateAt412700(length + 2u);
        auto* const control = static_cast<EnvironmentObjectReferenceControl*>(temporary.RawSlot().controlAt08);
        const auto bias = temporary.RawSlot().biasAt04;
        const auto count = control ? control->countAt10 : 0u;
        const auto address = [&]() noexcept
        {
            const auto base = temporary.RawSlot().baseAt00;
            return base ? reinterpret_cast<std::uint8_t*>(
                base + bias * (control ? control->strideAt0C : 0u)) : nullptr;
        };
        std::memset(address(), 0, count);
        auto* const destinationBytes = address();
        const auto input = Slice(start + 2u, length);
        std::memmove(destinationBytes, input.data(), length);
        const auto* const wide = reinterpret_cast<const wchar_t*>(address());
        if (!std::wmemchr(wide, L'\0', (length + 2u) / sizeof(wchar_t)))
            throw ParamsRuntimeException(ParamsRuntimeError::InvalidUtf16, start + 2u,
                "params SettingTag WCHAR text has no complete NUL within its byte allocation (safe boundary)");
        AssignRuntimeCStringWideAt4102D0(destination, wide);
        // 42012F/420255先进入terminate状态；正常固定control，异常展开才按当前Ref释放。
        ReleaseConvertedTemporary(temporary, control);
        if (control) temporary.RawSlot().baseAt00 = 0;
    }
    offset_ = start + length + 2u;
}

ParamsText ParamsRuntimeReadCursor::Text16() { ParamsText result; Text16(result); return result; }
}
