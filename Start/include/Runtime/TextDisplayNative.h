#pragma once

#include <Runtime/GraphicsRuntimeApi.h>
#include <Runtime/DrawLayersView.h>
#include <Runtime/TextDisplaySceneRuntime.h>
#include <Runtime/MessageFormatSelectionRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct TextDisplayNativeBody90h;
// 原5B4F78四槽：Run/Delete/Update/Draw；5B4F88才是下一类型RTTI。
struct TextDisplayNativeVtable final
{
    std::uint8_t (__thiscall* runAt00)(TextDisplayNativeBody90h*,std::int32_t);
    TextDisplayNativeBody90h* (__thiscall* deleteAt04)(TextDisplayNativeBody90h*,std::uint32_t);
    void (__thiscall* updateAt08)(TextDisplayNativeBody90h*);
    void (__thiscall* drawAt0C)(TextDisplayNativeBody90h*);
};
struct TextDisplayNativeBody90h final
{
    const TextDisplayNativeVtable* vptrAt00;
    std::uint32_t phaseAt04;
    std::uint32_t savedModeAt08;
    union { TransitOperation operationAt0C; };
    std::int32_t stepAt3C;
    union { YtlReferenceSlotRuntime baseAt40; };
    union { YtlReferenceSlotRuntime compositeAt4C; };
    std::uint8_t skipAt58;
    std::uint8_t unwrittenAt59[3];
    union { YtlReferenceSlotRuntime maskAt5C; };
    std::uint8_t unwrittenAt68[4];
    std::uint8_t drawAt6C;
    std::uint8_t overlayAt6D;
    std::uint8_t unwrittenAt6E[2];
    std::uint32_t waitAt70;
    union { YtlReferenceSlotRuntime textAt74; };
    RECT dirtyAt80;
    TextDisplayNativeBody90h() noexcept {}
};
static_assert(sizeof(TextDisplayNativeVtable)==0x10);
static_assert(offsetof(TextDisplayNativeVtable,updateAt08)==8);
static_assert(offsetof(TextDisplayNativeVtable,drawAt0C)==0x0C);
static_assert(sizeof(TextDisplayNativeBody90h)==0x90);
static_assert(std::is_standard_layout_v<TextDisplayNativeBody90h>);
static_assert(std::is_trivially_destructible_v<TextDisplayNativeBody90h>);
static_assert(offsetof(TextDisplayNativeBody90h,operationAt0C)==0x0C);
static_assert(offsetof(TextDisplayNativeBody90h,baseAt40)==0x40);
static_assert(offsetof(TextDisplayNativeBody90h,maskAt5C)==0x5C);
static_assert(offsetof(TextDisplayNativeBody90h,unwrittenAt68)==0x68);
static_assert(offsetof(TextDisplayNativeBody90h,textAt74)==0x74);
static_assert(offsetof(TextDisplayNativeBody90h,dirtyAt80)==0x80);

// 来源只借本次现场，不持有Root/Message/输入/Save快照。
class ITextDisplayNativeSource
{
public:
    virtual ~ITextDisplayNativeSource()=default;
    virtual std::int32_t ReadNamedAt4C73A0(const char*)=0;
    virtual bool CreateFontAt4A2113(std::int32_t)=0;
    virtual TextDisplayTextInitializationResult InitializeMessageAt4A2239(std::int32_t,
        MessageFormatSelectionRawTarget)=0;
    virtual bool LoadMaskAt4A26E4(YtlReferenceSlotRuntime&,std::int32_t)=0;
    virtual std::uint32_t ReadModeAt04()=0;
    virtual void WriteModeAt04(std::uint32_t)=0;
    virtual bool SuppressAt4C3430()=0;
    virtual bool AccelerateAt4AA970()=0;
    virtual bool ReadFlagAt14Bit1()=0;
    virtual bool ReadFlagAt18Bit1()=0;
    virtual std::uint32_t ReadPaceAt3C()=0;
    virtual std::int32_t ReadStepAt497900()=0;
    virtual bool ReadAdvanceAt4B0550()=0;
    virtual bool ReadNegativeWheelAt4AD740()=0;
    virtual bool ReadHeldSixAt4198D0()=0;
    virtual bool SuppressFrameAt4C3350()=0;
    virtual std::int32_t CaptureDirtyCountAt4B5CF9()=0;
    virtual const RECT* CaptureDirtyDataAt4B5D5C()=0;
    virtual void AdvanceSceneAt4C1760()=0;
};
struct TextDisplayNativeServices final
{
    ITextDisplayNativeSource& source;
    IDrawLayersViewSource& layers;
    IGraphicsRuntimeCalls& graphics;
    IRuntimeStringManagerSource& strings;
};
class TextDisplayNativeScope final
{
public:
    explicit TextDisplayNativeScope(TextDisplayNativeServices&) noexcept;
    ~TextDisplayNativeScope() noexcept;
    TextDisplayNativeScope(const TextDisplayNativeScope&)=delete;
    TextDisplayNativeScope& operator=(const TextDisplayNativeScope&)=delete;
private:
    TextDisplayNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
};
const TextDisplayNativeVtable& TextDisplayNativeMethods() noexcept;
TextDisplayNativeBody90h* ConstructTextDisplayNativeAt4A1F30(TextDisplayNativeBody90h&,TextDisplayNativeServices&);
TextDisplayNativeBody90h* NewTextDisplayNativeAt4AB7EE(TextDisplayNativeServices&,
    IEnvironmentRootMemory& =DefaultEnvironmentRootMemory());
void DestroyTextDisplayNativeAt4A4A80(TextDisplayNativeBody90h&) noexcept;
TextDisplayNativeBody90h* DeleteTextDisplayNativeAt4A71B0(TextDisplayNativeBody90h&,std::uint32_t) noexcept;
void UpdateTextDisplayNativeAt4BA940(TextDisplayNativeBody90h&,TextDisplayNativeServices&);
void DrawTextDisplayNativeAt4B5BF0(TextDisplayNativeBody90h&,TextDisplayNativeServices&);
std::uint8_t RunTextDisplayNativeAt4C3630(TextDisplayNativeBody90h&,std::int32_t,TextDisplayNativeServices&);
}
