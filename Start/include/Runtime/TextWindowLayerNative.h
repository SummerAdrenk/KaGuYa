#pragma once

#include <Runtime/GraphicsSurfaceNativeBody.h>
#include <Runtime/MessageWindowLayerNative.h>

namespace KaGuYa::Start::Runtime
{
struct ParamsGameSystem;
class IGraphicsWindowSurfaceApi;

// 45DB30/464510：只有原E8h正文；Ref和数组没有自动拥有析构，不提前清未写字段。
struct TextWindowLayerNativeBodyE8h final
{
    LayerItemNativeBody14h layer;
    RECT boundsAt14;
    union { YtlReferenceSlotRuntime canvasAt24; };
    RECT contentAt30;
    RECT nameBoundsAt40;
    std::int32_t glyphCursorAt50;
    std::uint8_t playbackStartedAt54;
    std::uint8_t unwrittenAt55[3];
    std::int32_t waitAt58;
    std::int32_t autoWaitAt5C;
    std::uint8_t skipAt60;
    std::uint8_t unwrittenAt61[3];
    union { DrawLayerReferenceArray referencesAt64; };
    union { YtlReferenceSlotRuntime nameAt70; };
    union { YtlReferenceSlotRuntime voicesAt7C; };
    union { YtlReferenceSlotRuntime bodyAt88; };
    std::int32_t formatIndexAt94;
    std::uint8_t shadowAt98;
    std::uint8_t renderAt99;
    std::uint8_t unreadAt9A;
    std::uint8_t unwrittenAt9B;
    union { YtlReferenceSlotRuntime normalBodyColorAt9C; };
    union { YtlReferenceSlotRuntime readBodyColorAtA8; };
    union { YtlReferenceSlotRuntime nameColorAtB4; };
    union { YtlReferenceSlotRuntime nameShadowColorAtC0; };
    std::uint32_t unwrittenAtCC;
    std::uint32_t unknownAtD0;
    std::uint32_t unknownAtD4;
    std::int32_t bodyOffsetXAtD8;
    std::int32_t bodyOffsetYAtDC;
    std::int32_t nameOffsetXAtE0;
    std::int32_t nameOffsetYAtE4;
    TextWindowLayerNativeBodyE8h() noexcept {}
    TextWindowLayerNativeBodyE8h(const TextWindowLayerNativeBodyE8h&) = delete;
    TextWindowLayerNativeBodyE8h& operator=(const TextWindowLayerNativeBodyE8h&) = delete;
};
static_assert(sizeof(TextWindowLayerNativeBodyE8h) == 0xE8);
static_assert(alignof(TextWindowLayerNativeBodyE8h) == 4);
static_assert(std::is_standard_layout_v<TextWindowLayerNativeBodyE8h>);
static_assert(std::is_trivially_destructible_v<TextWindowLayerNativeBodyE8h>);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, boundsAt14) == 0x14);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, canvasAt24) == 0x24);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, contentAt30) == 0x30);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, nameBoundsAt40) == 0x40);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, glyphCursorAt50) == 0x50);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, playbackStartedAt54) == 0x54);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, waitAt58) == 0x58);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, autoWaitAt5C) == 0x5C);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, skipAt60) == 0x60);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, referencesAt64) == 0x64);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, nameAt70) == 0x70);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, voicesAt7C) == 0x7C);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, bodyAt88) == 0x88);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, formatIndexAt94) == 0x94);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, shadowAt98) == 0x98);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, renderAt99) == 0x99);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, unreadAt9A) == 0x9A);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, unwrittenAt9B) == 0x9B);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, normalBodyColorAt9C) == 0x9C);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, readBodyColorAtA8) == 0xA8);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, nameColorAtB4) == 0xB4);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, nameShadowColorAtC0) == 0xC0);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, unwrittenAtCC) == 0xCC);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, unknownAtD0) == 0xD0);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, unknownAtD4) == 0xD4);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, bodyOffsetXAtD8) == 0xD8);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, bodyOffsetYAtDC) == 0xDC);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, nameOffsetXAtE0) == 0xE0);
static_assert(offsetof(TextWindowLayerNativeBodyE8h, nameOffsetYAtE4) == 0xE4);

// PE 5B2C90：前三槽ILayerItem，四个frame槽，最后46DC10；不是七槽表。
struct TextWindowLayerNativeVtable final
{
    LayerItemNativeFrameVtable frame;
    void (__thiscall* clearAt1C)(LayerItemNativeBody14h*);
    const LayerItemNativeVtable* AsBase() const noexcept { return frame.AsBase(); }
};
static_assert(sizeof(TextWindowLayerNativeVtable) == 0x20);
static_assert(offsetof(TextWindowLayerNativeVtable, clearAt1C) == 0x1C);

// 必填的原生取源边界，没有shared_ptr/颜色快照/默认无颜色实现。
// 每轮固定GameSystem后再捕获reader；477E90使用该旧GameSystem当前+7C Ref，
// 返回真实临时tag。4797C0沿用同一reader；E8构造负责tag正常/EH释放和469EC0。
class ITextWindowNativeColorSource
{
public:
    virtual ~ITextWindowNativeColorSource() = default;
    virtual ParamsGameSystem* CaptureGameSystemAt418480() = 0;
    virtual RuntimeCString* CaptureReaderAt47BCF0() = 0;
    virtual void ReadChildAt477E90(RuntimeCString* reader, YtlReferenceSlotRuntime& out,
        const YtlReferenceSlotRuntime& parent, const char* key) = 0;
    virtual std::uint32_t ReadRgbAt4797C0(RuntimeCString* reader,
        const YtlReferenceSlotRuntime& tag, const char* key) = 0;
};

struct TextWindowLayerNativeServices final
{
    IGraphicsSurfaceApi& surfaces;
    IGraphicsWindowSurfaceApi& drawing;
    EnvironmentRootOwner& environment;
    IMessageWindowNativeStateSource& state;
    ITextWindowNativeColorSource& colors;
    IRuntimeStringManagerSource& strings;
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory();
};
// 借用仅存在于调用栈；非空Surface销毁及动态八槽需覆盖此scope，支持嵌套恢复。
class TextWindowLayerNativeScope final
{
public:
    explicit TextWindowLayerNativeScope(TextWindowLayerNativeServices&) noexcept;
    ~TextWindowLayerNativeScope() noexcept;
    TextWindowLayerNativeScope(const TextWindowLayerNativeScope&) = delete;
    TextWindowLayerNativeScope& operator=(const TextWindowLayerNativeScope&) = delete;
private:
    TextWindowLayerNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
};

const TextWindowLayerNativeVtable& TextWindowLayerNativeMethods() noexcept;
TextWindowLayerNativeBodyE8h* ConstructTextWindowLayerNativeAt45DB30(
    TextWindowLayerNativeBodyE8h&, LayerListNativeBody18h*, TextWindowLayerNativeServices&);
void DestroyTextWindowLayerNativeAt464510(TextWindowLayerNativeBodyE8h&) noexcept;
TextWindowLayerNativeBodyE8h* DeleteTextWindowLayerNativeAt469590(
    TextWindowLayerNativeBodyE8h&, std::uint32_t flags) noexcept;
void DrawTextWindowLayerNativeAt482D30(TextWindowLayerNativeBodyE8h&,
    const RECT*, TextWindowLayerNativeServices&);
void CompositeTextWindowLayerNativeAt481CD0(TextWindowLayerNativeBodyE8h&,
    GraphicsSurfaceNativeBody*, const RECT*, TextWindowLayerNativeServices&);
std::uint8_t RequestTextWindowLayerNativeAt487310(TextWindowLayerNativeBodyE8h&,
    std::uint8_t requested, std::int32_t unused, TextWindowLayerNativeServices&);
void ClearTextWindowLayerNativeAt46DC10(TextWindowLayerNativeBodyE8h&,
    TextWindowLayerNativeServices&);
void ResetTextWindowLayerNativeAt47EE80(TextWindowLayerNativeBodyE8h&,
    TextWindowLayerNativeServices&);
void ResetTextWindowNativeAt46D170(TextWindowLayerNativeBodyE8h&,
    TextWindowLayerNativeServices&);
void ClearTextWindowPreviousRegionNativeAt418EE0(TextWindowLayerNativeBodyE8h&,
    TextWindowLayerNativeServices&);
// 只对应4C291B起的旧区Clear/dirty/两RECT清零；不冒充整个4C2740格式/文字主路径。
void ClearTextWindowFormatRegionNativeAt4C291B(TextWindowLayerNativeBodyE8h&,
    TextWindowLayerNativeServices&);
// 48A873：raw E8申请在捕获manager前，构造失败在展开中删raw；完成后append失败不补删。
TextWindowLayerNativeBodyE8h* CreateAndAppendTextWindowLayerNativeAt48A873(
    IDrawManagerNativeRootSource&, TextWindowLayerNativeServices&);
}
