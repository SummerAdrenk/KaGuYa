#include <Runtime/RuntimeCStringException.h>
#include <Runtime/RuntimeCStringConcatenation.h>

#include <ehdata.h>
#include <cstring>
#include <exception>
#include <new>
#include <stdexcept>

#if !defined(_MSC_VER) || !defined(_M_IX86) || _EH_RELATIVE_TYPEINFO
#error RuntimeCStringException requires the verified MSVC x86 absolute CT2 ABI.
#endif

namespace KaGuYa::Start::Runtime
{
namespace
{
// builtin 必须只生成 TI 地址；独立 /Od、/O2 验证检查此函数的反汇编。
__declspec(noinline) const ThrowInfo* CompilerCStringThrowInfo(const RuntimeCString& value) noexcept
{
    return static_cast<const ThrowInfo*>(std::__GetExceptionInfo(
        reinterpret_cast<const RuntimeCStringExceptionType&>(value)));
}

// EH 以 ECX=this、栈上传 source 调用复制叶；fastcall 的第二寄存器参数不使用。
void* __fastcall CopyCStringException(void* output, void*, const void* source)
{
    try
    {
        const char* chars;
        std::memcpy(&chars, source, sizeof(chars));
        auto* header = reinterpret_cast<ATL::CStringData*>(const_cast<char*>(chars)) - 1;
        auto* copied = CloneRuntimeCStringDataAt414540(header);
        const void* data = copied + 1;
        std::memcpy(output, &data, sizeof(data));
        return output;
    }
    catch (...)
    {
        // 原 543951 调 terminate；v143 frame.cpp 的对应边界改成 abort。
        // 在桥内恢复原复制失败合同，不把 CRT 的任意 fail-fast 当作 terminate。
        std::terminate();
    }
}

void __fastcall DestroyCStringException(void* object, void*)
{
    // 不能用 ATL 析构或 std::destroy_at：原正常异常对象销毁允许 Free 外传。
    DestroyRuntimeCStringAt40CD40(static_cast<RuntimeCString*>(object));
}

struct CatchableTypes2 final
{
    int count;
    const CatchableType* types[2];
};
static_assert(sizeof(CatchableType) == 28 && sizeof(ThrowInfo) == 16);
static_assert(sizeof(CatchableTypes2) == 12 && sizeof(void*) == 4);

struct CStringExceptionMetadata final
{
    explicit CStringExceptionMetadata(const ThrowInfo* original)
        : types{}, array{2, {&types[0], &types[1]}}, info{}
    {
        // 保留真正编译器 TypeDescriptor 和继承信息；不猜测新增基类或工具集布局。
        constexpr const char* names[] = {
            ".?AV?$CStringT@DV?$StrTraitATL@DV?$ChTraitsCRT@D@ATL@@@ATL@@@ATL@@",
            ".?AV?$CSimpleStringT@D$0A@@ATL@@"};
        if (!original || original->attributes || original->pForwardCompat ||
            !original->pCatchableTypeArray || original->pCatchableTypeArray->nCatchableTypes != 2)
            throw std::logic_error("Unsupported CStringT exception ABI");
        for (unsigned i = 0; i != 2; ++i)
        {
            const auto* type = original->pCatchableTypeArray->arrayOfCatchableTypes[i];
            if (!type || type->properties || !type->pType ||
                std::strcmp(type->pType->name, names[i]) != 0 || type->sizeOrOffset != 4 ||
                type->thisDisplacement.mdisp != 0 || type->thisDisplacement.pdisp != -1 ||
                type->thisDisplacement.vdisp != 0 || !type->copyFunction)
                throw std::logic_error("Unsupported CStringT catchable type ABI");
            types[i] = *type;
            types[i].copyFunction = reinterpret_cast<PMFN>(&CopyCStringException);
        }
        info = *original;
        info.pmfnUnwind = reinterpret_cast<PMFN>(&DestroyCStringException);
        info.pCatchableTypeArray = reinterpret_cast<const CatchableTypeArray*>(&array);
    }
    CStringExceptionMetadata(const CStringExceptionMetadata&) = delete;
    CStringExceptionMetadata& operator=(const CStringExceptionMetadata&) = delete;
    _s_CatchableType types[2];
    CatchableTypes2 array;
    _s_ThrowInfo info;
};

union ExceptionPayload
{
    ExceptionPayload() noexcept {}
    ~ExceptionPayload() noexcept {}
    RuntimeCStringConcatenation value;
};
static_assert(sizeof(ExceptionPayload) == sizeof(RuntimeCStringExceptionType));
}

[[noreturn]] void ThrowRuntimeCStringConcatAt466760(const RuntimeCString& prefix,
    const char* suffix, IRuntimeStringManagerSource& strings)
{
    // 表的自指针必须指向静态本体；不按值返回，无堆对象或对象旁表。
    // 编译器局部静态初始化保护不承载 CString 的所有权或逐对象信息。
    static const CStringExceptionMetadata metadata(CompilerCStringThrowInfo(prefix));
    ExceptionPayload payload;
    ::new (static_cast<void*>(&payload.value)) RuntimeCStringConcatenation(prefix, suffix, strings);
    // 这是 MSVC 原始 EH 对象交付，不声称是 ISO C++ 的 CStringT 构造。
    // _CxxThrowException 不复制正文；EH 最终调用上述可抛 destroy 叶。
    _CxxThrowException(&payload.value, reinterpret_cast<_ThrowInfo*>(
        const_cast<_s_ThrowInfo*>(&metadata.info)));
}
}
