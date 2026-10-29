#include <Runtime/RuntimeCString.h>

#include <atlstr.h>
#include <bit>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class Function>
Function ManagerSlot(ATL::IAtlStringMgr* manager, std::size_t offset) noexcept
{
    const std::byte* table;
    std::memcpy(&table, manager, sizeof(table));
    Function result;
    std::memcpy(&result, table + offset, sizeof(result));
    return result;
}

void ReleaseHeader(ATL::CStringData* data)
{
    if (_InterlockedDecrement(&data->nRefs) > 0) return;
    using Free = void (__thiscall*)(ATL::IAtlStringMgr*, ATL::CStringData*);
    auto* manager = data->pStringMgr;
    ManagerSlot<Free>(manager, 4)(manager, data);
}

class ProcessStringManagerSource final : public IRuntimeStringManagerSource
{
public:
    ATL::IAtlStringMgr* CaptureAt417360() override
    {
        // 4173A7..417449：线程安全懒建，借用进程堆，不为每个字符串另建堆。
        return ATL::CAtlStringMgr::GetInstance();
    }
};

}

void DestroyRuntimeCStringAt40CD40(RuntimeCString* element)
{
    static_assert(sizeof(void*) == 4);
    static_assert(offsetof(ATL::CStringData, pStringMgr) == 0);
    static_assert(offsetof(ATL::CStringData, nRefs) == 12);
    auto* data = reinterpret_cast<ATL::CStringData*>(
        reinterpret_cast<std::uintptr_t>(element->GetString()) - sizeof(ATL::CStringData));
    // 原 PE 5ADCC8 -> 417200 -> 堆管理器虚槽 +04；ECX=manager，栈上传 header，ret 4。
    // 调用者不消费 EAX，不能采用反编译器猜测的 stdcall/int 声明。
    // 不调用 ATL 析构、Release 或类型化 Free：v143 的 throw() 会提前终止，
    // 阻断原数组前缀清理，以及 47A762、47A85D/47A932、47A9D8 的局部捕获。
    // 原始虚槽调用不能移除实际目标自身的 noexcept。
    // v143 IAtlStringMgr::Free 覆盖方法内部抛出时仍在该边界终止。
    ReleaseHeader(data);
}

void ReleaseRuntimeCStringHeader(ATL::CStringData* header) { ReleaseHeader(header); }

void InitializeEmptyRuntimeCString(RuntimeCString& destination, IRuntimeStringManagerSource& strings)
{
    auto* manager = strings.CaptureAt417360();
    if (!manager) throw ATL::CAtlException(E_FAIL);
    using GetNil = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*);
    auto* header = ManagerSlot<GetNil>(manager, 12)(manager);
    // 原码直接add 10h，不解引用/校验返回header；空结果也不改成现代空串。
    const auto pointer = reinterpret_cast<std::uintptr_t>(header) + sizeof(ATL::CStringData);
    std::memcpy(&destination, &pointer, sizeof(pointer));
}

ATL::CStringData* CloneRuntimeCStringDataAt414540(ATL::CStringData* source)
{
    using Clone = ATL::IAtlStringMgr* (__thiscall*)(ATL::IAtlStringMgr*);
    auto* manager = source->pStringMgr;
    manager = ManagerSlot<Clone>(manager, 16)(manager);
    // 41454F/41455A均在Clone回调之后读原header，不能取回调前的refs/manager。
    if (source->nRefs >= 0 && manager == source->pStringMgr)
    {
        _InterlockedIncrement(&source->nRefs);
        return source;
    }
    using Allocate = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*, int, int);
    auto* next = ManagerSlot<Allocate>(manager, 0)(manager, source->nDataLength, 1);
    if (!next) throw ATL::CAtlException(E_OUTOFMEMORY);
    next->nDataLength = source->nDataLength;
    const auto size = static_cast<std::uint32_t>(source->nDataLength) + 1u;
    static_cast<void>(MoveRuntimeBytesAt43D9B0(next->data(), size, source->data(), size));
    return next;
}

IRuntimeStringManagerSource& DefaultRuntimeStringManagerSource() noexcept
{
    static ProcessStringManagerSource source;
    return source;
}

void AssignRuntimeCStringAt410220(RuntimeCString& destination, const RuntimeCString& source)
{
    auto* const previous = reinterpret_cast<ATL::CStringData*>(
        reinterpret_cast<std::uintptr_t>(destination.GetString()) - sizeof(ATL::CStringData));
    auto* const incoming = reinterpret_cast<ATL::CStringData*>(
        reinterpret_cast<std::uintptr_t>(source.GetString()) - sizeof(ATL::CStringData));
    if (previous == incoming) return;
    if (previous->nRefs < 0 || previous->pStringMgr != incoming->pStringMgr)
    {
        SetRuntimeCStringAt437680(destination, source.GetString(), source.GetLength());
        return;
    }
    // 41022C缓存旧header；Clone回调即使改了destination，410257仍减持缓存的旧串。
    // 410265旧Free失败时，新引用不回收且结果不发布。
    auto* const next = CloneRuntimeCStringDataAt414540(incoming);
    ReleaseHeader(previous);
    const char* data = static_cast<const char*>(next->data());
    // 41026F只写已构造对象的4-byte指针，不调用会额外减持的ATL赋值。
    std::memcpy(&destination, &data, sizeof(data));
}

void __cdecl FormatRuntimeCStringAt417130(RuntimeCString& destination, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    try { FormatRuntimeCStringVAt417150(destination, format, args); }
    catch (...) { va_end(args); throw; }
    va_end(args);
}

}
