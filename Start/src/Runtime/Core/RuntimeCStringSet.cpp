#include <Runtime/RuntimeCString.h>
#include <Runtime/ProcessAtlBaseModuleNative.h>

#include <Windows.h>

#include <algorithm>
#include <bit>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <limits>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
ATL::CStringData* Header(const char* data) noexcept
{
    return reinterpret_cast<ATL::CStringData*>(
        reinterpret_cast<std::uintptr_t>(data) - sizeof(ATL::CStringData));
}

char* Data(ATL::CStringData* header) noexcept
{
    return reinterpret_cast<char*>(reinterpret_cast<std::uintptr_t>(header) + sizeof(ATL::CStringData));
}

void Publish(RuntimeCString& destination, char* data) noexcept
{
    static_assert(sizeof(destination) == sizeof(data));
    std::memcpy(&destination, &data, sizeof(data));
}

template<class Function>
Function ManagerSlot(ATL::IAtlStringMgr* manager, std::size_t offset) noexcept
{
    // 不能经过 v143 IAtlStringMgr 的 throw() 声明；原始目标可将异常传给调用者。
    const std::byte* table;
    std::memcpy(&table, manager, sizeof(table));
    Function function;
    static_assert(sizeof(function) == sizeof(void*));
    std::memcpy(&function, table + offset, sizeof(function));
    return function;
}

ATL::IAtlStringMgr* CloneSubstringManager(ATL::CStringData* source,
    IRuntimeStringManagerSource& strings)
{
    using Clone = ATL::IAtlStringMgr* (__thiscall*)(ATL::IAtlStringMgr*);
    auto* manager = source->pStringMgr;
    if (manager)
    {
        auto* const cloned = ManagerSlot<Clone>(manager, 16)(manager);
        if (cloned) return cloned;
    }
    // 419D41/420CFB：仅原manager缺失或Clone返回空才重取全局manager并再次Clone。
    manager = strings.CaptureAt417360();
    return ManagerSlot<Clone>(manager, 16)(manager);
}

void ReleaseHeader(ATL::CStringData* header)
{
    if (_InterlockedDecrement(&header->nRefs) > 0) return;
    using Free = void (__thiscall*)(ATL::IAtlStringMgr*, ATL::CStringData*);
    auto* manager = header->pStringMgr;
    ManagerSlot<Free>(manager, 4)(manager, header);
}

void EmptyAt416340(RuntimeCString& destination)
{
    auto* header = Header(destination.GetString());
    auto* const manager = header->pStringMgr;
    if (!header->nDataLength) return;
    if (header->nRefs < 0)
    {
        if (header->nAllocLength < 0) throw ATL::CAtlException(E_INVALIDARG);
        header->nDataLength = 0;
        *const_cast<char*>(destination.GetString()) = 0;
        return;
    }
    ReleaseHeader(header);
    using GetNil = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*);
    // 416383仍使用释放前保存的 manager；不能从已释放 header 或全局重新取值。
    Publish(destination, Data(ManagerSlot<GetNil>(manager, 12)(manager)));
}

void ForkAt4170B0(RuntimeCString& destination, int capacity)
{
    const auto* const source = destination.GetString();
    auto* const previous = Header(source);
    auto* manager = previous->pStringMgr;
    const int previousLength = previous->nDataLength;
    using Clone = ATL::IAtlStringMgr* (__thiscall*)(ATL::IAtlStringMgr*);
    manager = ManagerSlot<Clone>(manager, 16)(manager);
    using Allocate = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*, int, int);
    auto* next = ManagerSlot<Allocate>(manager, 0)(manager, capacity, 1);
    if (!next) throw ATL::CAtlException(E_OUTOFMEMORY);
    char* const nextData = Data(next);
    const auto bytes = static_cast<std::uint32_t>((std::min)(previousLength, capacity)) + 1u;
    static_cast<void>(MoveRuntimeBytesAt43D9B0(nextData, bytes, source, bytes));
    next->nDataLength = previousLength;
    // 417114在Clone/Allocate后重读旧manager；Free失败时新块不回收，目标仍为旧指针。
    ReleaseHeader(previous);
    Publish(destination, nextData);
}

void ReallocateAt4314C0(RuntimeCString& destination, int capacity)
{
    auto* previous = Header(destination.GetString());
    if (previous->nAllocLength >= capacity || capacity <= 0)
        throw ATL::CAtlException(E_OUTOFMEMORY);
    using Reallocate = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*, ATL::CStringData*, int, int);
    auto* manager = previous->pStringMgr;
    auto* next = ManagerSlot<Reallocate>(manager, 8)(manager, previous, capacity, 1);
    if (!next) throw ATL::CAtlException(E_OUTOFMEMORY);
    Publish(destination, Data(next));
}

void PrepareWriteAt42D590(RuntimeCString& destination, int length)
{
    const auto* header = Header(destination.GetString());
    length = (std::max)(length, header->nDataLength);
    if (header->nRefs > 1) { ForkAt4170B0(destination, length); return; }
    const int capacity = header->nAllocLength;
    if (capacity >= length) return;
    const int increment = capacity > 0x40000000 ? 0x100000 : capacity / 2;
    const auto grown = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(capacity) + static_cast<std::uint32_t>(increment));
    ReallocateAt4314C0(destination, (std::max)(length, grown));
}

void InvalidParameter(int code)
{
    errno = code;
    _invalid_parameter_noinfo();
}

class DemoTextUnwind final
{
public:
    explicit DemoTextUnwind(RuntimeCString& text) noexcept : text_(&text) {}
    ~DemoTextUnwind() noexcept { if (text_) DestroyRuntimeCStringAt40CD40(text_); }
    void Disarm() noexcept { text_ = nullptr; }
private:
    RuntimeCString* text_;
};
}

void ConstructRuntimeCStringAt40B490(RuntimeCString* out,
    const char* source, int length, ATL::IAtlStringMgr* manager)
{
    if (!manager) throw ATL::CAtlException(E_FAIL);
    if (!source && length != 0) throw ATL::CAtlException(E_INVALIDARG);
    using Allocate = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*, int, int);
    auto* const header = ManagerSlot<Allocate>(manager, 0)(manager, length, 1);
    if (!header) throw ATL::CAtlException(E_OUTOFMEMORY);
    // 40B4B9早于有符号长度/容量检查；此函数没有EH回收动作，不能增加分配守卫。
    Publish(*out, Data(header));
    if (length < 0 || length > header->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
    header->nDataLength = length;
    const_cast<char*>(out->GetString())[length] = 0;
    // 40B4D2在写NUL后重读目标槽；复制容量就是显式length，不是分配容量。
    static_cast<void>(MoveRuntimeBytesAt43D9B0(const_cast<char*>(out->GetString()),
        static_cast<std::uint32_t>(length), source, static_cast<std::uint32_t>(length)));
}

void ConstructRuntimeCStringLeftAt419D00(RuntimeCString* out,
    const RuntimeCString& source, int count, IRuntimeStringManagerSource& strings)
{
    count = (std::max)(count, 0);
    auto* const header = Header(source.GetString());
    if (count >= header->nDataLength)
    {
        Publish(*out, Data(CloneRuntimeCStringDataAt414540(header)));
        return;
    }
    auto* const manager = CloneSubstringManager(header, strings);
    // 419D52：Clone/Capture允许改写source槽，长度保留而chars在回调后重取。
    ConstructRuntimeCStringAt40B490(out, source.GetString(), count, manager);
}

void ConstructRuntimeCStringMidAt420C80(RuntimeCString* out,
    const RuntimeCString& source, int start, int count, IRuntimeStringManagerSource& strings)
{
    start = (std::max)(start, 0);
    count = (std::max)(count, 0);
    if ((std::numeric_limits<int>::max)() - start < count)
        throw ATL::CAtlException(HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW));
    auto* const header = Header(source.GetString());
    const int length = header->nDataLength;
    if (start + count > length)
        count = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(length) -
            static_cast<std::uint32_t>(start));
    if (start > length) count = 0;
    if (start == 0 && count == length)
    {
        Publish(*out, Data(CloneRuntimeCStringDataAt414540(header)));
        return;
    }
    auto* const manager = CloneSubstringManager(header, strings);
    // 420D0F重取chars；超长start的零字节复制仍形成原DWORD地址并Allocate(0)。
    const auto address = reinterpret_cast<std::uintptr_t>(source.GetString()) +
        static_cast<std::uint32_t>(start);
    ConstructRuntimeCStringAt40B490(out, reinterpret_cast<const char*>(address), count, manager);
}

bool LoadRuntimeCStringResourceAt420510(RuntimeCString& destination, std::uint32_t id)
{
    // 先按language=0逐模块探测，命中后再作无语言参数的FindResourceW/取image。
    auto& module=ProcessAtlBaseModuleNative();
    AtlBaseModuleNativeServices services{DefaultWindowClassNativeApi(),DefaultAtlBaseModuleNativeDiagnostics()};
    auto instance=GetAtlBaseResourceInstanceNativeAt53EA42(module,0,services);
    std::uint32_t index=1;
    while(instance&&!ATL::AtlGetStringResourceImage(instance,id,WORD{0}))
    {
        // 42056F逐次查原表；null实例立即结束，不跳过空项或转现代ATL。
        instance=GetAtlBaseResourceInstanceNativeAt53EA42(module,std::bit_cast<std::int32_t>(index),services);
        ++index;
    }
    if (!instance) return false;
    const auto* const resource = ATL::AtlGetStringResourceImage(instance, id);
    if (!resource) return false;
    const int length = ::WideCharToMultiByte(CP_THREAD_ACP, 0,
        resource->achString, resource->nLength, nullptr, 0, nullptr, nullptr);
    if (length < 0) throw ATL::CAtlException(E_INVALIDARG);
    char* data = const_cast<char*>(destination.GetString());
    const auto* header = Header(data);
    const auto prepare = (1u - static_cast<std::uint32_t>(header->nRefs)) |
        (static_cast<std::uint32_t>(header->nAllocLength) - static_cast<std::uint32_t>(length));
    if (std::bit_cast<std::int32_t>(prepare) < 0)
    {
        PrepareWriteAt42D590(destination, length);
        data = const_cast<char*>(destination.GetString());
    }
    // 4205FB重读image长度；首遍0也调用第二遍，容量不含NUL且不消费返回值。
    static_cast<void>(::WideCharToMultiByte(CP_THREAD_ACP, 0,
        resource->achString, resource->nLength, data, length, nullptr, nullptr));
    auto* finalHeader = Header(destination.GetString()); // 42060D不是沿用转换前的目标。
    if (length > finalHeader->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
    finalHeader->nDataLength = length;
    const_cast<char*>(destination.GetString())[length] = 0; // 420617独立重读后写NUL。
    return true;
}

void InitializeDemoShortRuntimeCStringAt420650(RuntimeCString& result,
    const wchar_t* source, std::int32_t byteLength, IRuntimeStringManagerSource& strings,
    std::size_t availableSourceBytes)
{
    // 返回槽 GetNil 成功之后才有 state1/bit0 的展开资格。
    InitializeEmptyRuntimeCString(result, strings);
    DemoTextUnwind resultUnwind(result);
    SetRuntimeCStringAt437680(result, "", 0);
    if (byteLength != 0)
    {
        union Cell { RuntimeCString value; Cell() noexcept {} ~Cell() noexcept {} } temporary;
        InitializeEmptyRuntimeCString(temporary.value, strings);
        DemoTextUnwind temporaryUnwind(temporary.value);
        char* cachedCharacters = const_cast<char*>(temporary.value.GetString());
        const auto wideCount = byteLength >> 1;
        if (wideCount > 0)
        {
            // 42070F 的空源判断晚于第二次 GetNil；零/负 WCHAR 数仍会走赋值及释放。
            if (!source) throw ATL::CAtlException(E_INVALIDARG);
            if (static_cast<std::size_t>(wideCount) * sizeof(wchar_t) > availableSourceBytes)
                throw DemoShortTextInputTruncated{};
            const int length = ::WideCharToMultiByte(CP_THREAD_ACP, 0, source, wideCount,
                nullptr, 0, nullptr, nullptr);
            if (length < 0) throw ATL::CAtlException(E_INVALIDARG);
            const auto* header = Header(cachedCharacters);
            const auto prepare = (1u - static_cast<std::uint32_t>(header->nRefs)) |
                (static_cast<std::uint32_t>(header->nAllocLength) - static_cast<std::uint32_t>(length));
            if (std::bit_cast<std::int32_t>(prepare) < 0)
            {
                PrepareWriteAt42D590(temporary.value, length);
                cachedCharacters = const_cast<char*>(temporary.value.GetString());
            }
            // 420760/420766：0 长度也调用第二遍；提交到缓存 ESI，不重取目标槽。
            static_cast<void>(::WideCharToMultiByte(CP_THREAD_ACP, 0, source, wideCount,
                cachedCharacters, length, nullptr, nullptr));
            auto* finalHeader = Header(cachedCharacters);
            if (length > finalHeader->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
            finalHeader->nDataLength = length;
            cachedCharacters[length] = 0;
        }
        AssignRuntimeCStringAt410220(result, temporary.value);
        // 420787 先退回 state0：正常 Free 失败不重试临时串，但仍展开 result。
        temporaryUnwind.Disarm();
        ReleaseRuntimeCStringHeader(Header(cachedCharacters));
    }
    resultUnwind.Disarm();
}

int MoveRuntimeBytesAt43D9B0(void* destination, std::uint32_t capacity,
    const void* source, std::uint32_t length)
{
    if (!length) return 0;
    if (!destination) { InvalidParameter(EINVAL); return EINVAL; }
    if (source && capacity >= length)
    {
        std::memmove(destination, source, length);
        return 0;
    }
    // 43DA01先清目标整个容量，再设置errno并通知可返回的处理器。
    // 不可换成现代memmove_s（不清零）或memcpy_s（成功分支不保留重叠移动）。
    std::memset(destination, 0, capacity);
    const int error = source ? ERANGE : EINVAL;
    InvalidParameter(error);
    return error;
}

void AssignRuntimeCStringWideAt4102D0(RuntimeCString& destination, const wchar_t* source)
{
    AssignRuntimeCStringWideAt4102D0(&destination, source);
}

void AssignRuntimeCStringWideAt4102D0(RuntimeCString* destinationPointer, const wchar_t* source)
{
    if (!source)
    {
        if (!destinationPointer) _invalid_parameter_noinfo_noreturn();
        EmptyAt416340(*destinationPointer);
        return;
    }
    const int measured = ::WideCharToMultiByte(CP_THREAD_ACP, 0, source, -1,
        nullptr, 0, nullptr, nullptr);
    // 4102F4是LEA减1再有符号JLE；显式DWORD回绕，不引入C++有符号溢出。
    const auto length = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(measured) - 1u);
    // 4102FB/416340首次访问目标都在查询之后；空指针不是跳过Win32的理由。
    // 明确终止代替原硬故障，不在查询前构造一个无效的C++引用。
    if (!destinationPointer) _invalid_parameter_noinfo_noreturn();
    auto& destination = *destinationPointer;
    if (length <= 0) { EmptyAt416340(destination); return; }

    char* data = const_cast<char*>(destination.GetString());
    const auto* header = Header(data);
    const auto prepare = (1u - static_cast<std::uint32_t>(header->nRefs)) |
        (static_cast<std::uint32_t>(header->nAllocLength) - static_cast<std::uint32_t>(length));
    if (std::bit_cast<std::int32_t>(prepare) < 0)
    {
        PrepareWriteAt42D590(destination, length);
        data = const_cast<char*>(destination.GetString()); // 410316：回调后重读，不沿用旧data。
    }
    // 410318..410325：源地址保持入口值；容量恰为length而非length+1。
    // 不消费返回值、不预清缓冲，也不在转换失败时重试或走Empty。
    static_cast<void>(::WideCharToMultiByte(CP_THREAD_ACP, 0, source, -1,
        data, length, nullptr, nullptr));
    auto* finalHeader = Header(destination.GetString()); // 41032B：第二遍返回后再次取当前目标。
    if (length > finalHeader->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
    finalHeader->nDataLength = length;
    const_cast<char*>(destination.GetString())[length] = 0; // 410335独立重读后写终止字节。
}

void AppendRuntimeCStringAt413550(RuntimeCString& destination, const char* source, int limit)
{
    char* data = const_cast<char*>(destination.GetString());
    const auto offset = reinterpret_cast<std::uintptr_t>(source) - reinterpret_cast<std::uintptr_t>(data);
    const int previousLength = Header(data)->nDataLength;
    if (limit < 0) throw ATL::CAtlException(E_INVALIDARG);
    const auto added = source ? static_cast<std::uint32_t>(strnlen_s(source, static_cast<std::size_t>(limit))) : 0u;
    if (static_cast<std::int32_t>(0x7FFFFFFFu - added) < previousLength)
        throw ATL::CAtlException(E_INVALIDARG);
    const auto length = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(previousLength) + added);
    if (length < 0) throw ATL::CAtlException(E_INVALIDARG);
    const auto* header = Header(destination.GetString());
    const auto prepare = (1u - static_cast<std::uint32_t>(header->nRefs)) |
        (static_cast<std::uint32_t>(header->nAllocLength) - static_cast<std::uint32_t>(length));
    if (std::bit_cast<std::int32_t>(prepare) < 0)
    {
        PrepareWriteAt42D590(destination, length);
        data = const_cast<char*>(destination.GetString());
    }
    // 4135B4 沿用分配前的长度和相对偏移；不是按新 header 再决定追加位置。
    if (offset <= static_cast<std::uint32_t>(previousLength))
        source = reinterpret_cast<const char*>(reinterpret_cast<std::uintptr_t>(data) + offset);
    static_cast<void>(MoveRuntimeBytesAt43D9B0(data + previousLength, added, source, added));
    auto* finalHeader = Header(destination.GetString());
    if (length > finalHeader->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
    finalHeader->nDataLength = length;
    const_cast<char*>(destination.GetString())[length] = 0;
}

void PrepareRuntimeCStringWriteAt42D590(RuntimeCString& destination, int length)
{
    PrepareWriteAt42D590(destination, length);
}

char* GetRuntimeCStringBufferSetLengthAt417260(RuntimeCString& destination,int length)
{
    if(length<0)throw ATL::CAtlException(E_INVALIDARG);
    auto* data=const_cast<char*>(destination.GetString());
    const auto* header=Header(data);
    const auto prepare=(1u-static_cast<std::uint32_t>(header->nRefs))|
        (static_cast<std::uint32_t>(header->nAllocLength)-static_cast<std::uint32_t>(length));
    if(std::bit_cast<std::int32_t>(prepare)<0)
    {PrepareWriteAt42D590(destination,length);data=const_cast<char*>(destination.GetString());}
    if(length>Header(data)->nAllocLength)throw ATL::CAtlException(E_INVALIDARG);
    Header(data)->nDataLength=length;
    const_cast<char*>(destination.GetString())[length]=0;
    return data; // 417292缓存返回值，终止字节的目的仍独立重读。
}

void ReleaseRuntimeCStringBufferAt486B10(RuntimeCString& destination,int length)
{
    if(length==-1)
    {
        const auto* data=destination.GetString();
        length=data?static_cast<int>(strnlen_s(data,Header(data)->nAllocLength)):0;
    }
    if(length<0||length>Header(destination.GetString())->nAllocLength)throw ATL::CAtlException(E_INVALIDARG);
    Header(destination.GetString())->nDataLength=length;
    const_cast<char*>(destination.GetString())[length]=0;
}

void ConcatenateRuntimeCStringAt4145E0(RuntimeCString& destination,
    const char* first, std::uint32_t firstLength, const char* second, std::uint32_t secondLength)
{
    const auto length = std::bit_cast<std::int32_t>(firstLength + secondLength);
    if (length < 0) throw ATL::CAtlException(E_INVALIDARG);
    auto* buffer = const_cast<char*>(destination.GetString());
    const auto* header = Header(buffer);
    const auto prepare = (1u - static_cast<std::uint32_t>(header->nRefs)) |
        (static_cast<std::uint32_t>(header->nAllocLength) - static_cast<std::uint32_t>(length));
    if (std::bit_cast<std::int32_t>(prepare) < 0)
    {
        PrepareWriteAt42D590(destination, length);
        buffer = const_cast<char*>(destination.GetString());
    }
    // 41460E..41462A：两次复制沿用缓存buffer，输入不做自别名重定位。
    static_cast<void>(MoveRuntimeBytesAt43D9B0(buffer, firstLength, first, firstLength));
    static_cast<void>(MoveRuntimeBytesAt43D9B0(buffer + firstLength, secondLength, second, secondLength));
    auto* finalHeader = Header(destination.GetString());
    if (length > finalHeader->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
    finalHeader->nDataLength = length;
    const_cast<char*>(destination.GetString())[length] = 0;
}

void FormatRuntimeCStringVAt417150(RuntimeCString& destination, const char* format, va_list args)
{
    static_assert(sizeof(va_list) == 4);
    if (!format) throw ATL::CAtlException(E_INVALIDARG);
    // 43D9A0返回options地址；测长加bit1，第二遍在分配回调后重读原options。
    auto* const options = __local_stdio_printf_options();
    const int length = __stdio_common_vsprintf(*options | 2ull, nullptr, 0, format, nullptr, args);
    if (length < 0) throw ATL::CAtlException(E_FAIL);
    char* data = const_cast<char*>(destination.GetString());
    const auto* header = Header(data);
    const auto prepare = (1u - static_cast<std::uint32_t>(header->nRefs)) |
        (static_cast<std::uint32_t>(header->nAllocLength) - static_cast<std::uint32_t>(length));
    if (std::bit_cast<std::int32_t>(prepare) < 0)
    {
        PrepareWriteAt42D590(destination, length);
        data = const_cast<char*>(destination.GetString());
    }
    // 不复制format/参数正文，不消费第二遍返回值，不经过ATL GetBuffer/ReleaseBuffer。
    static_cast<void>(__stdio_common_vsprintf_s(*options, data,
        static_cast<std::size_t>(length) + 1u, format, nullptr, args));
    auto* finalHeader = Header(destination.GetString());
    if (length > finalHeader->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
    finalHeader->nDataLength = length;
    const_cast<char*>(destination.GetString())[length] = 0;
}

void AppendRuntimeCStringLineFeedAt479F48(RuntimeCString& destination)
{
    const auto* header = Header(destination.GetString());
    const int previousLength = header->nDataLength;
    const auto length = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(previousLength) + 1u);
    if (length < 0) throw ATL::CAtlException(E_INVALIDARG);
    const auto prepare = (1u - static_cast<std::uint32_t>(header->nRefs)) |
        (static_cast<std::uint32_t>(header->nAllocLength) - static_cast<std::uint32_t>(length));
    if (std::bit_cast<std::int32_t>(prepare) < 0) PrepareWriteAt42D590(destination, length);
    const_cast<char*>(destination.GetString())[previousLength] = '\n';
    auto* finalHeader = Header(destination.GetString());
    if (length > finalHeader->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
    finalHeader->nDataLength = length;
    const_cast<char*>(destination.GetString())[length] = 0;
}

void LowerRuntimeCStringAt481690(RuntimeCString& destination)
{
    const auto* header = Header(destination.GetString());
    const int length = header->nDataLength;
    if (length < 0) throw ATL::CAtlException(E_INVALIDARG);
    const auto prepare = (1u - static_cast<std::uint32_t>(header->nRefs)) |
        (static_cast<std::uint32_t>(header->nAllocLength) - static_cast<std::uint32_t>(length));
    if (std::bit_cast<std::int32_t>(prepare) < 0) PrepareWriteAt42D590(destination, length);
    const auto result = _mbslwr_s(reinterpret_cast<unsigned char*>(
        const_cast<char*>(destination.GetString())), static_cast<std::uint32_t>(length) + 1u);
    // 4136D0：0及80原样通过；12=>OOM，22/34=>INVALIDARG，其他=>FAIL。
    if (result != 0 && result != 80)
        throw ATL::CAtlException(result == 12 ? E_OUTOFMEMORY :
            result == 22 || result == 34 ? E_INVALIDARG : E_FAIL);
    auto* finalHeader = Header(destination.GetString());
    if (length > finalHeader->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
    finalHeader->nDataLength = length;
    const_cast<char*>(destination.GetString())[length] = 0;
}

void SetRuntimeCStringAt437680(RuntimeCString& destination, const char* source, int length)
{
    if (!length) { EmptyAt416340(destination); return; }
    if (!source) throw ATL::CAtlException(E_INVALIDARG);
    char* data = const_cast<char*>(destination.GetString());
    const auto* header = Header(data);
    const int previousLength = header->nDataLength;
    const auto offset = reinterpret_cast<std::uintptr_t>(source) - reinterpret_cast<std::uintptr_t>(data);
    if (length < 0) throw ATL::CAtlException(E_INVALIDARG);
    // 原指令按32位回绕后检验符号，避免C++有符号溢出改变损坏头部的分支。
    const auto prepare = (1u - static_cast<std::uint32_t>(header->nRefs)) |
        (static_cast<std::uint32_t>(header->nAllocLength) - static_cast<std::uint32_t>(length));
    if (std::bit_cast<std::int32_t>(prepare) < 0)
    {
        PrepareWriteAt42D590(destination, length);
        data = const_cast<char*>(destination.GetString());
    }
    if (offset <= static_cast<std::uint32_t>(previousLength))
    {
        // 别名只保存原offset；分离/重分配后用新data重定位，而非继续读取旧块。
        auto* relocated = reinterpret_cast<const char*>(reinterpret_cast<std::uintptr_t>(data) + offset);
        if (!relocated) InvalidParameter(EINVAL);
        else if (static_cast<std::uint32_t>(Header(data)->nAllocLength) < static_cast<std::uint32_t>(length))
            InvalidParameter(ERANGE);
        else std::memmove(data, relocated, static_cast<std::size_t>(length));
    }
    else
    {
        static_cast<void>(MoveRuntimeBytesAt43D9B0(data, static_cast<std::uint32_t>(Header(data)->nAllocLength),
            source, static_cast<std::uint32_t>(length)));
    }
    // invalid-parameter处理器可返回并改变目标，437727和437732均重新读取指针。
    auto* finalHeader = Header(destination.GetString());
    if (length > finalHeader->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
    finalHeader->nDataLength = length;
    const_cast<char*>(destination.GetString())[length] = 0;
}
}
