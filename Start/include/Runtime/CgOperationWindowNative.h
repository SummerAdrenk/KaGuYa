#pragma once
#include <Runtime/CgOperationDialogNative.h>
#include <Runtime/LayerWindowNativeBase.h>
#include <Runtime/SceneManagerNative.h>
#include <Runtime/InputNativeObjects.h>

namespace KaGuYa::Start::Runtime
{
struct CgOperationWindowNativeBody30h final
{
    LayerWindowNativeBody24h window;
    union{YtlReferenceSlotRuntime dialogAt24;};
    CgOperationWindowNativeBody30h() noexcept{}
    ~CgOperationWindowNativeBody30h() noexcept{}
};
struct CgOperationWindowNativeVtable8 final
{
    LayerItemNativeFrameVtable frame;
    void(__thiscall* invalidateAt1C)(LayerItemNativeBody14h*,RECT);
};
static_assert(sizeof(CgOperationWindowNativeBody30h)==0x30);
static_assert(offsetof(CgOperationWindowNativeBody30h,dialogAt24)==0x24);
static_assert(sizeof(CgOperationWindowNativeVtable8)==0x20);
static_assert(offsetof(CgOperationWindowNativeVtable8,invalidateAt1C)==0x1C);
class ICgOperationWindowNativeSource
{
public:
    virtual ~ICgOperationWindowNativeSource()=default;
    // 原fresh 413BE0/6198C4+30解析；不提前读command或保活某代Manager。
    virtual SceneManagerNativeBody48h* CaptureScenesAt413BE0(std::uint32_t site)=0;
    // 4BD812的601160锁/6198C8惰性构造/normal-EH解锁，来源仍是同一Environment owner。
    virtual EnvironmentRootStorage* CaptureEnvironmentInlineAt6198C8(std::uint32_t site)=0;
};
struct CgOperationWindowNativeServices final
{
    CgOperationDialogNativeServices& dialog;
    ICgOperationWindowNativeSource& source;
    InputRootOwner& input;
    InputNativeServices& inputServices;
};
const CgOperationWindowNativeVtable8& CgOperationWindowNativeMethods() noexcept;
CgOperationWindowNativeBody30h* ConstructCgOperationWindowNativeAt499C10(
    CgOperationWindowNativeBody30h&,LayerListNativeBody18h*,CgOperationWindowNativeServices&);
void DestroyCgOperationWindowNative(CgOperationWindowNativeBody30h&) noexcept;
CgOperationWindowNativeBody30h* DeleteCgOperationWindowNativeAt4A60D0(
    CgOperationWindowNativeBody30h&,std::uint32_t flags) noexcept;
void DrawCgOperationWindowNativeAt4B54F0(CgOperationWindowNativeBody30h&,const RECT*);
void CompositeCgOperationWindowNativeAt4B5240(CgOperationWindowNativeBody30h&,
    GraphicsSurfaceNativeBody*,const RECT*);
void SynchronizeCgOperationWindowDirtyNativeAt4AB980(CgOperationWindowNativeBody30h&);
void SetCgOperationWindowVisibilityNativeAt4C22B0(CgOperationWindowNativeBody30h&,std::uint8_t);
std::int32_t PollCgOperationWindowNativeAt4BD590(CgOperationWindowNativeBody30h&,
    CgOperationWindowNativeServices&);
std::uint8_t HitCgOperationWindowNativeAt4C2690(CgOperationWindowNativeBody30h&,
    CgOperationWindowNativeServices&);
}
