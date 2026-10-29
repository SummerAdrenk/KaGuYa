#pragma once
#include <Runtime/GuiDialogNative.h>
#include <Runtime/GuiClickedEventNative.h>
#include <Runtime/GuiControlResourceNative.h>
#include <Runtime/TransitRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct SaveLoadMessageBoxNativeServices;
struct SaveLoadConfirmDialogNativeServices;
// 原4EB950/4F9030：正文只持原Ref/向量/标量。服务和业务替身不得进入184h。
struct SaveLoadDialogNativeBody184h final
{
    union { GuiDialogNativeBody5Ch common; };
    union { GuiControlChildrenStorage dialogsAt5C; };
    std::uint32_t resultAt68;
    std::int32_t phaseAt6C,transitionPhaseAt70,nextPhaseAt74;
    RECT boundsAt78;
    union { YtlReferenceSlotRuntime capturedAt88; };
    union { YtlReferenceSlotRuntime canvasAt94; };
    union { YtlReferenceSlotRuntime blackAtA0; };
    union { TransitOperation operationAtAC; };
    std::int32_t stepAtDC;
    std::byte unwrittenAtE0[4];
    std::int32_t pageModeAtE4;
    std::byte unwrittenAtE8[4];
    union { YtlReferenceSlotRuntime eventsAtEC; };
    union { GuiControlSurfaceReferences pageDigitsAtF8; };
    RECT pageBoundsAt104;
    union { EnvironmentVectorStorage<POINT> pagePositionsAt114; };
    union { GuiControlSurfaceReferences slotDigitsAt120; };
    union { GuiControlSurfaceReferences dateDigitsAt12C; };
    union { GuiControlSurfaceReferences noDataAt138; };
    union { GuiControlSurfaceReferences noPreviewAt144; };
    union { GuiControlSurfaceReferences newMarkerAt150; };
    std::byte unwrittenAt15C[4];
    std::int32_t sourceIndexAt160,slotIndexAt164;
    union { YtlReferenceSlotRuntime deleteConfirmationAt168; };
    union { YtlReferenceSlotRuntime slotConfirmationAt174; };
    std::byte unwrittenAt180[4];
    SaveLoadDialogNativeBody184h() noexcept {}
};
static_assert(sizeof(SaveLoadDialogNativeBody184h)==0x184);
static_assert(std::is_standard_layout_v<SaveLoadDialogNativeBody184h>);
static_assert(std::is_trivially_destructible_v<SaveLoadDialogNativeBody184h>);
#define SAVELOAD_OFFSET(field,value) static_assert(offsetof(SaveLoadDialogNativeBody184h,field)==value)
SAVELOAD_OFFSET(dialogsAt5C,0x5C);SAVELOAD_OFFSET(resultAt68,0x68);SAVELOAD_OFFSET(boundsAt78,0x78);
SAVELOAD_OFFSET(capturedAt88,0x88);SAVELOAD_OFFSET(canvasAt94,0x94);SAVELOAD_OFFSET(blackAtA0,0xA0);
SAVELOAD_OFFSET(operationAtAC,0xAC);SAVELOAD_OFFSET(stepAtDC,0xDC);SAVELOAD_OFFSET(pageModeAtE4,0xE4);
SAVELOAD_OFFSET(eventsAtEC,0xEC);SAVELOAD_OFFSET(pageDigitsAtF8,0xF8);SAVELOAD_OFFSET(pageBoundsAt104,0x104);
SAVELOAD_OFFSET(pagePositionsAt114,0x114);SAVELOAD_OFFSET(slotDigitsAt120,0x120);SAVELOAD_OFFSET(dateDigitsAt12C,0x12C);
SAVELOAD_OFFSET(noDataAt138,0x138);SAVELOAD_OFFSET(noPreviewAt144,0x144);SAVELOAD_OFFSET(newMarkerAt150,0x150);
SAVELOAD_OFFSET(deleteConfirmationAt168,0x168);SAVELOAD_OFFSET(slotConfirmationAt174,0x174);SAVELOAD_OFFSET(unwrittenAt180,0x180);
SAVELOAD_OFFSET(sourceIndexAt160,0x160);SAVELOAD_OFFSET(slotIndexAt164,0x164);
#undef SAVELOAD_OFFSET
// 原5BE3C8..5BE458共37槽，+94为RTTI，不是追加业务槽。
struct SaveLoadDialogNativeVtable37 final
{
    GuiDialogNativeVtable35 common;
    std::int32_t (__thiscall* appendDialogAt8C)(GuiDialogNativeBody5Ch*,YtlReferenceSlotRuntime);
    YtlReferenceSlotRuntime* (__thiscall* findDialogAt90)(GuiDialogNativeBody5Ch*,YtlReferenceSlotRuntime*,std::int32_t);
};
static_assert(sizeof(SaveLoadDialogNativeVtable37)==0x94);
class ISaveLoadDialogNativeConstructionSource
{
public:
    virtual ~ISaveLoadDialogNativeConstructionSource()=default;
    virtual std::int32_t ReadInitialPageFromEnvironmentAt4EBC35()=0;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0()=0;
    virtual void* CaptureDrawAfter413BE0()=0;
    virtual bool LoadIntoAt4800A0(void*,GraphicsSurfaceNativeBody*,const char*)=0;
    virtual std::uint8_t LoadSurfaceAt480000(void*,YtlReferenceSlotRuntime*,const char*)=0;
    // 分立原生子类的必填构造边界，不能返回modern shared_ptr或把完整父构造委托出去。
    virtual GuiDialogNativeBody5Ch* ConstructSlotConfirmationAt4EACA0(void*)=0;
    virtual GuiDialogNativeBody5Ch* ConstructDeleteConfirmationAt4EC8A0(void*)=0;
    // 非原业务元数据，仅声明同一套独立确认服务，不能保存在184h/18h控制块内。
    virtual SaveLoadMessageBoxNativeServices* NativeDeleteConfirmationServices() const noexcept{return nullptr;}
    virtual SaveLoadConfirmDialogNativeServices* NativeSlotConfirmationServices() const noexcept{return nullptr;}
};
// 当前恢复构造/拥有核；完整37槽业务尚未完成，dispatch明确由已装配宿主提供。
// 不发布填空stub的全局Save表，不因依赖未实现返回伪成功，也暂不接默认24h场景。
struct SaveLoadDialogNativeConstructionServices final
{
    ISaveLoadDialogNativeConstructionSource& source;
    IGraphicsSurfaceApi& surfaces;
    const SaveLoadDialogNativeVtable37& dispatch;
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
};
using SaveLoadPagePositionsNative=EnvironmentVectorStorage<POINT>;
POINT* GrowSaveLoadPagePositionsNativeAt4D8D30(SaveLoadPagePositionsNative&,POINT*,const POINT&);
void DestroySaveLoadPagePositionsNativeAt4F7130(SaveLoadPagePositionsNative&) noexcept;
SaveLoadDialogNativeBody184h* ConstructSaveLoadDialogNativeAt4EB950(void*,std::uint32_t modeArgument,
    std::uint32_t unusedRootByte,SaveLoadDialogNativeConstructionServices&);
void DestroySaveLoadDialogNativeAt4F9030(SaveLoadDialogNativeBody184h*,SaveLoadDialogNativeConstructionServices&) noexcept;
SaveLoadDialogNativeBody184h* DeleteSaveLoadDialogNativeAt4FDD70(SaveLoadDialogNativeBody184h*,std::uint32_t,
    SaveLoadDialogNativeConstructionServices&) noexcept;
class ISaveLoadDialogNativeTransitionSource
{
public:
    virtual ~ISaveLoadDialogNativeTransitionSource()=default;
    virtual std::uint8_t AccelerateAt4C3430()=0;
    virtual std::uint8_t AccelerateAt4AA970()=0;
    virtual TransitRuntime* CaptureTransitAfter413BE0()=0;
    // 51CC28先捕获Save+70，51CC3F再捕获slot0，最后497900；不能反序或缓存旧归档。
    virtual std::int32_t ReadStepAt51CC28()=0;
    virtual void CommitScreenAt4B02E0()=0;
};
void AdvanceSaveLoadDialogNativeAt51CA80(SaveLoadDialogNativeBody184h&,ISaveLoadDialogNativeTransitionSource&,
    IGraphicsTransitApi&);
}
