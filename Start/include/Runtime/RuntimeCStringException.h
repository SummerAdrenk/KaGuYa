#pragma once

#include <Runtime/RuntimeCString.h>
#include <atlstr.h>

namespace KaGuYa::Start::Runtime
{
// 原 PE 5F0F1C 的完整异常类型；不是 RuntimeCString 或现代包装异常。
using RuntimeCStringExceptionType =
    ATL::CStringT<char, ATL::StrTraitATL<char, ATL::ChTraitsCRT<char>>>;
static_assert(sizeof(RuntimeCStringExceptionType) == 4);

// 仅支持已验证的 MSVC x86 CT2 ABI。原位拼接后交付 EH，无本地 CString 析构。
// 按值 catch 的局部对象仍由调用方编译器析构，其 ATL noexcept 边界不由本叶改变。
[[noreturn]] void ThrowRuntimeCStringConcatAt466760(const RuntimeCString& prefix,
    const char* suffix,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
