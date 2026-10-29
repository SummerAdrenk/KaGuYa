#pragma once

#include <Runtime/RuntimeCString.h>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
struct RuntimeCStringFolderNativeBody8h;
struct RuntimeCStringFolderNativeVtable final
{
    RuntimeCStringFolderNativeBody8h* (__thiscall* deletingDestructorAt00)(
        RuntimeCStringFolderNativeBody8h*, std::uint32_t);
};

// 440100：原IStringScanner/CStringToFolder仅有vptr与CString，不拥有路径来源或服务。
// 原生构造/释放必须显式调用；C++外壳不提前GetNil，也不补做第二次析构。
struct RuntimeCStringFolderNativeBody8h final
{
    RuntimeCStringFolderNativeBody8h() noexcept {}
    ~RuntimeCStringFolderNativeBody8h() noexcept {}
    RuntimeCStringFolderNativeBody8h(const RuntimeCStringFolderNativeBody8h&) = delete;
    RuntimeCStringFolderNativeBody8h& operator=(const RuntimeCStringFolderNativeBody8h&) = delete;

    const RuntimeCStringFolderNativeVtable* vptrAt00;
    union { RuntimeCString textAt04; };
};
static_assert(sizeof(RuntimeCStringFolderNativeBody8h) == 8);
static_assert(offsetof(RuntimeCStringFolderNativeBody8h, textAt04) == 4);
static_assert(std::is_standard_layout_v<RuntimeCStringFolderNativeBody8h>);
static_assert(sizeof(RuntimeCStringFolderNativeVtable) == 4);

const RuntimeCStringFolderNativeVtable& RuntimeStringScannerNativeMethods() noexcept;
const RuntimeCStringFolderNativeVtable& RuntimeCStringFolderNativeMethods() noexcept;

// rawPath地址在入口已选定；GetNil/Set空串之后才测其当前内容长度。
RuntimeCStringFolderNativeBody8h* ConstructRuntimeCStringFolderNativeAt440100(
    RuntimeCStringFolderNativeBody8h& body, const char* rawPath,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

// 440AEE..440B0B：调用前先撤销scanner展开资格；正常Free可抛，禁止改调noexcept析构。
void ReleaseRuntimeCStringFolderInlineAt440AEE(RuntimeCStringFolderNativeBody8h& body);
// 594CF1 -> 440360：异常展开用；Free再抛终止。两条路径都不清text指针。
void DestroyRuntimeCStringFolderNativeAt440360(RuntimeCStringFolderNativeBody8h& body) noexcept;
RuntimeCStringFolderNativeBody8h* DeleteRuntimeCStringFolderNativeAt440530(
    RuntimeCStringFolderNativeBody8h& body, std::uint32_t flags) noexcept;
RuntimeCStringFolderNativeBody8h* DeleteRuntimeStringScannerNativeAt4122F0(
    RuntimeCStringFolderNativeBody8h& body, std::uint32_t flags) noexcept;
}
