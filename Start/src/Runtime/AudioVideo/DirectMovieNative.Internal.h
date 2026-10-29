#pragma once
#include <Runtime/DirectMovieNative.h>

namespace KaGuYa::Start::Runtime::DirectMovieNativeDetail
{
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
template<class Result,class... Args>
Result Com(void* object,std::size_t offset,Args... args)
{
    using Method=Result(__stdcall*)(void*,Args...);
    const auto table=Read(*static_cast<void***>(object));
    const auto method=reinterpret_cast<Method>(Read(table[offset/4]));
    return method(object,args...);
}
using Body=DirectMovieNativeBody38h;
[[noreturn]] void Error(DirectMovieNativeServices&,int,const char*,std::uint32_t);
}
