#pragma once

#include <Runtime/EnvironmentRootRuntime.h>

#include <cstddef>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
static_assert(sizeof(EnvironmentVectorStorage<RuntimeCString>) == 12);
static_assert(offsetof(EnvironmentVectorStorage<RuntimeCString>, begin) == 0);
static_assert(offsetof(EnvironmentVectorStorage<RuntimeCString>, end) == 4);
static_assert(offsetof(EnvironmentVectorStorage<RuntimeCString>, capacity) == 8);

// 40B7A0 -> 43A7E0：等长 nil 串构造；原第二栈参数未读取，不赋予业务含义。
// 全部串完成才发布 end；中途失败由普通 43A700 清当前存储，不补串前缀析构。
EnvironmentVectorStorage<RuntimeCString>* ConstructRuntimeCStringVectorAt40B7A0(
    EnvironmentVectorStorage<RuntimeCString>& destination, std::uint32_t count,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 456E80：原生 x86 CString 三指针 vector，不带数组 cookie 或内嵌分配器。
// 初始三指针置空；保留原部分构造失败状态，不额外补回滚。
void ResizeRuntimeCStringVectorAt456E80(
    EnvironmentVectorStorage<RuntimeCString>& storage, std::uint32_t count,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 41D5E9..41D61E / 4085C0：复制追加；允许 source 别名旧元素，不移走源串。
// 快路径缓存写入槽、回调后重读 end；增长路径先构造插入值，再复制当前旧范围。
void AppendRuntimeCStringAt41D5E9(
    EnvironmentVectorStorage<RuntimeCString>& storage, const RuntimeCString& source,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 40B6E0：构造未拥有存储的目的 vector；先清三指针，再按源 size 精确分配并逐串 Clone。
// 分配后重读源范围，end 取实际复制完成指针；不是赋值，不释放目的的旧存储。
void CopyRuntimeCStringVectorAt40B6E0(
    EnvironmentVectorStorage<RuntimeCString>& destination,
    const EnvironmentVectorStorage<RuntimeCString>& source,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 43A700 自身可抛；复制构造 catch 必须用此入口，清理错误可替换原异常。
// begin 为空不清其余槽；正序清理后重读当前存储，成功释放后才清三指针。
void DestroyRuntimeCStringVectorAt43A700(
    EnvironmentVectorStorage<RuntimeCString>& storage,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 40DD20 -> 43A700：正序析构、释放存储后清零；PE EHFlags=5，异常终止。
// begin 为空时不改 end/capacity；不得再对元素执行第二轮 ATL 析构。
void DestroyRuntimeCStringVectorAt40DD20(
    EnvironmentVectorStorage<RuntimeCString>& storage,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;
}
