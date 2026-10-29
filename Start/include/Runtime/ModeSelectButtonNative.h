#pragma once
#include <Runtime/CgUiButtonNative.h>
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct ModeSelectButtonNativeBody10h;
struct ModeSelectButtonNativeVtable2 final
{
    ModeSelectButtonNativeBody10h* (__thiscall* deleteAt00)(ModeSelectButtonNativeBody10h*,std::uint32_t);
    void (__thiscall* resetAt04)(ModeSelectButtonNativeBody10h*);
};
struct ModeSelectButtonNativeBody10h final
{
    const ModeSelectButtonNativeVtable2* vptrAt00;
    union{YtlReferenceSlotRuntime buttonsAt04;};
    ModeSelectButtonNativeBody10h() noexcept{}
    ~ModeSelectButtonNativeBody10h() noexcept{}
};
struct CgUiButtonRecordNative28h final
{
    const char* namesAt00[7];
    POINT positionAt1C;
    std::int32_t timelineAt24;
};
static_assert(sizeof(ModeSelectButtonNativeBody10h)==0x10);
static_assert(sizeof(ModeSelectButtonNativeVtable2)==8);
static_assert(sizeof(CgUiButtonRecordNative28h)==40);
class IModeSelectButtonNativeSource
{
public:
    virtual ~IModeSelectButtonNativeSource()=default;
    virtual void* CaptureDrawAt45AD92()=0;
    // 原601658 + (index + 4*mode)*40；不在正文加0..2模式限制。
    virtual const CgUiButtonRecordNative28h* RecordAt601658(std::uint32_t offset)=0;
    virtual std::uint8_t LoadAt480930(void* receiver,CgUiButtonNativeBody6Ch*,
        const CgUiButtonRecordNative28h*,std::int32_t setPosition)=0;
};
struct ModeSelectButtonNativeServices final
{
    IEnvironmentRootMemory& memory;
    IModeSelectButtonNativeSource& source;
    IGraphicsSurfaceApi& surfaces;
    CgUiButtonNativeServices& button;
};
class ModeSelectButtonNativeScope final
{
public:
    explicit ModeSelectButtonNativeScope(ModeSelectButtonNativeServices&) noexcept;
    ~ModeSelectButtonNativeScope() noexcept;
    ModeSelectButtonNativeScope(const ModeSelectButtonNativeScope&)=delete;
    ModeSelectButtonNativeScope& operator=(const ModeSelectButtonNativeScope&)=delete;
private:
    GraphicsSurfaceNativeScope surface_;
    CgUiButtonNativeScope button_;
};
ModeSelectButtonNativeBody10h* ConstructModeSelectButtonNativeAt45AC40(
    ModeSelectButtonNativeBody10h&,std::int32_t,ModeSelectButtonNativeServices&);
ModeSelectButtonNativeBody10h* NewModeSelectButtonNativeAt4703F9(std::int32_t,ModeSelectButtonNativeServices&);
void ClearModeSelectButtonNativeAt486AD0(ModeSelectButtonNativeBody10h&);
void ResetModeSelectButtonNativeAt487470(ModeSelectButtonNativeBody10h&);
void DestroyOptionButtonBaseNativeAt464920(ModeSelectButtonNativeBody10h&) noexcept;
ModeSelectButtonNativeBody10h* DeleteModeSelectButtonNativeAt468E50(ModeSelectButtonNativeBody10h*,std::uint32_t) noexcept;
const ModeSelectButtonNativeVtable2& ModeSelectButtonNativeVtable() noexcept;
const ModeSelectButtonNativeVtable2& OptionButtonBaseNativeVtable() noexcept;
}
