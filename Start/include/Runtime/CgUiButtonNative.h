#pragma once
#include <Runtime/GuiControlResourceNative.h>
#include <Runtime/InputRootNative.h>
#include <Runtime/GraphicsSurfaceRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct CgUiButtonNativeBody6Ch;
struct CgUiButtonNativeVtable5 final
{
    CgUiButtonNativeBody6Ch* (__thiscall* deleteAt00)(CgUiButtonNativeBody6Ch*,std::uint32_t);
    std::uint8_t (__thiscall* timelineAt04)(CgUiButtonNativeBody6Ch*);
    POINT* (__thiscall* pointerAt08)(CgUiButtonNativeBody6Ch*,POINT*);
    std::uint8_t (__thiscall* pressAt0C)(CgUiButtonNativeBody6Ch*);
    std::uint8_t (__thiscall* releaseAt10)(CgUiButtonNativeBody6Ch*);
};
struct CgUiButtonNativeBody6Ch final
{
    const CgUiButtonNativeVtable5* vptrAt00;
    std::int32_t stateAt04;
    std::uint8_t pixelHitAt08;
    std::uint8_t unwrittenAt09[3];
    RECT hitRectAt0C;
    std::int32_t inputModeAt1C;
    std::uint8_t latchAt20;
    std::uint8_t unwrittenAt21[3];
    POINT positionAt24;
    std::uint8_t useNaturalAt2C;
    std::uint8_t unwrittenAt2D[3];
    POINT naturalAt30;
    SIZE extentAt38;
    std::int32_t slotsAt40[7];
    union{GuiControlSurfaceReferences surfacesAt5C;};
    std::int32_t timelineAt68;
    CgUiButtonNativeBody6Ch() noexcept{}
    ~CgUiButtonNativeBody6Ch() noexcept{}
};
static_assert(sizeof(CgUiButtonNativeBody6Ch)==0x6C);
static_assert(sizeof(CgUiButtonNativeVtable5)==0x14);
static_assert(offsetof(CgUiButtonNativeBody6Ch,surfacesAt5C)==0x5C);
static_assert(offsetof(CgUiButtonNativeBody6Ch,timelineAt68)==0x68);

// 只借现场输入/图形/字符串来源，正文不保存服务或第二份状态。
class ICgUiButtonInputNativeSource
{
public:
    virtual ~ICgUiButtonInputNativeSource()=default;
    virtual InputRootNativeBody24h* CaptureAt418210()=0;
};
struct CgUiButtonNativeServices final
{
    ICgUiButtonInputNativeSource& input;
    IGraphicsSurfacePixelApi& pixels;
    IGraphicsWindowSurfaceApi& graphics;
    IRuntimeStringManagerSource& strings;
};
class CgUiButtonNativeScope final
{
public:
    explicit CgUiButtonNativeScope(CgUiButtonNativeServices&) noexcept;
    ~CgUiButtonNativeScope() noexcept;
    CgUiButtonNativeScope(const CgUiButtonNativeScope&)=delete;
    CgUiButtonNativeScope& operator=(const CgUiButtonNativeScope&)=delete;
private:
    CgUiButtonNativeServices* previous_;
};
const CgUiButtonNativeVtable5& CgUiButtonNativeVtable() noexcept;
const CgUiButtonNativeVtable5& GuiButtonGraphicsBaseNativeVtable() noexcept;
CgUiButtonNativeBody6Ch* ConstructCgUiButtonNativeAt459530(CgUiButtonNativeBody6Ch&);
void DestroyCgUiButtonNativeAt463160(CgUiButtonNativeBody6Ch&) noexcept;
CgUiButtonNativeBody6Ch* DeleteCgUiButtonNativeAt4669E0(CgUiButtonNativeBody6Ch*,std::uint32_t) noexcept;
GraphicsSurfaceNativeBody* ResolveCgUiButtonSurfaceNativeAt4ADED0(CgUiButtonNativeBody6Ch&,std::int32_t) noexcept;
std::int32_t ProcessCgUiButtonNativeAt4A98E0(CgUiButtonNativeBody6Ch&,std::int32_t,CgUiButtonNativeServices&);
std::uint8_t DrawCgUiButtonNativeAt4B5290(CgUiButtonNativeBody6Ch&,GraphicsSurfaceNativeBody*,const RECT*,CgUiButtonNativeServices&);
std::uint8_t AttachCgUiButtonSurfaceNativeAt4C1F20(CgUiButtonNativeBody6Ch&,std::uint32_t,const char*,const YtlReferenceSlotRuntime&,CgUiButtonNativeServices&);
}
