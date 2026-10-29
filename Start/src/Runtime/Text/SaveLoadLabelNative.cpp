#include <Runtime/SaveLoadLabelNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>

#include <bit>
#include <cstring>
#include <mbctype.h>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 18号Start_源文件.exe只读取证：5BDC34=0D 0A 00；5BDC38=A1 AD 00。
constexpr char lineExpansion[] = "\r\n";
constexpr char truncationMarker[] = "\xA1\xAD";
// 4DCAAD/4DCAB4的立即数按小端展开，不猜测可读placeholder。
constexpr unsigned char namePatterns[2][3] = {{0xF0, 0x40, 0}, {0xF0, 0x41, 0}};

ATL::CStringData* Header(const char* chars)
{ return reinterpret_cast<ATL::CStringData*>(reinterpret_cast<std::uintptr_t>(chars) - 16u); }

int Add(int left, int right)
{ return std::bit_cast<std::int32_t>(std::uint32_t(left) + std::uint32_t(right)); }
int Subtract(int left, int right)
{ return std::bit_cast<std::int32_t>(std::uint32_t(left) - std::uint32_t(right)); }

void Publish(RuntimeCString& value, ATL::CStringData* header)
{
    const auto chars = reinterpret_cast<std::uintptr_t>(header) + 16u;
    std::memcpy(&value, &chars, sizeof(chars));
}

struct ResultUnwind final
{
    RuntimeCString* value;
    // 5A0DF0/5A0AC0：失败时本producer仍拥有返回槽，caller尚无资格。
    ~ResultUnwind() noexcept { if (value) DestroyRuntimeCStringAt40CD40(value); }
};

class Text final
{
public:
    template<class Producer> explicit Text(Producer producer)
    { producer(&storage_.value); active_ = true; }
    // 只用于真实展开；正常路径显式释放，不能经过ATL析构throw()边界。
    ~Text() noexcept { if (active_) DestroyRuntimeCStringAt40CD40(&storage_.value); }
    Text(const Text&) = delete;
    Text& operator=(const Text&) = delete;
    RuntimeCString& Value() { return storage_.value; }
    void ReleaseCached(const char* chars)
    {
        active_ = false; // 原码先退EH state，Free抛出不重试此临时。
        ReleaseRuntimeCStringHeader(Header(chars));
    }
    void ReleaseCurrent() { ReleaseCached(storage_.value.GetString()); }
private:
    union Storage
    {
        Storage() {}
        ~Storage() {}
        RuntimeCString value;
    } storage_;
    bool active_ = false;
};

void ConstructClone(RuntimeCString* fresh, const RuntimeCString& value)
{ Publish(*fresh, CloneRuntimeCStringDataAt414540(Header(value.GetString()))); }

// 4FA490与4DAAD1内联单字节追加：显式长度保留NUL，不能用413550测长。
void AppendByte(RuntimeCString& value, char byte)
{
    char* chars = const_cast<char*>(value.GetString());
    auto* header = Header(chars);
    const int oldLength = header->nDataLength;
    const int length = Add(oldLength, 1);
    if (length < 0) throw ATL::CAtlException(E_INVALIDARG);
    const auto prepare = (1u - std::uint32_t(header->nRefs)) |
        (std::uint32_t(header->nAllocLength) - std::uint32_t(length));
    if (std::bit_cast<std::int32_t>(prepare) < 0)
    {
        PrepareRuntimeCStringWriteAt42D590(value, length);
        chars = const_cast<char*>(value.GetString());
    }
    chars[oldLength] = byte;
    header = Header(value.GetString());
    if (length > header->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
    header->nDataLength = length;
    const_cast<char*>(value.GetString())[length] = 0;
}

char ReadByte(const RuntimeCString& value, int index)
{
    if (index < 0 || index > value.GetLength()) throw ATL::CAtlException(E_INVALIDARG);
    return value.GetString()[index]; // index==length合法，尾lead读到终止NUL。
}

int FindLineByte(const RuntimeCString& value, unsigned byte)
{
    if (value.GetLength() <= 0) return -1;
    const auto* hit = _mbschr(reinterpret_cast<const unsigned char*>(value.GetString()), byte);
    return hit ? std::bit_cast<std::int32_t>(std::uint32_t(reinterpret_cast<std::uintptr_t>(hit) -
        reinterpret_cast<std::uintptr_t>(value.GetString()))) : -1;
}

void ConstructTail(RuntimeCString* fresh, const RuntimeCString& value, int start,
    ISaveLoadLabelNativeSource& source)
{
    // 原lea/sub以DWORD回绕，随后由共享Mid执行非负裁剪和加法溢出检查。
    ConstructRuntimeCStringMidAt420C80(fresh, value, start,
        Subtract(value.GetLength(), start), source.Strings());
}

const RuntimeCString& CaptureName(ISaveLoadLabelNativeSource& source, unsigned offset)
{
    auto* registry = ResolveEnvironmentArchiveAt417990(*source.CaptureEnvironmentAt4180C0(), 1);
    return *reinterpret_cast<const RuntimeCString*>(reinterpret_cast<std::uintptr_t>(registry) + offset);
}

void DestroyNamePrefix(RuntimeCString* values, unsigned count) noexcept
{
    // 53FB52：再次抛出时在该元素终止；不能继续尝试更早的元素。
    while (count) DestroyRuntimeCStringAt40CD40(values + --count);
}

struct NamePrefixUnwind final
{
    RuntimeCString* values;
    const unsigned& count;
    bool armed = true;
    ~NamePrefixUnwind() noexcept { if (armed) DestroyNamePrefix(values, count); }
};

void DestroyNames(RuntimeCString* values, unsigned count)
{
    NamePrefixUnwind prefix{values, count};
    // 53FAB4自身可抛：失败元素已从count移除，展开先清剩余前缀，再外传异常。
    // 即使本次是在原state7的unwind内调用，也不能提前noexcept而跳过此前缀。
    while (count) DestroyRuntimeCStringAt40CD40(values + --count);
    prefix.armed = false;
}

class Names final
{
public:
    explicit Names(ISaveLoadLabelNativeSource& source)
    {
        NamePrefixUnwind prefix{storage_.values, completed_};
        ConstructClone(storage_.values, CaptureName(source, 0x58));
        completed_ = 1;
        ConstructClone(storage_.values + 1, CaptureName(source, 0x5C));
        completed_ = 2;
        prefix.armed = false;
    }
    ~Names() noexcept { Release(); }
    Names(const Names&) = delete;
    Names& operator=(const Names&) = delete;
    RuntimeCString& operator[](unsigned index) { return storage_.values[index]; }
    void Release()
    {
        const unsigned count = completed_;
        completed_ = 0; // 4DCD57：外层state7先退state0，内部iterator负责自己的前缀。
        if (count) DestroyNames(storage_.values, count);
    }
private:
    union Storage
    {
        Storage() {}
        ~Storage() {}
        RuntimeCString values[2];
    } storage_;
    unsigned completed_ = 0;
};
}

RuntimeCString* ExpandSaveLoadLabelNativeAt4DC660(RuntimeCString* fresh,
    const char* input, ISaveLoadLabelNativeSource& source)
{
    InitializeEmptyRuntimeCString(*fresh, source.Strings());
    ResultUnwind result{fresh};
    SetRuntimeCStringAt437680(*fresh, "", 0);
    {
        Text remainder([&](RuntimeCString* out) { source.ConstructInputAt40B530(out, input); });
        while (remainder.Value().GetLength() != 0)
        {
            const int cr = FindLineByte(remainder.Value(), 13);
            const int lf = FindLineByte(remainder.Value(), 10);
            // 4DC729：相等走整串追加；不同按缺失(-1)/先后选CR或LF。
            const int kind = cr < lf ? (cr < 0 ? 2 : 1) : cr > lf ? (lf < 0 ? 1 : 2) : 0;
            if (kind == 0)
            {
                AppendRuntimeCStringAt413550(*fresh, remainder.Value().GetString(), remainder.Value().GetLength());
                SetRuntimeCStringAt437680(remainder.Value(), "", 0);
                continue;
            }
            const int index = kind == 1 ? cr : lf;
            {
                Text left([&](RuntimeCString* out) {
                    ConstructRuntimeCStringLeftAt419D00(out, remainder.Value(), index, source.Strings());
                });
                const char* cached = left.Value().GetString();
                AppendRuntimeCStringAt413550(*fresh, cached, Header(cached)->nDataLength);
                left.ReleaseCached(cached);
            }
            {
                Text tail([&](RuntimeCString* out) { ConstructTail(out, remainder.Value(), Add(index, 1), source); });
                const char* cached = tail.Value().GetString();
                AssignRuntimeCStringAt410220(remainder.Value(), tail.Value());
                tail.ReleaseCached(cached);
            }
            if (kind == 2 && remainder.Value().GetLength() != 0)
                AppendRuntimeCStringAt413550(*fresh, lineExpansion, 2);
        }
        remainder.ReleaseCurrent();
    }
    // 4DCABA/4DCB1E：两次Capture及slot1解引用，第一姓名Clone之后才捕获第二根。
    Names names(source);
    for (unsigned group = 0; group < 2;)
    {
        const auto* hit = fresh->GetLength() < 0 ? nullptr :
            _mbsstr(reinterpret_cast<const unsigned char*>(fresh->GetString()), namePatterns[group]);
        const int index = hit ? std::bit_cast<std::int32_t>(std::uint32_t(reinterpret_cast<std::uintptr_t>(hit) -
            reinterpret_cast<std::uintptr_t>(fresh->GetString()))) : -1;
        if (index < 0) { ++group; continue; }
        Text prefix([&](RuntimeCString* out) {
            ConstructRuntimeCStringLeftAt419D00(out, *fresh, index, source.Strings());
        });
        AppendRuntimeCStringAt413550(prefix.Value(), names[group].GetString(), names[group].GetLength());
        {
            Text tail([&](RuntimeCString* out) { ConstructTail(out, *fresh, Add(index, 2), source); });
            const char* cached = tail.Value().GetString();
            AppendRuntimeCStringAt413550(prefix.Value(), cached, Header(cached)->nDataLength);
            tail.ReleaseCached(cached);
        }
        const char* cached = prefix.Value().GetString(); // 4DCCD0：Assign前缓存，展开才重读。
        AssignRuntimeCStringAt410220(*fresh, prefix.Value());
        prefix.ReleaseCached(cached);
        // 不推进group：原4DCD42回到当前pattern搜索；姓名自含本组pattern会无限替换。
    }
    // 53FAB4逆序销毁数组；后一个失败时展开仍销毁前一个，返回槽仍由本入口清理。
    names.Release();
    result.value = nullptr;
    return fresh;
}

RuntimeCString* TruncateSaveLoadLabelNativeAt4DA8E0(RuntimeCString* fresh,
    const char* input, std::int32_t maxBytesPerLine, std::int32_t maxLines,
    ISaveLoadLabelNativeSource& source)
{
    InitializeEmptyRuntimeCString(*fresh, source.Strings());
    ResultUnwind result{fresh};
    SetRuntimeCStringAt437680(*fresh, "", 0);
    Text text([&](RuntimeCString* out) { source.ConstructInputAt40B530(out, input); });
    int position = 0, width = 0, lines = 0, previousBytes = 0;
    bool marked = false;
    while (position < text.Value().GetLength() && lines < maxLines)
    {
        const char byte = ReadByte(text.Value(), position);
        int step = 1;
        if (byte == '\r' || byte == '\n')
        {
            AppendByte(*fresh, byte);
            // 4DAB1C：分配回调之后重读输入；不是根据追加前缓存byte判断LF。
            if (ReadByte(text.Value(), position) == '\n')
            { width = 0; marked = false; lines = Add(lines, 1); }
        }
        else if (width >= maxBytesPerLine)
        {
            if (!marked)
            {
                Text left([&](RuntimeCString* out) {
                    ConstructRuntimeCStringLeftAt419D00(out, *fresh,
                        Subtract(fresh->GetLength(), previousBytes), source.Strings());
                });
                AssignRuntimeCStringAt410220(*fresh, left.Value());
                left.ReleaseCurrent(); // 4DA9EE：Assign之后才读临时槽。
                AppendRuntimeCStringAt413550(*fresh, truncationMarker, 2);
                marked = true;
            }
            // 截断后逐字节跳过，不能按lead跨过后续CR/LF。
        }
        else
        {
            // 4DAA2D是movsx；lead由当前CRT多字节表决定，不固化CP932。
            const bool lead = _ismbblead(static_cast<int>(static_cast<signed char>(byte))) != 0;
            AppendByte(*fresh, ReadByte(text.Value(), position));
            if (lead) { AppendByte(*fresh, ReadByte(text.Value(), Add(position, 1))); step = 2; }
            width = Add(width, step);
            previousBytes = step;
        }
        position = Add(position, step);
    }
    text.ReleaseCurrent();
    result.value = nullptr;
    return fresh;
}
}
