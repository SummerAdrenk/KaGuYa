#include "ScriptFileCommandSupport.h"

#include <new>

namespace KaGuYa::Start::Runtime::FileCommandDetail
{
template<class F> F ManagerSlot(ATL::IAtlStringMgr* manager, std::size_t offset) noexcept
{
    const std::byte* table;
    // 读取原生vptr字节，保留后续可抛虚槽调用。
    std::memcpy(&table, static_cast<const void*>(manager), sizeof(table));
    F result;
    std::memcpy(&result, table + offset, sizeof(result));
    return result;
}
void ConstructReportAt4CA280(RuntimeCString& out, const char* prefix,
    const RuntimeCString& detail, IRuntimeStringManagerSource& strings)
{
    using Clone = ATL::IAtlStringMgr* (__thiscall*)(ATL::IAtlStringMgr*);
    using Nil = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*);
    auto* manager = (reinterpret_cast<const ATL::CStringData*>(detail.GetString()) - 1)->pStringMgr;
    if (manager) manager = ManagerSlot<Clone>(manager, 16)(manager);
    if (!manager)
    {
        auto* global = strings.CaptureAt417360();
        if (!global) throw ATL::CAtlException(E_FAIL); // 原空指针解引用的明确安全边界。
        manager = ManagerSlot<Clone>(global, 16)(global);
        if (!manager) throw ATL::CAtlException(E_FAIL);
    }
    const auto chars = reinterpret_cast<std::uintptr_t>(ManagerSlot<Nil>(manager, 12)(manager)) + 16u;
    // GetNil已提供引用；只发布原生指针，不额外构造或赋值CString。
    std::memcpy(static_cast<void*>(&out), &chars, sizeof(chars));
    TextUnwind cleanup{&out};
    // 4CA2EA之后重读detail；Clone/GetNil可能更换其缓冲，不缓存调用前的字符。
    const char* const current = detail.GetString();
    const auto prefixLength = prefix ? static_cast<std::uint32_t>(std::strlen(prefix)) : 0u;
    const auto detailLength = static_cast<std::uint32_t>(
        (reinterpret_cast<const ATL::CStringData*>(current) - 1)->nDataLength);
    ConcatenateRuntimeCStringAt4145E0(out, prefix, prefixLength, current, detailLength);
    cleanup.value = nullptr;
}
void DeleteConstructing(IEnvironmentRootMemory& memory, void* p) noexcept { memory.DeleteBody(p, 4); }
Command CreateCommand(ScrContainer& parser, IScriptFileCommandSource& source, IEnvironmentRootMemory& memory,
    ScriptControlCommandKind kind)
{
    void* allocation = memory.AllocateBody(4);
    auto* body = allocation ? ::new (allocation) ScriptControlCommandNative : nullptr;
    Command result;
    if (body)
    {
        try
        {
            const auto root = source.CaptureAfterEnsureAt413BE0();
            const auto pc = root.paramsAt47BDB0(root.object).programCounter;
            body->source = reinterpret_cast<const std::byte*>(parser.bytesAt04.DataAt(pc));
            const auto& bytes = parser.bytesAt04.RawSlot();
            result.base = bytes.baseAt00;
            if (bytes.controlAt08)
            {
                const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(bytes.controlAt08);
                result.extent = std::uint64_t{control->countAt10} * control->strideAt0C;
                result.knownExtent = true;
            }
        }
        catch (...) { DeleteConstructing(memory, body); throw; }
    }
    // state2在进入4C82D0之前已撤销；control分配失败不能擅自回收孤立body。
    CreateScriptControlCommandReference(result.reference, body, kind, memory);
    return result;
}
void Show(IScrVmCommandErrorSink& errors, const char* text)
{
    const auto& environment = errors.CaptureAt4180C0();
    static_cast<void>(errors.ShowMessageBoxA(reinterpret_cast<HWND>(environment.windowAt2C),
        text, environment.titleAt10.GetString(), MB_ICONERROR));
}
}
