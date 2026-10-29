#pragma once
#include <Runtime/ArchiveReaderNative.h>
#include <Runtime/ScriptNativeBody.h>
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct ScriptParamsNativeBody;
struct ScriptReaderFactoryNativeBody14h;
struct ScriptReaderFactoryNativeVtable1 final
{
    ScriptReaderFactoryNativeBody14h* (__thiscall* deleteAt00)(ScriptReaderFactoryNativeBody14h*,std::uint32_t);
};
struct ScriptReaderFactoryNativeBody14h final
{
    const ScriptReaderFactoryNativeVtable1* vptrAt00;
    union{YtlReferenceSlotRuntime readerAt04;};
    std::int32_t fieldAt10;
    ScriptReaderFactoryNativeBody14h() noexcept{}
};
static_assert(sizeof(ScriptReaderFactoryNativeBody14h)==0x14);
static_assert(offsetof(ScriptReaderFactoryNativeBody14h,readerAt04)==4);
class IScriptLoadNativeSource
{
public:
    virtual ~IScriptLoadNativeSource()=default;
    // 4D3D0F直接418480/Pattern/4AD1B0；不预先选运行根或复制名字。
    virtual const char* FindNameAt4AD1B0(std::int32_t)=0;
    virtual void LoadAt4D44C0(ScriptReaderFactoryNativeBody14h&,const char*)=0;
    virtual ScriptParamsNativeBody* CaptureParamsAfter413BE0()=0;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0()=0;
    virtual int MessageBoxAt4D3EE6(HWND,const char*,const char*,UINT)=0;
};
struct ScriptLoadNativeServices final
{
    IScriptLoadNativeSource& source;
    IRuntimeStringManagerSource& strings;
    const IScriptHeaderSource& headers;
};
const ScriptReaderFactoryNativeVtable1& ScriptReaderFactoryNativeMethods() noexcept;
void DestroyScriptReaderFactoryNativeAt4C9D20(ScriptReaderFactoryNativeBody14h&) noexcept;
std::uint8_t LoadScriptNativeAt4D3C90(ScriptNativeBody&,std::int32_t,ScriptLoadNativeServices&);
}
