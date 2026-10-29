#pragma once

#include <Runtime/ModeButtonNative.h>
#include <Runtime/SceneManagerPublicNativeReferences.h>
#include <Runtime/GuiControlResourceLoadRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct CgButtonNativeBody84h;

// 原PE 5B5190..5B51C8：15槽，所有显式参数按栈传递；不扩充50h的13槽表。
struct CgButtonNativeVtable15 final
{
    CgButtonNativeBody84h* (__thiscall* deleteAt00)(CgButtonNativeBody84h*, std::uint32_t);
    std::int32_t (__thiscall* processAt04)(CgButtonNativeBody84h*);
    std::int32_t (__thiscall* invalidateAt08)(CgButtonNativeBody84h*, RECT);
    std::uint8_t (__thiscall* releaseAt0C)(CgButtonNativeBody84h*);
    std::int32_t (__thiscall* inputAt10)(CgButtonNativeBody84h*);
    void (__thiscall* resetAllAt14)(CgButtonNativeBody84h*);
    std::int32_t (__thiscall* resetAt18)(CgButtonNativeBody84h*, std::int32_t);
    std::int32_t (__thiscall* focusAt1C)(CgButtonNativeBody84h*, std::int32_t);
    std::int32_t (__thiscall* pressAt20)(CgButtonNativeBody84h*, std::int32_t);
    std::int32_t (__thiscall* commitAt24)(CgButtonNativeBody84h*, std::int32_t);
    std::int32_t (__thiscall* selectAt28)(CgButtonNativeBody84h*, std::int32_t);
    std::int32_t (__thiscall* currentAt2C)(CgButtonNativeBody84h*);
    std::int32_t (__thiscall* navigateAt30)(CgButtonNativeBody84h*);
    void (__thiscall* drawAt34)(CgButtonNativeBody84h*);
    void (__thiscall* compositeAt38)(CgButtonNativeBody84h*, GraphicsSurfaceNativeBody*, const RECT*);
};
struct CgButtonNativeBody84h final
{
    union { ModeButtonNativeBody50h common; };
    GuiControlSurfaceReferences pagesAt50;
    union { YtlReferenceSlotRuntime pageButtonsAt5C; };
    std::uint32_t unwrittenAt68;
    std::int32_t pageAt6C;
    std::int32_t perPageAt70;
    std::int32_t categoryAt74;
    union { YtlReferenceSlotRuntime selectorAt78; };
    // 499DC0的晚写字段与+68原字节必须保留；不能使用整块{}初始化。
    CgButtonNativeBody84h() noexcept {}
    ~CgButtonNativeBody84h() noexcept {}
    CgButtonNativeBody84h(const CgButtonNativeBody84h&) = delete;
    CgButtonNativeBody84h& operator=(const CgButtonNativeBody84h&) = delete;
};
static_assert(sizeof(CgButtonNativeBody84h) == 0x84);
static_assert(alignof(CgButtonNativeBody84h) == 4);
static_assert(std::is_standard_layout_v<CgButtonNativeBody84h>);
static_assert(offsetof(CgButtonNativeBody84h, common) == 0);
static_assert(offsetof(CgButtonNativeBody84h, pagesAt50) == 0x50);
static_assert(offsetof(CgButtonNativeBody84h, pageButtonsAt5C) == 0x5C);
static_assert(offsetof(CgButtonNativeBody84h, unwrittenAt68) == 0x68);
static_assert(offsetof(CgButtonNativeBody84h, pageAt6C) == 0x6C);
static_assert(offsetof(CgButtonNativeBody84h, perPageAt70) == 0x70);
static_assert(offsetof(CgButtonNativeBody84h, categoryAt74) == 0x74);
static_assert(offsetof(CgButtonNativeBody84h, selectorAt78) == 0x78);
static_assert(sizeof(CgButtonNativeVtable15) == 0x3C);
static_assert(offsetof(CgButtonNativeVtable15, compositeAt38) == 0x38);

// 只抽象原调用叶。Root不是原21Ch布局，槽查询必须由宿主映射到唯一真实Ref。
// 不允许实现者把499DC0/4BEAF0/4ABE80等整段业务转发到现代group。
class ICgButtonNativeSource
{
public:
    virtual ~ICgButtonNativeSource() = default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t callSite) = 0;
    virtual InputRootNativeBody24h* CaptureInputAt418210(std::uint32_t callSite) = 0;
    // 对应原内联检查/发布/解锁后重读；必须映射同一Environment owner的发布槽。
    virtual EnvironmentRootStorage*& EnvironmentPublicationAt6198C8(std::uint32_t readSite) = 0;
    virtual void EnsureRootAt413BE0(std::uint32_t callSite) = 0;
    virtual void* RootAt6198C4(std::uint32_t readSite) = 0;
    virtual YtlReferenceSlotRuntime& RootDrawSlotAt18(void*) = 0;
    virtual YtlReferenceSlotRuntime& RootSoundSlotAt24(void*) = 0;
    virtual YtlReferenceSlotRuntime& RootScenesSlotAt30(void*) = 0;
    // 原480930已是公共原生装载入口；receiver是本次已捕获DrawManager，不额外Ensure。
    virtual std::uint8_t LoadButtonAt480930(void* receiver, CgUiButtonNativeBody6Ch*,
        const CgUiButtonRecordNative28h*, std::int32_t setPosition) = 0;
    virtual const char* CueNameAt(std::uint32_t originalGlobalAddress) = 0;
    virtual void PlayCueAt4BCD70(void* sound, const char*, std::int32_t channel,
        std::int32_t value, std::uint8_t checkType) = 0;
    // 4ABE80内联SurfaceDraw只抽导出叶；handle/各Ref资格与dirty循环在本模块判断。
    virtual void DrawPageAfter4182F0(GraphicsSurfaceNativeBody*, const RECT*) = 0;
    // 每矩形一次6198B8惰建/6010AC/InvalidatedRect，不允许批量整屏替代。
    virtual void InvalidatedRectAt4AC316(const RECT*) = 0;
};
struct CgButtonNativeServices final
{
    ModeButtonNativeServices& mode;
    // 必须与source返回的Environment owner使用同一601160锁；用于ctor内联惰建。
    IEnvironmentRootLock& environmentLock;
    ICgButtonNativeSource& source;
    IGuiControlResourceLoadServices& pictures;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsSurfaceRawCopyApi& copy;
    IRuntimeStringManagerSource& strings;
    ModeSelectButtonNativeServices& publicButtons;
    OptionCharSelectDialogNativeDependencies& selector;
};

// Main必须先建scope后持有Ref；所有逃逸Ref的最后一次释放仍须在同源scope内。
// scope不拥有84h/公共GUI/根，不把服务借用写入任何原control或正文。
class CgButtonNativeScope final
{
public:
    explicit CgButtonNativeScope(CgButtonNativeServices&) noexcept;
    ~CgButtonNativeScope() noexcept;
    CgButtonNativeScope(const CgButtonNativeScope&) = delete;
    CgButtonNativeScope& operator=(const CgButtonNativeScope&) = delete;
private:
    CgButtonNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    CgUiButtonNativeScope buttons_;
    ModeButtonNativeScope mode_;
    ModeSelectButtonNativeScope publicButtons_;
    OptionCharSelectDialogNativeScope selector_;
};
CgButtonNativeServices& RequireCgButtonNativeServices();
const CgButtonNativeVtable15& CgButtonNativeVtable() noexcept;
CgButtonNativeBody84h* ConstructCgButtonNativeAt499DC0(
    CgButtonNativeBody84h&, std::int32_t category, CgButtonNativeServices&);
void DestroyCgButtonNativeAt4A3C10(CgButtonNativeBody84h&) noexcept;
CgButtonNativeBody84h* DeleteCgButtonNativeAt4A6170(CgButtonNativeBody84h*, std::uint32_t) noexcept;
// fresh槽工厂供B4场景持有原Ref；构造catch正常返回仍有完整析构资格。
void ConstructCgButtonReferenceNative(YtlReferenceSlotRuntime& fresh,
    std::int32_t category, CgButtonNativeServices&);
std::int32_t ProcessCgButtonNativeAt4BEAF0(CgButtonNativeBody84h&);
std::int32_t ReadCgButtonInputNativeAt4A9FC0(CgButtonNativeBody84h&);
std::int32_t NavigateCgButtonNativeAt4B4BC0(CgButtonNativeBody84h&);
void DrawCgButtonNativeAt4ABE80(CgButtonNativeBody84h&);
void CompositeCgButtonNativeAt4A80B0(CgButtonNativeBody84h&, GraphicsSurfaceNativeBody*, const RECT*);
void ResetCgButtonNativeAt4C1260(CgButtonNativeBody84h&, std::int32_t invalidate);
}
