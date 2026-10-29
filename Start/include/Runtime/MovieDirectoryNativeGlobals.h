#pragma once
#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
using MovieDirectoryNativeExit=void(__cdecl*)();
using MovieDirectoryNativeRegistrar=int(__cdecl*)(MovieDirectoryNativeExit);
RuntimeCString& ProcessMovieDirectoryNative()noexcept;
void InitializeMovieDirectoryNativeAt404A70(RuntimeCString&,MovieDirectoryNativeExit,
    MovieDirectoryNativeRegistrar,IRuntimeStringManagerSource& =DefaultRuntimeStringManagerSource());
}
