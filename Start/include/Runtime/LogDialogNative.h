#pragma once

#include <Runtime/LogDialogNativeConstruction.h>
#include <Runtime/GuiDialogNative.h>
#include <Runtime/GuiClickedEventNative.h>
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiSliderNative.h>
#include <Runtime/GraphicsTextNativeApi.h>
#include <Runtime/InputNativeObjects.h>
#include <Runtime/MessageFormatSelectionRuntime.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/ScriptParamsNativeBody.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
struct SoundManagerNativeBody50h;
struct LogTextNativeBody30h;
struct LogTextNativeVtable1 final
{
    LogTextNativeBody30h* (__thiscall* deleteAt00)(LogTextNativeBody30h*,std::uint32_t);
};
struct LogTextNativeBody30h final
{
    const LogTextNativeVtable1* vptrAt00;
    std::int32_t xAt04,yAt08,historyAt0C,formatAt10,eventAt14;
    union { EnvironmentVectorStorage<RuntimeCString> voicesAt18; };
    union { YtlReferenceSlotRuntime surfaceAt24; };
    LogTextNativeBody30h() noexcept {}
};
struct LogTextDialogNativeBodyA0h final
{
    union { GuiDialogNativeBody5Ch common; };
    union { YtlReferenceSlotRuntime eventsAt5C; };
    union { EnvironmentVectorStorage<YtlReferenceSlotRuntime> rowsAt68; };
    std::int32_t positionAt74;
    union { YtlReferenceSlotRuntime voicesAt78; };
    RECT boundsAt84;
    union { YtlReferenceSlotRuntime frameAt94; };
    LogTextDialogNativeBodyA0h() noexcept {}
};
struct LogScrollDialogNativeBody80h final
{
    union { GuiDialogNativeBody5Ch common; };
    RECT boundsAt5C;
    std::int32_t repeatAt6C,heldEventAt70;
    union { YtlReferenceSlotRuntime eventsAt74; };
    LogScrollDialogNativeBody80h() noexcept {}
};
struct LogDialogNativeBodyDCh final
{
    union { GuiDialogNativeBody5Ch common; };
    std::int32_t phaseAt5C,unwrittenAt60,unwrittenAt64;
    RECT boundsAt68;
    union { YtlReferenceSlotRuntime textAt78; };
    union { YtlReferenceSlotRuntime scrollAt84; };
    union { YtlReferenceSlotRuntime backgroundAt90; };
    union { YtlReferenceSlotRuntime reservedAt9C; };
    std::int32_t fieldAtA8,fieldAtAC,fieldAtB0,fieldAtB4,fieldAtB8,fieldAtBC;
    std::int32_t fieldAtC0,fieldAtC4,fieldAtC8,fieldAtCC,fieldAtD0,fieldAtD4,unwrittenAtD8;
    LogDialogNativeBodyDCh() noexcept {}
};
struct LogDialogNativeVtable36 final
{
    GuiDialogNativeVtable35 common;
    RECT* (__thiscall* boundsAt8C)(GuiDialogNativeBody5Ch*,RECT*);
};
static_assert(sizeof(LogTextNativeBody30h)==0x30 && sizeof(LogTextDialogNativeBodyA0h)==0xA0);
static_assert(sizeof(LogScrollDialogNativeBody80h)==0x80 && sizeof(LogDialogNativeBodyDCh)==0xDC);
static_assert(sizeof(LogTextNativeVtable1)==4 && sizeof(LogDialogNativeVtable36)==0x90);
static_assert(alignof(LogDialogNativeBodyDCh)==4 && std::is_standard_layout_v<LogDialogNativeBodyDCh>);
#define LOG_NATIVE_OFFSET(type,field,offset) static_assert(offsetof(type,field)==offset)
LOG_NATIVE_OFFSET(LogDialogNativeBodyDCh,phaseAt5C,0x5C);
LOG_NATIVE_OFFSET(LogDialogNativeBodyDCh,boundsAt68,0x68);
LOG_NATIVE_OFFSET(LogDialogNativeBodyDCh,textAt78,0x78);
LOG_NATIVE_OFFSET(LogDialogNativeBodyDCh,scrollAt84,0x84);
LOG_NATIVE_OFFSET(LogDialogNativeBodyDCh,backgroundAt90,0x90);
LOG_NATIVE_OFFSET(LogDialogNativeBodyDCh,reservedAt9C,0x9C);
LOG_NATIVE_OFFSET(LogDialogNativeBodyDCh,unwrittenAtD8,0xD8);
LOG_NATIVE_OFFSET(LogTextDialogNativeBodyA0h,eventsAt5C,0x5C);
LOG_NATIVE_OFFSET(LogTextDialogNativeBodyA0h,rowsAt68,0x68);
LOG_NATIVE_OFFSET(LogTextDialogNativeBodyA0h,positionAt74,0x74);
LOG_NATIVE_OFFSET(LogTextDialogNativeBodyA0h,voicesAt78,0x78);
LOG_NATIVE_OFFSET(LogTextDialogNativeBodyA0h,boundsAt84,0x84);
LOG_NATIVE_OFFSET(LogTextDialogNativeBodyA0h,frameAt94,0x94);
LOG_NATIVE_OFFSET(LogScrollDialogNativeBody80h,boundsAt5C,0x5C);
LOG_NATIVE_OFFSET(LogScrollDialogNativeBody80h,repeatAt6C,0x6C);
LOG_NATIVE_OFFSET(LogScrollDialogNativeBody80h,eventsAt74,0x74);
LOG_NATIVE_OFFSET(LogTextNativeBody30h,voicesAt18,0x18);
LOG_NATIVE_OFFSET(LogTextNativeBody30h,surfaceAt24,0x24);
#undef LOG_NATIVE_OFFSET

// 仅原叶及已选正文借用，不提供整段Log业务或现代对话框回调。
class ILogDialogNativeSource
{
public:
    virtual ~ILogDialogNativeSource()=default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t site)=0;
    virtual ParamsRuntimeRoot* CaptureParamsAt418480(std::uint32_t site)=0;
    virtual InputRootNativeBody24h* CaptureInputAt418210(std::uint32_t site)=0;
    virtual void EnsureRootAt413BE0(std::uint32_t site)=0;
    virtual void* RootAt6198C4(std::uint32_t site)=0;
    virtual YtlReferenceSlotRuntime& RootDrawSlotAt18(void* fixedRoot)=0;
    virtual YtlReferenceSlotRuntime& RootSoundSlotAt24(void* fixedRoot)=0;
    virtual YtlReferenceSlotRuntime& RootFileManagerSlotAt00(void* fixedRoot)=0;
    virtual YtlReferenceSlotRuntime& FileManagerMessageSlotAt10(void* fixedFiles)=0;
    virtual ScriptParamsNativeBody* ParamsAt47BDB0(void* fixedRoot)=0;
    virtual const char* FontFaceAt61A684()=0;
    virtual YtlReferenceSlotRuntime* CaptureBackgroundAt4ACDD0(void* fixedDraw,YtlReferenceSlotRuntime* out)=0;
    virtual bool LoadSurfaceAt480000(void* fixedDraw,YtlReferenceSlotRuntime& out,const char* name)=0;
    virtual std::int32_t ReadFormatAt417C40(void* fixedMessage,std::int32_t index,
        MessageFormatSelectionRawTarget output,std::uint32_t variant,bool markRead)=0;
    // 声音叶只消费已经固定的50h，不能重新捕获Root或维护另一套声音状态。
    virtual bool PlayVoicesAt42C4F0(SoundManagerNativeBody50h* fixedSound,const YtlReferenceSlotRuntime* names)=0;
    virtual bool AnyVoiceAt4196C0(SoundManagerNativeBody50h* fixedSound)=0;
    virtual void AdjustEffectsAt4C2410(SoundManagerNativeBody50h* fixedSound)=0;
    virtual void RefreshSoundAt433F60(SoundManagerNativeBody50h* fixedSound)=0;
};
class ILogDialogNativeTextApi
{
public:
    virtual ~ILogDialogNativeTextApi()=default;
    virtual bool CreateFontAt4388B0(const GraphicsTextNativeFontParameters*)=0;
    virtual bool InitializeAt4389D0(const char*,std::int32_t,std::int32_t)=0;
    virtual bool BlitAllAt53A980(GraphicsSurfaceHandle,RECT*)=0;
};
class LogDialogNativeTextApi final:public ILogDialogNativeTextApi
{
public:
    explicit LogDialogNativeTextApi(GraphicsTextNativeApi& api) noexcept:api_(api){}
    bool CreateFontAt4388B0(const GraphicsTextNativeFontParameters* p) override{return api_.CreateTextFontNativeAt4388B0(p);}
    bool InitializeAt4389D0(const char* p,std::int32_t x,std::int32_t y) override{return api_.InitializeText(p,x,y);}
    bool BlitAllAt53A980(GraphicsSurfaceHandle h,RECT* out) override{return api_.BlitAllTextToSurface(h,out);}
private:
    GraphicsTextNativeApi& api_;
};
struct LogDialogNativeServices final
{
    ILogDialogNativeSource& source;
    IGuiControlResourceLoadServices& pictures;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    ILogDialogNativeTextApi& text;
    IGraphicsWindowSurfaceApi& drawing;
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
};
class LogDialogNativeScope final
{
public:
    explicit LogDialogNativeScope(LogDialogNativeServices&) noexcept;
    ~LogDialogNativeScope() noexcept;
    LogDialogNativeScope(const LogDialogNativeScope&)=delete;
    LogDialogNativeScope& operator=(const LogDialogNativeScope&)=delete;
private:
    LogDialogNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    GuiControlGraphicsScope graphics_;
    GuiInputNativeScope input_;
    GuiDialogNativeScope screen_;
};
LogDialogNativeServices& RequireLogDialogNativeServices();
GuiDialogNativeBody5Ch* ConstructLogDialogNativeAt4E23D0(void* rawDCh);
LogTextDialogNativeBodyA0h* ConstructLogTextDialogNativeAt4E1B70(void*,GuiDialogNativeBody5Ch*,std::int32_t);
LogScrollDialogNativeBody80h* ConstructLogScrollDialogNativeAt4F10C0(void*,GuiDialogNativeBody5Ch*,std::int32_t);
void DestroyLogDialogNativeAt4F7EC0(LogDialogNativeBodyDCh*) noexcept;
LogDialogNativeBodyDCh* DeleteLogDialogNativeAt4FCA50(LogDialogNativeBodyDCh*,std::uint32_t) noexcept;
LogTextDialogNativeBodyA0h* DeleteLogTextDialogNativeAt4FC920(LogTextDialogNativeBodyA0h*,std::uint32_t) noexcept;
LogScrollDialogNativeBody80h* DeleteLogScrollDialogNativeAt4FE140(LogScrollDialogNativeBody80h*,std::uint32_t) noexcept;
LogTextNativeBody30h* DeleteLogTextNativeAt4FE870(LogTextNativeBody30h*,std::uint32_t) noexcept;
const GuiDialogNativeVtable35& LogDialogNativeVtable() noexcept;
const LogDialogNativeVtable36& LogTextDialogNativeVtable() noexcept;
const LogDialogNativeVtable36& LogScrollDialogNativeVtable() noexcept;
const LogTextNativeVtable1& LogTextNativeVtable() noexcept;
class LogDialogNativeConstruction final:public ILogDialogNativeConstruction
{
public:
    explicit LogDialogNativeConstruction(LogDialogNativeServices& services) noexcept:services_(services){}
    GuiDialogNativeBody5Ch* ConstructLogDialogAt4E23D0(void* rawDCh) override;
    LogDialogNativeServices* NativeDialogServices() const noexcept override{return &services_;}
private:
    LogDialogNativeServices& services_;
};
void ConstructLogDialogFontFaceAt406620(RuntimeCString&,IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource());
using LogDialogNativeExit=void(__cdecl*)();
class ILogDialogNativeExitRegistrar
{
public:
    virtual ~ILogDialogNativeExitRegistrar()=default;
    virtual int RegisterAtExit(LogDialogNativeExit)=0;
};
void InitializeLogDialogNativeFontAt406620(RuntimeCString&,LogDialogNativeExit,
    ILogDialogNativeExitRegistrar&,IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource());
RuntimeCString& ProcessLogDialogNativeFontFace() noexcept;
std::span<const std::uint32_t> LogDialogOriginalTableSample18(std::uint32_t index);
struct LogDialogNativeEhEvidence final
{
    std::uint32_t functionVA,funcInfoVA;
    std::span<const std::uint32_t,9> funcInfo;
    std::span<const std::uint32_t> unwind;
};
std::span<const LogDialogNativeEhEvidence> LogDialogNativeEhSample18() noexcept;
}
