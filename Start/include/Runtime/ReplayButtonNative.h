#pragma once

#include <Runtime/CgButtonNative.h>

namespace KaGuYa::Start::Runtime
{
class ParamsRuntimeRoot;
struct ReplayButtonNativeBody84h;

// 原PE 5B529C..5B52D4：Replay独立15槽；+04..+38与ICgScBtnProc原表相同。
// 全部为thiscall；+08的RECT按值传入，+38的RECT按指针传入。
struct ReplayButtonNativeVtable15 final
{
    ReplayButtonNativeBody84h* (__thiscall* deleteAt00)(ReplayButtonNativeBody84h*, std::uint32_t);
    std::int32_t (__thiscall* processAt04)(ReplayButtonNativeBody84h*);
    std::int32_t (__thiscall* invalidateAt08)(ReplayButtonNativeBody84h*, RECT);
    std::uint8_t (__thiscall* releaseAt0C)(ReplayButtonNativeBody84h*);
    std::int32_t (__thiscall* inputAt10)(ReplayButtonNativeBody84h*);
    void (__thiscall* resetAllAt14)(ReplayButtonNativeBody84h*);
    std::int32_t (__thiscall* resetAt18)(ReplayButtonNativeBody84h*, std::int32_t);
    std::int32_t (__thiscall* focusAt1C)(ReplayButtonNativeBody84h*, std::int32_t);
    std::int32_t (__thiscall* pressAt20)(ReplayButtonNativeBody84h*, std::int32_t);
    std::int32_t (__thiscall* commitAt24)(ReplayButtonNativeBody84h*, std::int32_t);
    std::int32_t (__thiscall* selectAt28)(ReplayButtonNativeBody84h*, std::int32_t);
    std::int32_t (__thiscall* currentAt2C)(ReplayButtonNativeBody84h*);
    std::int32_t (__thiscall* navigateAt30)(ReplayButtonNativeBody84h*);
    void (__thiscall* drawAt34)(ReplayButtonNativeBody84h*);
    void (__thiscall* compositeAt38)(ReplayButtonNativeBody84h*, GraphicsSurfaceNativeBody*, const RECT*);
};

// 原RTTI：CModeReplayBtn -> ICgScBtnProc -> IModeBtnProc -> IBtnProc，全链this偏移为0。
// 与CCGdispBtn为兄弟类型；不能调用499DC0，也不能另造现代group保存正文状态。
struct ReplayButtonNativeBody84h final
{
    union { ModeButtonNativeBody50h common; };
    GuiControlSurfaceReferences pagesAt50;
    union { YtlReferenceSlotRuntime pageButtonsAt5C; };
    std::uint32_t unwrittenAt68;
    std::int32_t pageAt6C;
    std::int32_t perPageAt70;
    std::int32_t categoryAt74;
    union { YtlReferenceSlotRuntime selectorAt78; };

    // 49F270不写+68；+3C/+40/+6C/+70/+74按原站点晚写，禁止整块清零。
    ReplayButtonNativeBody84h() noexcept {}
    ~ReplayButtonNativeBody84h() noexcept {}
    ReplayButtonNativeBody84h(const ReplayButtonNativeBody84h&) = delete;
    ReplayButtonNativeBody84h& operator=(const ReplayButtonNativeBody84h&) = delete;
};
static_assert(sizeof(ReplayButtonNativeBody84h) == 0x84);
static_assert(alignof(ReplayButtonNativeBody84h) == 4);
static_assert(std::is_standard_layout_v<ReplayButtonNativeBody84h>);
static_assert(offsetof(ReplayButtonNativeBody84h, common) == 0x00);
static_assert(offsetof(ReplayButtonNativeBody84h, pagesAt50) == 0x50);
static_assert(offsetof(ReplayButtonNativeBody84h, pageButtonsAt5C) == 0x5C);
static_assert(offsetof(ReplayButtonNativeBody84h, unwrittenAt68) == 0x68);
static_assert(offsetof(ReplayButtonNativeBody84h, pageAt6C) == 0x6C);
static_assert(offsetof(ReplayButtonNativeBody84h, perPageAt70) == 0x70);
static_assert(offsetof(ReplayButtonNativeBody84h, categoryAt74) == 0x74);
static_assert(offsetof(ReplayButtonNativeBody84h, selectorAt78) == 0x78);
static_assert(sizeof(ReplayButtonNativeVtable15) == 0x3C);
static_assert(offsetof(ReplayButtonNativeVtable15, deleteAt00) == 0x00);
static_assert(offsetof(ReplayButtonNativeVtable15, processAt04) == 0x04);
static_assert(offsetof(ReplayButtonNativeVtable15, invalidateAt08) == 0x08);
static_assert(offsetof(ReplayButtonNativeVtable15, releaseAt0C) == 0x0C);
static_assert(offsetof(ReplayButtonNativeVtable15, inputAt10) == 0x10);
static_assert(offsetof(ReplayButtonNativeVtable15, resetAllAt14) == 0x14);
static_assert(offsetof(ReplayButtonNativeVtable15, resetAt18) == 0x18);
static_assert(offsetof(ReplayButtonNativeVtable15, focusAt1C) == 0x1C);
static_assert(offsetof(ReplayButtonNativeVtable15, pressAt20) == 0x20);
static_assert(offsetof(ReplayButtonNativeVtable15, commitAt24) == 0x24);
static_assert(offsetof(ReplayButtonNativeVtable15, selectAt28) == 0x28);
static_assert(offsetof(ReplayButtonNativeVtable15, currentAt2C) == 0x2C);
static_assert(offsetof(ReplayButtonNativeVtable15, navigateAt30) == 0x30);
static_assert(offsetof(ReplayButtonNativeVtable15, drawAt34) == 0x34);
static_assert(offsetof(ReplayButtonNativeVtable15, compositeAt38) == 0x38);

// ICgButtonNativeSource是原ICgScBtnProc公共调用叶合同，不是现代CG业务对象。
// 49F270新增四次独立Params捕获；返回真实30h正文，禁止缓存为一次或复制成影子数据。
class IReplayButtonNativeSource : public ICgButtonNativeSource
{
public:
    virtual const ParamsRuntimeRoot* CaptureParamsAt418480(std::uint32_t callSite) = 0;
};
struct ReplayButtonNativeServices final
{
    ModeButtonNativeServices& mode;
    // 与source的Environment owner共用601160锁及6198C8发布槽。
    IEnvironmentRootLock& environmentLock;
    IReplayButtonNativeSource& source;
    IGuiControlResourceLoadServices& pictures;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsSurfaceRawCopyApi& copy;
    IRuntimeStringManagerSource& strings;
    ModeSelectButtonNativeServices& publicButtons;
    OptionCharSelectDialogNativeDependencies& selector;
};

// 仅绑定原共享槽所需服务，不持有CgButton正文，也不调用CG构造器。
// Main须先建立scope；Replay/子按钮/Surface/公共GUI所有逃逸Ref先退役，再退役scope。
class ReplayButtonNativeScope final
{
public:
    explicit ReplayButtonNativeScope(ReplayButtonNativeServices&) noexcept;
    ~ReplayButtonNativeScope() noexcept;
    ReplayButtonNativeScope(const ReplayButtonNativeScope&) = delete;
    ReplayButtonNativeScope& operator=(const ReplayButtonNativeScope&) = delete;
    ReplayButtonNativeScope(ReplayButtonNativeScope&&) = delete;
    ReplayButtonNativeScope& operator=(ReplayButtonNativeScope&&) = delete;
private:
    ReplayButtonNativeServices* previous_;
    CgButtonNativeServices commonServices_;
    CgButtonNativeScope commonScope_;
};

ReplayButtonNativeServices& RequireReplayButtonNativeServices();
const ReplayButtonNativeVtable15& ReplayButtonNativeVtable() noexcept;
ReplayButtonNativeBody84h* ConstructReplayButtonNativeAt49F270(
    ReplayButtonNativeBody84h&, std::int32_t category, ReplayButtonNativeServices&);
// 原4A4C60先写ICgScBtnProc表，非虚正常析构和构造EH共用；释放异常必须terminate。
void DestroyReplayButtonNativeAt4A4C60(ReplayButtonNativeBody84h&) noexcept;
// 原4A6D70先写Replay表，再调用4A4C60；flags&1才sized-delete 84h。
ReplayButtonNativeBody84h* DeleteReplayButtonNativeAt4A6D70(
    ReplayButtonNativeBody84h*, std::uint32_t flags) noexcept;
// 原49EE76..49EEEF：fresh Ref12、84h单体、18h control，stride84h/count1。
// ctor catch正常返回的部分正文仍须发布原拥有Ref，不能伪装为构造失败或全新空对象。
void ConstructReplayButtonReferenceNative(YtlReferenceSlotRuntime& fresh,
    std::int32_t category, ReplayButtonNativeServices&);
std::int32_t ProcessReplayButtonNativeAt4BEAF0(ReplayButtonNativeBody84h&);
std::int32_t ReadReplayButtonInputNativeAt4A9FC0(ReplayButtonNativeBody84h&);
std::int32_t NavigateReplayButtonNativeAt4B4BC0(ReplayButtonNativeBody84h&);
void DrawReplayButtonNativeAt4ABE80(ReplayButtonNativeBody84h&);
void CompositeReplayButtonNativeAt4A80B0(
    ReplayButtonNativeBody84h&, GraphicsSurfaceNativeBody*, const RECT*);
void ResetReplayButtonNativeAt4C1260(ReplayButtonNativeBody84h&, std::int32_t invalidate);
}
