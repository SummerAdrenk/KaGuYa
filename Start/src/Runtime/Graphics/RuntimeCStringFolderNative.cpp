#include <Runtime/RuntimeCStringFolderNative.h>

#include <bit>
#include <cstring>
#include <mbstring.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
ATL::CStringData* Header(const RuntimeCString& text) noexcept
{
    return reinterpret_cast<ATL::CStringData*>(const_cast<char*>(text.GetString()) - 16);
}

RuntimeCStringFolderNativeBody8h* __fastcall DeleteScanner(
    RuntimeCStringFolderNativeBody8h* body, void*, std::uint32_t flags) noexcept
{
    return DeleteRuntimeStringScannerNativeAt4122F0(*body, flags);
}
RuntimeCStringFolderNativeBody8h* __fastcall DeleteFolder(
    RuntimeCStringFolderNativeBody8h* body, void*, std::uint32_t flags) noexcept
{
    return DeleteRuntimeCStringFolderNativeAt440530(*body, flags);
}
using DeleteSlot = decltype(RuntimeCStringFolderNativeVtable::deletingDestructorAt00);
const RuntimeCStringFolderNativeVtable scannerMethods{
    reinterpret_cast<DeleteSlot>(&DeleteScanner)};
const RuntimeCStringFolderNativeVtable folderMethods{
    reinterpret_cast<DeleteSlot>(&DeleteFolder)};

// 5E3C54：state0只清CString；state1清基类；state2先清临时串再落入state1。
struct BodyUnwind final
{
    RuntimeCStringFolderNativeBody8h& body;
    int state = -1;
    ~BodyUnwind() noexcept
    {
        if (state == 0) DestroyRuntimeCStringAt40CD40(&body.textAt04);
        else if (state == 1) DestroyRuntimeCStringFolderNativeAt440360(body);
    }
};
union TemporaryText
{
    TemporaryText() noexcept {}
    ~TemporaryText() noexcept {}
    RuntimeCString value;
};
struct TemporaryUnwind final
{
    RuntimeCString& value;
    bool armed = true;
    ~TemporaryUnwind() noexcept
    {
        if (armed) DestroyRuntimeCStringAt40CD40(&value);
    }
};
}

const RuntimeCStringFolderNativeVtable& RuntimeStringScannerNativeMethods() noexcept
{
    return scannerMethods; // PE 5ADD38，唯一槽4122F0。
}
const RuntimeCStringFolderNativeVtable& RuntimeCStringFolderNativeMethods() noexcept
{
    return folderMethods; // PE 5B1D94，唯一槽440530。
}

RuntimeCStringFolderNativeBody8h* ConstructRuntimeCStringFolderNativeAt440100(
    RuntimeCStringFolderNativeBody8h& body, const char* rawPath,
    IRuntimeStringManagerSource& strings)
{
    body.vptrAt00 = &scannerMethods;
    BodyUnwind unwind{body};
    InitializeEmptyRuntimeCString(body.textAt04, strings);
    unwind.state = 0;
    SetRuntimeCStringAt437680(body.textAt04, "", 0);
    unwind.state = 1;
    body.vptrAt00 = &folderMethods;
    // 440191..4401AD：不能把strlen前移到GetNil之前，也不复制入口路径。
    const auto length = rawPath ? static_cast<std::uint32_t>(std::strlen(rawPath)) : 0u;
    SetRuntimeCStringAt437680(body.textAt04, rawPath, std::bit_cast<std::int32_t>(length));
    const auto* match = ::_mbsrchr(
        reinterpret_cast<const unsigned char*>(body.textAt04.GetString()), '\\');
    bool empty = !match;
    std::int32_t prefixLength = 0;
    if (match)
    {
        // 4401C6：扫描后重读当前chars；DWORD相减与INC保持原有符号判定。
        const auto offset = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(match)) -
            static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(body.textAt04.GetString()));
        prefixLength = std::bit_cast<std::int32_t>(offset + 1u);
        empty = std::bit_cast<std::int32_t>(offset) < 0 ||
            prefixLength >= body.textAt04.GetLength();
    }
    if (empty)
    {
        SetRuntimeCStringAt437680(body.textAt04, "", 0);
    }
    else
    {
        TemporaryText temporary;
        // 4401DA..44026B与419D00/40B490同序：Clone后晚读源，Allocate后不重新取源。
        // 成功返回才进入state2，分配成功但容量校验失败不补回收临时块。
        ConstructRuntimeCStringLeftAt419D00(&temporary.value, body.textAt04, prefixLength, strings);
        auto* const incoming = Header(temporary.value);
        TemporaryUnwind temporaryUnwind{temporary.value};
        AssignRuntimeCStringAt410220(body.textAt04, temporary.value);
        // 4402D4先撤临时串资格，4402D8释放此前缓存的EDI，不因回调重取槽。
        temporaryUnwind.armed = false;
        ReleaseRuntimeCStringHeader(incoming);
    }
    unwind.state = -1;
    return &body;
}

void ReleaseRuntimeCStringFolderInlineAt440AEE(RuntimeCStringFolderNativeBody8h& body)
{
    auto* const header = Header(body.textAt04);
    body.vptrAt00 = &scannerMethods;
    ReleaseRuntimeCStringHeader(header);
}

void DestroyRuntimeCStringFolderNativeAt440360(RuntimeCStringFolderNativeBody8h& body) noexcept
{
    // 5E3C90.EHFlags=5；40F980的5DECC4同样为5，不允许Free异常穿出。
    ReleaseRuntimeCStringFolderInlineAt440AEE(body);
}

RuntimeCStringFolderNativeBody8h* DeleteRuntimeCStringFolderNativeAt440530(
    RuntimeCStringFolderNativeBody8h& body, std::uint32_t flags) noexcept
{
    // 5E3CB4.EHFlags=5；flags3仍是scalar，原sized delete明确8字节。
    ReleaseRuntimeCStringFolderInlineAt440AEE(body);
    if (flags & 1u) ::operator delete(&body, sizeof(body));
    return &body;
}

RuntimeCStringFolderNativeBody8h* DeleteRuntimeStringScannerNativeAt4122F0(
    RuntimeCStringFolderNativeBody8h& body, std::uint32_t flags) noexcept
{
    // 5DECE8.EHFlags=5；独立保留基表槽身份，删除行为与派生槽相同。
    ReleaseRuntimeCStringFolderInlineAt440AEE(body);
    if (flags & 1u) ::operator delete(&body, sizeof(body));
    return &body;
}
}
