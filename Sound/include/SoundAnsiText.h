#pragma once

#include <atlstr.h>

#if defined(_CONVERSION_DONT_USE_THREAD_LOCALE) || defined(_ATL_NO_EXCEPTIONS) || \
    defined(_CSTRING_DISABLE_NARROW_WIDE_CONVERSION)
#error SoundAnsiText requires ATL thread-ACP conversion and C++ exceptions.
#endif
#if !defined(_ATL_USE_WINAPI_FAMILY_DESKTOP_APP)
#error SoundAnsiText requires ATL desktop string-resource loading.
#endif

namespace KaGuYa::Sound
{
class SoundAnsiText final
{
public:
    explicit SoundAnsiText(const char* source);

    const wchar_t* GetString() const noexcept;
    int GetLength() const noexcept;
    bool IsEmpty() const noexcept;

private:
    ATL::CAtlStringW text_;
};

// 仅在句柄登记查找成功后构造，持有到子对象Open返回。
class SoundAnsiOpenNames final
{
public:
    SoundAnsiOpenNames(const char* logicalName, const char* physicalName);

    const wchar_t* LogicalName() const noexcept;
    const wchar_t* PhysicalName() const noexcept;

private:
    // 原03 1000337C..10003397：成员声明顺序就是转换顺序。
    SoundAnsiText logicalName_;
    SoundAnsiText physicalName_;
};
}
