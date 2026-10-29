#pragma once

#include <Runtime/FileInfoProgressNative.h>
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct FileInfoProgressDialogNativeControlVtable2 final
{
    void(__thiscall* destroyAt00)(FileInfoProgressDialogNativeControl18h*);
    FileInfoProgressDialogNativeControl18h*(__thiscall* deleteAt04)(FileInfoProgressDialogNativeControl18h*, DWORD);
};
struct FileInfoProgressDialogNativeControl18h final
{
    const FileInfoProgressDialogNativeControlVtable2* vptrAt00;
    DWORD referencesAt04;
    BYTE owningAt08;
    BYTE unwrittenAt09[3];
    DWORD strideAt0C;
    DWORD countAt10;
    FileInfoProgressDialogNativeBody24h* payloadAt14;
    FileInfoProgressDialogNativeControl18h() noexcept {}
};
static_assert(sizeof(FileInfoProgressDialogNativeControlVtable2) == 8);
static_assert(sizeof(FileInfoProgressDialogNativeControl18h) == 0x18);
static_assert(offsetof(FileInfoProgressDialogNativeControl18h, payloadAt14) == 0x14);

namespace FileInfoProgressNativeDetail
{
template<class T> T Read(const T& value) noexcept { return *static_cast<const volatile T*>(&value); }
const FileInfoProgressNativeVtable5& BaseVtable() noexcept;
const FileInfoProgressDialogNativeControlVtable2& DialogControlVtable() noexcept;
FileInfoProgressDialogNativeBody24h* Resolve(const FileInfoProgressDialogNativeReference0Ch&) noexcept;
void Release(FileInfoProgressDialogNativeReference0Ch&);
}
}
