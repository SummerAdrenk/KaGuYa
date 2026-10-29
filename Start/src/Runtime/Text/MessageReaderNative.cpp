#include <Runtime/MessageReaderNative.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/RuntimeCString.h>

#include <bit>
#include <cstdint>
#include <emmintrin.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class Reader>
Reader* AllocateReader(IRuntimeStringManagerSource& strings)
{
    void* storage = ::operator new(sizeof(Reader));
    return storage ? ::new (storage) Reader(strings) : nullptr;
}
}

NativeMessage2Runtime* AllocateNativeMessage2Runtime(IRuntimeStringManagerSource& strings)
{ return AllocateReader<NativeMessage2Runtime>(strings); }
NativeLegacyMessageRuntime* AllocateNativeLegacyMessageRuntime(IRuntimeStringManagerSource& strings)
{ return AllocateReader<NativeLegacyMessageRuntime>(strings); }
NativeNullMessageRuntime* AllocateNativeNullMessageRuntime(IRuntimeStringManagerSource& strings)
{ return AllocateReader<NativeNullMessageRuntime>(strings); }

static_assert(sizeof(std::uintptr_t) == 4);
static_assert(sizeof(NativeMessageReader::PathStorage) == 4);
static_assert(alignof(NativeMessageReader) == 4);

NativeMessageReader::NativeMessageReader(IRuntimeStringManagerSource& strings)
{
    // 45A71A/45A8D1/45EDB5：进入函数体时基类表已写，+04..+0B 不初始化。
    auto* manager = strings.CaptureAt417360();
    if (!manager) throw ATL::CAtlException(E_FAIL);
    // 先增持共享 nil，再发布 +0C；不经临时 CString，不 Clone 或 Allocate(0)。
    // v143 GetNilString 的 throw() 限定仍在，不冒充任意原始 ABI 目标的异常传播。
    ::new (static_cast<void*>(&pathAt0C.value)) RuntimeCString(manager);
}

void NativeMessageReader::DestroyPathAt40CD40()
{
    // 492F08..492F20：可抛的原始 Free 路径；不清槽、不重试、不再执行 ATL 析构。
    DestroyRuntimeCStringAt40CD40(&pathAt0C.value);
}

void NativeMessageReader::DecodeBufferAt46DFD0(std::uint8_t* buffer, std::int32_t length)
{
    if (encryptedAt08 == 0 || length <= 0) return;

    std::int32_t index = 0;
    if (length >= 0x40)
    {
        // 46DFFA..46E01E：先捕获 SIMD key，按 DWORD 地址回绕检查整个区间是否覆盖 key。
        const auto key = _mm_set1_epi8(std::bit_cast<char>(keyAt09));
        const auto begin = reinterpret_cast<std::uintptr_t>(buffer);
        const auto last = begin + static_cast<std::uint32_t>(length) - 1u;
        const auto keyAddress = reinterpret_cast<std::uintptr_t>(&keyAt09);
        if (begin > keyAddress || last < keyAddress)
        {
            const auto vectorEnd = length - (length & 0x3F);
            for (; index < vectorEnd; index += 0x40)
            {
                // 46E033..46E067：四次独立的非对齐 16-byte 读/XOR/写，按地址正序。
                auto* const block = buffer + index;
                auto value = _mm_loadu_si128(reinterpret_cast<const __m128i*>(block));
                _mm_storeu_si128(reinterpret_cast<__m128i*>(block), _mm_xor_si128(value, key));
                value = _mm_loadu_si128(reinterpret_cast<const __m128i*>(block + 0x10));
                _mm_storeu_si128(reinterpret_cast<__m128i*>(block + 0x10), _mm_xor_si128(value, key));
                value = _mm_loadu_si128(reinterpret_cast<const __m128i*>(block + 0x20));
                _mm_storeu_si128(reinterpret_cast<__m128i*>(block + 0x20), _mm_xor_si128(value, key));
                value = _mm_loadu_si128(reinterpret_cast<const __m128i*>(block + 0x30));
                _mm_storeu_si128(reinterpret_cast<__m128i*>(block + 0x30), _mm_xor_si128(value, key));
            }
        }
    }

    // 46E078..46E081：每字节重读 key；覆盖 key 后它变为 0，不再重读 encrypted。
#pragma loop(no_vector)
    for (; index < length; ++index)
        buffer[index] ^= keyAt09;
}

void NativeMessageReader::DecodeReferenceAt46DF80(const ByteArrayReferenceRuntime& bytes)
{
    // 46DF87..46DF98：只借用 Ref，先捕获 control/count，再读 base，不增持或改槽。
    const auto& slot = bytes.RawSlot();
    const auto* const control = static_cast<const EnvironmentObjectReferenceControl*>(slot.controlAt08);
    const auto count = control ? control->countAt10 : 0u;
    const auto base = slot.baseAt00;
    std::uint8_t* buffer = nullptr;
    if (base)
    {
        const auto stride = control ? control->strideAt0C : 0u;
        const auto bias = slot.biasAt04;
        buffer = reinterpret_cast<std::uint8_t*>(base + bias * stride);
    }
    // 46DFA6/46DFC7：空 base 也虚调槽 0；count 不扣 bias、不乘 stride，保留符号位。
    this->DecodeBufferAt46DFD0(buffer, std::bit_cast<std::int32_t>(count));
}

float NativeMessageReader::ReadVersionAt47DDE0() const
{
    return versionAt04; // 47DDE0：直接读取 +04，不为未初始化版本补默认值。
}

NativeNullMessageRuntime::NativeNullMessageRuntime(IRuntimeStringManagerSource& strings) noexcept
    : NativeMessageReader(strings)
{
    // 45ED90/5E5E3C：构造 EHFlags=5，基类中的 Capture/路径构造也在此 noexcept 边界内。
}

NativeNullMessageRuntime::~NativeNullMessageRuntime() noexcept(false)
{
    // 492F00 内联释放可抛；基类和 PathStorage 的空析构不重复释放路径。
    // controlDestroy 必须在此正常返回后才单独释放 10h，不能以 delete 表达式代替。
    DestroyPathAt40CD40();
}

float NativeNullMessageRuntime::ReadVersionAt47DDE0() const
{
    return 0.0f; // 47DDF0：fldz，不读取未初始化的 +04。
}

bool NativeNullMessageRuntime::Load(const char*)
{
    return false; // 484E30：不读取路径，不打开流，也不修改任何正文字段。
}

std::int32_t NativeNullMessageRuntime::GetFormatCount() const noexcept
{
    return 0; // 47ADB0。
}

Message2FormatRef NativeNullMessageRuntime::GetFormat(std::int32_t)
{
    return nullptr; // 47AB66..47AB73：仅返回三字段全零的 Ref。
}

std::int32_t NativeNullMessageRuntime::GetChoiceCount() const noexcept
{
    return 0; // 47D150。
}

MessageTextCommandReference NativeNullMessageRuntime::GetChoice(std::int32_t)
{
    return nullptr; // 47D0F6..47D103：忽略索引，不分配字符串正文。
}

std::int32_t NativeNullMessageRuntime::GetMessageCount() const noexcept
{
    return 0; // 47B750。
}

MessageVoiceAndTextRef NativeNullMessageRuntime::GetMessage(std::int32_t)
{
    return nullptr; // 47B356..47B363：不分配正文、语音或文本对象。
}
}
