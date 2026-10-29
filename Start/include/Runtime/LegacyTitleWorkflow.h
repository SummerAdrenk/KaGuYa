#pragma once

#include <Runtime/DrawTransitionView.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/SceneManagerView.h>
#include <Runtime/TransitRuntime.h>
#include <Runtime/UnSaveDataRuntime.h>

namespace KaGuYa::Start::Runtime
{
// Baku 49DFD0：共享协调状态，不是原 CTitle 布局，也不拥有菜单/Surface/根对象。
// owner 在 49C770 对应的资源构造、合成及 common 初始化全部成功后才运行它。
struct LegacyTitleWorkflowState final
{
    std::int32_t phase = 0;
    TransitOperation operation;
    std::int32_t step = 0;
};

struct LegacyTitleWorkflowResources final
{
    YtlReferenceSlotRuntime& frame;
    const YtlReferenceSlotRuntime& black;
};

class ILegacyTitleMenu
{
public:
    virtual ~ILegacyTitleMenu() = default;
    // 49DC30：旧六项菜单返回索引，不是现代 TitleDialog 的 event 3..8。
    // 实现负责输入释放、逐项扫描、cue 和 overlay；不能直接套 ModeButton 总 poll。
    virtual std::int32_t PollSelection() = 0;
    // 49E790：仅消费菜单自己的 dirty，逐 rect 合成背景/叠层/按钮并 commit，最后 clear。
    virtual void FlushDirty() = 0;
};

class ILegacyTitleAudio
{
public:
    virtual ~ILegacyTitleAudio() = default;
    // 49E049..49E09F：先取 registry1 参数音量，再取标题名，再调用播放。
    virtual std::int32_t ReadBgmVolume() = 0;
    virtual const char* TitleBgmName() = 0;
    // 4C8D30：先写当前 BGM 名，再播放；由真实 Sound owner 适配，返回值不门控 phase。
    virtual std::uint8_t PlayBgm(const char* nameCp932, std::int32_t volume, std::int32_t loop) = 0;
    virtual std::uint8_t ClearSound() = 0;
    // 49E0B7..49E0D7：使用当时 registry2 的声音名容器，不是标题私有清单。
    virtual void StopCommonSounds() = 0;
};

class ILegacyTitleNamePrompt
{
public:
    virtual ~ILegacyTitleNamePrompt() = default;
    // 49E288..49E2DA：每次重新取 GameSystem；bit0 真短路 bit1，然后才读统一 SaveData。
    virtual bool EditableNameBit(std::uint32_t bit) = 0;
    virtual bool SkipNamePrompt() = 0;
    // 49E2DC..49E34D：构造旧 ID112 -> IME(true) -> gate(0) -> DialogBox
    // -> gate(1) -> IME(false) -> 析构；返回 1/2/-1。异常仅展开 dialog，不补 gate/IME 恢复。
    // 必须实现旧位门控验证和 skip/given/family 提交；现代 GUI 的 6/7 不能仅换码冒充。
    virtual std::int32_t RunModal() = 0;
};

class ILegacyTitleQuickLoad
{
public:
    virtual ~ILegacyTitleQuickLoad() = default;
    // 44FA10/44FA90：返回码保持原值，不把 prepare==1 当作已恢复存档。
    virtual std::int32_t Prepare() = 0;
    virtual std::int32_t Poll() = 0;
    // 44FB20：包含读取器释放及失败提示；false 留在标题，异常外传。
    virtual bool Load() = 0;
    // 46A5B0 -> 4C9960：借当前消息框唯一背景 Ref 槽，消息框不存在才返回 nullptr。
    // 不创建消息框，不额外增持；复制/赋值/释放由工作流执行。不是裸 surface handle。
    virtual YtlReferenceSlotRuntime* MessageBackgroundSlot() = 0;
};

class ILegacyTitleWorkflowHost
{
public:
    virtual ~ILegacyTitleWorkflowHost() = default;
    // 与 TitleSceneNative 的根捕获相同：每次选择当前 owner，不缓存上一代 publication。
    virtual SceneManagerView CaptureScenes() = 0;
    virtual UnSaveDataRuntime& CaptureCommon() = 0;
    virtual bool Pressed(std::int32_t action) = 0;
    virtual bool Held(std::int32_t action) = 0;
    virtual bool MousePressedEdge(std::int32_t button) = 0;
    virtual std::int32_t ReplayValue() = 0;
    // 4C9190：向空 output 交付一个持有引用；正常创建失败交付空 Ref，不伪造截图。
    // 抛出前由 callee 处理自身未交付资源，成功返回后才建立 caller 的临时释放资格。
    virtual void CaptureFrame(YtlReferenceSlotRuntime& output) = 0;
    virtual DrawTransitionView CaptureDraw() = 0;
    virtual std::uint8_t LoadStartupScript(const char* nameCp932, std::int32_t argument) = 0;
    // 49E4D8/49E4E6/49E5AB/49E3A8 是四个不同的语义端口，不传原对象偏移。
    virtual void WriteStartupFlag(std::uint8_t value) = 0;
    virtual void WriteStartupReadyFlag(std::uint8_t value) = 0;
    virtual void WriteLoadEntryFlag(std::uint8_t value) = 0;
    virtual void WriteSpecialMenuFlag(std::uint8_t value) = 0;
    virtual std::int32_t PostWindowMessage(std::uint32_t message,
        std::uint32_t wparam, std::int32_t lparam) = 0;
};

struct LegacyTitleWorkflowServices final
{
    ILegacyTitleWorkflowHost& host;
    ILegacyTitleMenu& menu;
    ILegacyTitleAudio& audio;
    ILegacyTitleNamePrompt& names;
    ILegacyTitleQuickLoad& quickLoad;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsScreenCalls& screen;
};

// 49DFD0 / 4C8C30：一次 Update，无完成/成功返回协议，Draw 为空。
// Queue 会移除 active；调度者必须持局部 scene Ref 到本调用返回（包括随后 root 写入）。
// 不负责原 ctor/dtor、工厂注册或服务装配；禁止将现代 D8h/1Ch 正文强转为此状态。
void UpdateLegacyTitleWorkflow(LegacyTitleWorkflowState& state,
    LegacyTitleWorkflowResources resources, LegacyTitleWorkflowServices& services);
}
