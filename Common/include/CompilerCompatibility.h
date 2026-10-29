#pragma once

#if defined(__clang__) && defined(_MSC_VER) && defined(__cplusplus)
// MSVC前端会注入这些CRT名字；Clang需要声明，仍链接原CRT，不替换失败行为。
extern "C" int __cdecl _purecall(void);
typedef const struct _s_ThrowInfo _ThrowInfo;
#endif
